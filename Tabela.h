#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "Pagina.h"
#include "DomainTypes.h"

class Tabela {
public:
    std::string nomeArquivo;
    std::vector<sgbd::Coluna> colunas;
    std::vector<Pagina> paginas;
    int numPaginas;
    int numColunas;

    Tabela(const std::string& nomeArquivo) : nomeArquivo(nomeArquivo) {
        std::ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            throw std::runtime_error("Erro ao abrir arquivo: " + nomeArquivo);
        }

        // Lê o cabeçalho
        std::string linha;
        std::getline(arquivo, linha);
        std::stringstream ss(linha);
        std::string coluna;
        
        // Define os tipos das colunas baseado no nome do arquivo
        std::vector<sgbd::TipoColuna> tipos;
        if (nomeArquivo == "vinho.csv") {
            tipos = {
                sgbd::TipoColuna::ID_UVA,
                sgbd::TipoColuna::ID_PAIS_PRODUCAO,
                sgbd::TipoColuna::NOME,
                sgbd::TipoColuna::DESCRICAO
            };
        } else if (nomeArquivo == "uva.csv") {
            tipos = {
                sgbd::TipoColuna::ID_UVA,
                sgbd::TipoColuna::ID_PAIS_ORIGEM,
                sgbd::TipoColuna::NOME,
                sgbd::TipoColuna::DESCRICAO
            };
        } else if (nomeArquivo == "pais.csv") {
            tipos = {
                sgbd::TipoColuna::ID_PAIS,
                sgbd::TipoColuna::NOME,
                sgbd::TipoColuna::REGIAO
            };
        }

        int i = 0;
        while (std::getline(ss, coluna, ',')) {
            if (i < tipos.size()) {
                colunas.emplace_back(coluna, tipos[i]);
            }
            i++;
        }
        numColunas = colunas.size();

        // Lê as tuplas
        Pagina pagina;
        while (std::getline(arquivo, linha)) {
            Tupla tupla(linha, tipos);
            pagina.adicionarTupla(tupla);
            if (pagina.cheia()) {
                paginas.push_back(pagina);
                pagina = Pagina();
            }
        }
        if (!pagina.vazia()) {
            paginas.push_back(pagina);
        }
        numPaginas = paginas.size();
    }

    int getIndiceColuna(const std::string& nome) const {
        for (size_t i = 0; i < colunas.size(); ++i) {
            if (colunas[i].getNome() == nome) {
                return i;
            }
        }
        return -1;
    }

    sgbd::TipoColuna getTipoColuna(const std::string& nome) const {
        for (const auto& col : colunas) {
            if (col.getNome() == nome) {
                return col.getTipo();
            }
        }
        throw std::runtime_error("Coluna não encontrada: " + nome);
    }
};
