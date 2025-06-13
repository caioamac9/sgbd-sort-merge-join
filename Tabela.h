
#ifndef TABELA_H
#define TABELA_H

#include "Pagina.h"
#include <fstream>
#include <string>

class Tabela {
public:
    std::string nomeArquivo;
    std::vector<std::string> colunas;
    Tabela(const std::string& nomeArquivo);

    void carregarCabecalho();
    int getIndiceColuna(const std::string& nome) const;
};

#endif
