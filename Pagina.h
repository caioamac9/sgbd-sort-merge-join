#ifndef PAGINA_H
#define PAGINA_H

#include "Tupla.h"
#include <vector>
#include <string>

#define MAX_TUPLAS_POR_PAGINA 4

class Pagina {
private:
    bool modificada = false;

public:
    std::vector<Tupla> tuplas;
    bool cheia() const;
    void adicionarTupla(const Tupla& t);
    std::string serializar() const;
    void limpar();
    void marcarModificada();
    bool estaModificada() const;
};

#endif
