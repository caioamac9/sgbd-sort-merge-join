#include "ExternalSorter.h"
#include "Tupla.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <queue>

#define PAGE_SIZE 10
#define BUFFER_PAGES 4
#define MERGE_FANIN (BUFFER_PAGES - 1)

void ExternalSorter::sort(const std::string& nomeArquivo, int colIndex) {
    std::ifstream arq(nomeArquivo);
    std::string header;
    std::getline(arq, header);

    // Fase 1: Criação dos runs
    int runCount = 0;
    std::vector<Tupla> buffer;
    std::string linha;
    std::vector<std::string> runFiles;

    while (std::getline(arq, linha)) {
        buffer.emplace_back(linha);
        if (buffer.size() == PAGE_SIZE) {
            std::sort(buffer.begin(), buffer.end(), [colIndex](const Tupla& a, const Tupla& b) {
                try {
                    int valA = std::stoi(a.getAtributo(colIndex));
                    int valB = std::stoi(b.getAtributo(colIndex));
                    return valA < valB;
                } catch (...) {
                    return a.getAtributo(colIndex) < b.getAtributo(colIndex);
                }
            });
            std::string runName = "temp_run_" + std::to_string(runCount) + ".csv";
            std::ofstream runFile(runName);
            runFile << header << "\n";
            for (const auto& t : buffer) {
                runFile << t.toString() << "\n";
            }
            runFiles.push_back(runName);
            runCount++;
            buffer.clear();
        }
    }
    // Salvar último run se houver
    if (!buffer.empty()) {
        std::sort(buffer.begin(), buffer.end(), [colIndex](const Tupla& a, const Tupla& b) {
            try {
                int valA = std::stoi(a.getAtributo(colIndex));
                int valB = std::stoi(b.getAtributo(colIndex));
                return valA < valB;
            } catch (...) {
                return a.getAtributo(colIndex) < b.getAtributo(colIndex);
            }
        });
        std::string runName = "temp_run_" + std::to_string(runCount) + ".csv";
        std::ofstream runFile(runName);
        runFile << header << "\n";
        for (const auto& t : buffer) {
            runFile << t.toString() << "\n";
        }
        runFiles.push_back(runName);
        runCount++;
        buffer.clear();
    }

    // Fase 2: Merge dos runs (k-way merge, k=MERGE_FANIN)
    int mergePass = 0;
    while (runFiles.size() > 1) {
        std::vector<std::string> newRunFiles;
        for (size_t i = 0; i < runFiles.size(); i += MERGE_FANIN) {
            // Abrir até MERGE_FANIN runs
            std::vector<std::ifstream> runStreams;
            std::vector<std::string> currentLines;
            std::vector<bool> hasLine;
            for (size_t j = 0; j < MERGE_FANIN && (i + j) < runFiles.size(); ++j) {
                runStreams.emplace_back(runFiles[i + j]);
                std::string tempHeader;
                std::getline(runStreams[j], tempHeader); // pula header
                std::string line;
                if (std::getline(runStreams[j], line)) {
                    currentLines.push_back(line);
                    hasLine.push_back(true);
                } else {
                    currentLines.push_back("");
                    hasLine.push_back(false);
                }
            }
            // Nome do novo run
            std::string newRunName = "temp_run_merge_" + std::to_string(mergePass) + "_" + std::to_string(i / MERGE_FANIN) + ".csv";
            std::ofstream out(newRunName);
            out << header << "\n";
            // Página de saída
            std::vector<Tupla> outputPage;
            while (true) {
                // Encontrar o menor elemento entre os runs ativos
                int minIdx = -1;
                for (size_t j = 0; j < currentLines.size(); ++j) {
                    if (hasLine[j]) {
                        if (minIdx == -1 ||
                            ([&]() {
                                try {
                                    int valA = std::stoi(Tupla(currentLines[j]).getAtributo(colIndex));
                                    int valB = std::stoi(Tupla(currentLines[minIdx]).getAtributo(colIndex));
                                    return valA < valB;
                                } catch (...) {
                                    return Tupla(currentLines[j]).getAtributo(colIndex) < Tupla(currentLines[minIdx]).getAtributo(colIndex);
                                }
                            })()) {
                            minIdx = j;
                        }
                    }
                }
                if (minIdx == -1) break; // Todos os runs acabaram
                outputPage.push_back(Tupla(currentLines[minIdx]));
                if (outputPage.size() == PAGE_SIZE) {
                    for (const auto& t : outputPage) {
                        out << t.toString() << "\n";
                    }
                    outputPage.clear();
                }
                // Avançar o run de onde saiu o menor
                if (std::getline(runStreams[minIdx], currentLines[minIdx])) {
                    hasLine[minIdx] = true;
                } else {
                    hasLine[minIdx] = false;
                }
            }
            // Salvar tuplas restantes
            for (const auto& t : outputPage) {
                out << t.toString() << "\n";
            }
            newRunFiles.push_back(newRunName);
        }
        // Remover arquivos antigos
        for (const auto& f : runFiles) std::remove(f.c_str());
        runFiles = newRunFiles;
        mergePass++;
    }
    // Renomear o último run para sorted_nomeArquivo
    if (!runFiles.empty()) {
        std::rename(runFiles[0].c_str(), ("sorted_" + nomeArquivo).c_str());
    }
}
