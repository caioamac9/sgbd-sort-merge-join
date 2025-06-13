#include "Pagina.h"

bool Pagina::cheia() const {
    return tuplas.size() >= MAX_TUPLAS_POR_PAGINA;
}

void Pagina::adicionarTupla(const Tupla& t) {
    tuplas.push_back(t);
    modificada = true;
}

std::string Pagina::serializar() const {
    std::string result;
    for (const auto& t : tuplas) {
        result += t.toString() + "\n";
    }
    return result;
}

void Pagina::limpar() {
    tuplas.clear();
    modificada = false;
}

void Pagina::marcarModificada() {
    modificada = true;
}

bool Pagina::estaModificada() const {
    return modificada;
}
