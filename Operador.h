
#ifndef OPERADOR_H
#define OPERADOR_H

#include "Tabela.h"
#include "Tupla.h"
#include "Pagina.h"
#include <vector>
#include <string>

class Operador {
private:
    Tabela t1, t2;
    std::vector<Pagina> paginasResultado;
    int io_count = 0;
    int pag_count = 0;
    std::string col1, col2;

public:
    Operador(Tabela t1, Tabela t2, std::string col1, std::string col2);
    void executar();
    int numTuplasGeradas() const;
    int numIOExecutados() const;
    int numPagsGeradas() const;
    void salvarTuplasGeradas(const std::string& arquivoSaida) const;
};

#endif
