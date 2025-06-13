#include "Operador.h"
#include "ExternalSorter.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>

#define PAGE_SIZE 10
#define BUFFER_PAGES 4

Operador::Operador(Tabela t1, Tabela t2, std::string col1, std::string col2)
    : t1(t1), t2(t2), col1(col1), col2(col2) {
    // Verifica compatibilidade das colunas
    sgbd::TipoColuna tipo1 = t1.getTipoColuna(col1);
    sgbd::TipoColuna tipo2 = t2.getTipoColuna(col2);
    
    if (tipo1 != tipo2) {
        throw std::runtime_error("Colunas incompatíveis para join: " + col1 + " e " + col2);
    }

    int idx1 = t1.getIndiceColuna(col1);
    int idx2 = t2.getIndiceColuna(col2);
    std::cout << "Ordenando tabela " << t1.nomeArquivo << " pela coluna " << col1 << std::endl;
    ExternalSorter::sort(t1.nomeArquivo, idx1);
    std::cout << "Ordenando tabela " << t2.nomeArquivo << " pela coluna " << col2 << std::endl;
    ExternalSorter::sort(t2.nomeArquivo, idx2);
}

void Operador::executar() {
    std::ifstream f1("sorted_" + t1.nomeArquivo);
    std::ifstream f2("sorted_" + t2.nomeArquivo);
    std::string header1, header2;
    std::getline(f1, header1);
    std::getline(f2, header2);

    // Buffer de páginas (4 páginas no total)
    std::vector<Tupla> buffer1;  // 2 páginas para tabela 1
    std::vector<Tupla> buffer2;  // 1 página para tabela 2
    std::vector<Tupla> resultBuffer;  // 1 página para resultados

    std::string linha1, linha2;
    bool has1 = true, has2 = true;
    int matchCount = 0;
    int totalTuplas1 = 0;
    int totalTuplas2 = 0;
    int currentKey1 = -1, currentKey2 = -1;
    Pagina pagina;
    io_count = 0;
    pag_count = 0;

    // Função para carregar página da tabela 1
    auto loadPage1 = [&]() {
        if (buffer1.size() >= PAGE_SIZE * 2) return;  // Buffer cheio
        while (buffer1.size() < PAGE_SIZE * 2 && has1) {
            if (std::getline(f1, linha1)) {
                buffer1.emplace_back(linha1, std::vector<sgbd::TipoColuna>{
                    sgbd::TipoColuna::ID_UVA,
                    sgbd::TipoColuna::ID_PAIS_PRODUCAO,
                    sgbd::TipoColuna::NOME,
                    sgbd::TipoColuna::DESCRICAO
                });
                totalTuplas1++;
            } else {
                has1 = false;
            }
        }
        if (!buffer1.empty()) io_count++;  // Leitura de página
    };

    // Função para carregar página da tabela 2
    auto loadPage2 = [&]() {
        if (buffer2.size() >= PAGE_SIZE) return;  // Buffer cheio
        while (buffer2.size() < PAGE_SIZE && has2) {
            if (std::getline(f2, linha2)) {
                buffer2.emplace_back(linha2, std::vector<sgbd::TipoColuna>{
                    sgbd::TipoColuna::ID_UVA,
                    sgbd::TipoColuna::ID_PAIS_ORIGEM,
                    sgbd::TipoColuna::NOME,
                    sgbd::TipoColuna::DESCRICAO
                });
                totalTuplas2++;
            } else {
                has2 = false;
            }
        }
        if (!buffer2.empty()) io_count++;  // Leitura de página
    };

    // Função para escrever página de resultados
    auto writeResultPage = [&]() {
        if (!resultBuffer.empty()) {
            for (const auto& t : resultBuffer) {
                pagina.adicionarTupla(t);
                if (pagina.cheia()) {
                    paginasResultado.push_back(pagina);
                    pagina = Pagina();
                    pag_count++;
                    io_count++;  // Escrita de página
                }
            }
            resultBuffer.clear();
        }
    };

    // Carregar páginas iniciais
    loadPage1();
    loadPage2();

    while (has1 || !buffer1.empty() || has2 || !buffer2.empty()) {
        // Encontrar próxima chave em cada buffer
        currentKey1 = -1;
        currentKey2 = -1;
        
        for (const auto& t : buffer1) {
            try {
                currentKey1 = std::stoi(t.getAtributo(t1.getIndiceColuna(col1)));
                break;
            } catch (...) {
                // Ignora erros de conversão
            }
        }
        
        for (const auto& t : buffer2) {
            try {
                currentKey2 = std::stoi(t.getAtributo(t2.getIndiceColuna(col2)));
                break;
            } catch (...) {
                // Ignora erros de conversão
            }
        }

        // Se não encontrou chaves, carrega mais páginas
        if (currentKey1 == -1 && has1) {
            loadPage1();
            continue;
        }
        if (currentKey2 == -1 && has2) {
            loadPage2();
            continue;
        }

        // Se um dos buffers está vazio e não há mais dados, termina
        if (buffer1.empty() && !has1 && buffer2.empty() && !has2) {
            break;
        }

        // Comparar chaves
        if (currentKey1 < currentKey2 || currentKey2 == -1) {
            // Avançar na tabela 1
            if (!buffer1.empty()) {
                buffer1.erase(buffer1.begin());
                if (buffer1.size() < PAGE_SIZE && has1) loadPage1();
            }
        } else if (currentKey1 > currentKey2 || currentKey1 == -1) {
            // Avançar na tabela 2
            if (!buffer2.empty()) {
                buffer2.erase(buffer2.begin());
                if (buffer2.size() < PAGE_SIZE && has2) loadPage2();
            }
        } else {
            // Chaves iguais - fazer join
            std::vector<Tupla> matches1, matches2;
            
            // Coletar todas as tuplas com a mesma chave em buffer1
            while (!buffer1.empty()) {
                try {
                    int key = std::stoi(buffer1[0].getAtributo(t1.getIndiceColuna(col1)));
                    if (key == currentKey1) {
                        matches1.push_back(buffer1[0]);
                        buffer1.erase(buffer1.begin());
                        if (buffer1.size() < PAGE_SIZE && has1) loadPage1();
                    } else {
                        break;
                    }
                } catch (...) {
                    buffer1.erase(buffer1.begin());
                    if (buffer1.size() < PAGE_SIZE && has1) loadPage1();
                }
            }

            // Coletar todas as tuplas com a mesma chave em buffer2
            while (!buffer2.empty()) {
                try {
                    int key = std::stoi(buffer2[0].getAtributo(t2.getIndiceColuna(col2)));
                    if (key == currentKey2) {
                        matches2.push_back(buffer2[0]);
                        buffer2.erase(buffer2.begin());
                        if (buffer2.size() < PAGE_SIZE && has2) loadPage2();
                    } else {
                        break;
                    }
                } catch (...) {
                    buffer2.erase(buffer2.begin());
                    if (buffer2.size() < PAGE_SIZE && has2) loadPage2();
                }
            }

            // Fazer o join entre as tuplas coletadas
            for (const auto& t1 : matches1) {
                for (const auto& t2 : matches2) {
                    resultBuffer.emplace_back(Tupla(t1, t2));
                    matchCount++;
                    
                    if (resultBuffer.size() == PAGE_SIZE) {
                        writeResultPage();
                    }
                }
            }
        }
    }

    // Escrever resultados restantes
    writeResultPage();
    if (!pagina.tuplas.empty()) {
        paginasResultado.push_back(pagina);
        pag_count++;
        io_count++;  // Escrita de página
    }

    // Imprimir estatísticas
    std::cout << "Estatísticas da junção:" << std::endl;
    std::cout << "Total de tuplas na tabela 1: " << totalTuplas1 << std::endl;
    std::cout << "Total de tuplas na tabela 2: " << totalTuplas2 << std::endl;
    std::cout << "Total de correspondências encontradas: " << matchCount << std::endl;
    std::cout << "Total de páginas geradas: " << pag_count << std::endl;
    std::cout << "Total de operações de I/O: " << io_count << std::endl;
}

int Operador::numTuplasGeradas() const {
    int total = 0;
    for (const auto& p : paginasResultado) {
        total += p.tuplas.size();
    }
    return total;
}

int Operador::numIOExecutados() const {
    return io_count;
}

int Operador::numPagsGeradas() const {
    return pag_count;
}

void Operador::salvarTuplasGeradas(const std::string& arquivoSaida) {
    std::ofstream out(arquivoSaida);
    if (!out.is_open()) {
        throw std::runtime_error("Erro ao abrir arquivo para escrita: " + arquivoSaida);
    }

    // Escreve o cabeçalho
    for (const auto& col : t1.colunas) {
        out << col.getNome() << ",";
    }
    for (const auto& col : t2.colunas) {
        out << col.getNome() << ",";
    }
    out << std::endl;

    // Escreve as tuplas
    for (const auto& pagina : paginasResultado) {
        for (const auto& tupla : pagina.tuplas) {
            out << tupla.serializar() << std::endl;
        }
    }
}
