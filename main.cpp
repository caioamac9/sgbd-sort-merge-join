#include "Tabela.h"
#include "Operador.h"
#include <iostream>

int main() {
    Tabela vinho("vinho.csv");
    Tabela uva("uva.csv");
    Tabela pais("pais.csv");

    // Primeiro teste: Vinho ⋈ Uva
    Operador op1(vinho, uva, "uva_id", "uva_id");
    op1.executar();
    std::cout << "[Vinho ⋈ Uva] Pags: " << op1.numPagsGeradas()
              << " IOs: " << op1.numIOExecutados()
              << " Tuplas: " << op1.numTuplasGeradas() << std::endl;
    op1.salvarTuplasGeradas("resultado_vinho_uva.csv");

    // Segundo teste: Uva ⋈ Vinho (ordem inversa)
    Operador op1_inverso(uva, vinho, "uva_id", "uva_id");
    op1_inverso.executar();
    std::cout << "[Uva ⋈ Vinho] Pags: " << op1_inverso.numPagsGeradas()
              << " IOs: " << op1_inverso.numIOExecutados()
              << " Tuplas: " << op1_inverso.numTuplasGeradas() << std::endl;
    op1_inverso.salvarTuplasGeradas("resultado_uva_vinho.csv");

    Operador op2(vinho, pais, "pais_producao_id", "pais_id");
    op2.executar();
    std::cout << "[Vinho ⋈ Pais] Pags: " << op2.numPagsGeradas()
              << " IOs: " << op2.numIOExecutados()
              << " Tuplas: " << op2.numTuplasGeradas() << std::endl;
    op2.salvarTuplasGeradas("resultado_vinho_pais.csv");

    Operador op3(uva, pais, "pais_origem_id", "pais_id");
    op3.executar();
    std::cout << "[Uva ⋈ Pais] Pags: " << op3.numPagsGeradas()
              << " IOs: " << op3.numIOExecutados()
              << " Tuplas: " << op3.numTuplasGeradas() << std::endl;
    op3.salvarTuplasGeradas("resultado_uva_pais.csv");

    return 0;
}
