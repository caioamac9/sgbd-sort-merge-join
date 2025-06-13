#pragma once

#include "Tabela.h"
#include "DomainTypes.h"
#include <string>
#include <vector>

class Operador {
private:
    Tabela t1, t2;
    std::string col1, col2;
    std::vector<Pagina> paginasResultado;
    int io_count;
    int pag_count;

public:
    Operador(Tabela t1, Tabela t2, std::string col1, std::string col2);
    void executar();
    void salvarTuplasGeradas(const std::string& arquivoSaida);
    int numIOExecutados() const { return io_count; }
    int numPagsGeradas() const { return pag_count; }
    int numTuplasGeradas() const {
        int total = 0;
        for (const auto& p : paginasResultado) {
            total += p.numTuplas;
        }
        return total;
    }
};
