#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "DomainTypes.h"

class Tupla {
private:
    std::vector<sgbd::ValorColuna> valores;

public:
    Tupla() = default;

    // Construtor a partir de uma linha CSV
    Tupla(const std::string& linha, const std::vector<sgbd::TipoColuna>& tipos) {
        std::stringstream ss(linha);
        std::string valor;
        int i = 0;
        
        while (std::getline(ss, valor, ',')) {
            if (i < tipos.size()) {
                try {
                    int idValor = std::stoi(valor);
                    valores.emplace_back(sgbd::criarIdValor(idValor, tipos[i]), tipos[i]);
                } catch (...) {
                    // Se não for um ID, trata como string
                    valores.emplace_back(nullptr, tipos[i]);
                }
            }
            i++;
        }
    }

    // Construtor para join
    Tupla(const Tupla& t1, const Tupla& t2) {
        valores.insert(valores.end(), t1.valores.begin(), t1.valores.end());
        valores.insert(valores.end(), t2.valores.begin(), t2.valores.end());
    }

    std::string getAtributo(int idx) const {
        if (idx >= 0 && idx < valores.size()) {
            return valores[idx].toString();
        }
        return "";
    }

    bool operator==(const Tupla& other) const {
        if (valores.size() != other.valores.size()) return false;
        for (size_t i = 0; i < valores.size(); ++i) {
            if (!(valores[i] == other.valores[i])) return false;
        }
        return true;
    }

    bool operator<(const Tupla& other) const {
        if (valores.empty() || other.valores.empty()) return false;
        return valores[0] < other.valores[0];
    }

    std::string serializar() const {
        std::stringstream ss;
        for (size_t i = 0; i < valores.size(); ++i) {
            ss << valores[i].toString();
            if (i < valores.size() - 1) ss << ",";
        }
        return ss.str();
    }
};
