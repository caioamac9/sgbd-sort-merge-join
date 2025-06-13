
#include "Tupla.h"
#include <sstream>

Tupla::Tupla() {}

Tupla::Tupla(const std::string& linha_csv) {
    std::stringstream ss(linha_csv);
    std::string item;
    while (std::getline(ss, item, ',')) {
        atributos.push_back(item);
    }
}

Tupla::Tupla(const Tupla& a, const Tupla& b) {
    atributos = a.atributos;
    atributos.insert(atributos.end(), b.atributos.begin(), b.atributos.end());
}

std::string Tupla::getAtributo(int i) const {
    return atributos[i];
}

std::string Tupla::toString() const {
    std::string out;
    for (int i = 0; i < atributos.size(); ++i) {
        out += atributos[i];
        if (i != atributos.size() - 1) out += ",";
    }
    return out;
}
