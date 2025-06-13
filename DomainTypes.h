#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <memory>

namespace sgbd {

// Enumeração para os tipos de colunas
enum class TipoColuna {
    ID_UVA,
    ID_PAIS,
    ID_PAIS_PRODUCAO,
    ID_PAIS_ORIGEM,
    NOME,
    REGIAO,
    DESCRICAO
};

// Classe base para valores de domínio
class ValorDominio {
public:
    virtual ~ValorDominio() = default;
    virtual std::string toString() const = 0;
    virtual bool operator==(const ValorDominio& other) const = 0;
    virtual bool operator<(const ValorDominio& other) const = 0;
};

// Classe para valores de ID
class IdValor : public ValorDominio {
private:
    int valor;
    TipoColuna tipo;

public:
    IdValor(int valor, TipoColuna tipo) : valor(valor), tipo(tipo) {}

    std::string toString() const override {
        return std::to_string(valor);
    }

    bool operator==(const ValorDominio& other) const override {
        const IdValor* otherId = dynamic_cast<const IdValor*>(&other);
        if (!otherId) return false;
        return valor == otherId->valor && tipo == otherId->tipo;
    }

    bool operator<(const ValorDominio& other) const override {
        const IdValor* otherId = dynamic_cast<const IdValor*>(&other);
        if (!otherId) return false;
        if (tipo != otherId->tipo) return false;
        return valor < otherId->valor;
    }

    TipoColuna getTipo() const { return tipo; }
    int getValor() const { return valor; }
};

// Classe para representar uma coluna
class Coluna {
private:
    std::string nome;
    TipoColuna tipo;

public:
    Coluna(const std::string& nome, TipoColuna tipo) : nome(nome), tipo(tipo) {}

    const std::string& getNome() const { return nome; }
    TipoColuna getTipo() const { return tipo; }

    bool operator==(const Coluna& other) const {
        return tipo == other.tipo;
    }
};

// Classe para representar um valor de coluna
class ValorColuna {
private:
    std::shared_ptr<ValorDominio> valor;
    TipoColuna tipo;

public:
    ValorColuna(std::shared_ptr<ValorDominio> valor, TipoColuna tipo) 
        : valor(valor), tipo(tipo) {}

    std::string toString() const {
        return valor ? valor->toString() : "";
    }

    bool operator==(const ValorColuna& other) const {
        if (!valor || !other.valor) return false;
        return *valor == *other.valor;
    }

    bool operator<(const ValorColuna& other) const {
        if (!valor || !other.valor) return false;
        return *valor < *other.valor;
    }

    TipoColuna getTipo() const { return tipo; }
};

// Função auxiliar para criar um valor de ID
std::shared_ptr<ValorDominio> criarIdValor(int valor, TipoColuna tipo) {
    return std::make_shared<IdValor>(valor, tipo);
}

// Função para verificar compatibilidade de colunas para join
bool colunasSaoCompativeisParaJoin(const Coluna& col1, const Coluna& col2) {
    return col1 == col2;
}

} // namespace sgbd 