
#ifndef TUPLA_H
#define TUPLA_H

#include <vector>
#include <string>

class Tupla {
public:
    std::vector<std::string> atributos;

    Tupla();
    Tupla(const std::string& linha_csv);
    Tupla(const Tupla& a, const Tupla& b);
    std::string getAtributo(int i) const;
    std::string toString() const;
};

#endif
