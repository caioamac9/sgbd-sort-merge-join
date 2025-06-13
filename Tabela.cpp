
#include "Tabela.h"
#include <sstream>
#include <stdexcept>

Tabela::Tabela(const std::string& nomeArquivo) : nomeArquivo(nomeArquivo) {
    carregarCabecalho();
}

void Tabela::carregarCabecalho() {
    std::ifstream arq(nomeArquivo);
    std::string header;
    if (std::getline(arq, header)) {
        std::stringstream ss(header);
        std::string col;
        while (std::getline(ss, col, ',')) {
            colunas.push_back(col);
        }
    }
}

int Tabela::getIndiceColuna(const std::string& nome) const {
    for (int i = 0; i < colunas.size(); ++i) {
        if (colunas[i] == nome) return i;
    }
    throw std::runtime_error("Coluna nao encontrada: " + nome);
}
