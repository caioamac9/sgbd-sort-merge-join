#include "BufferManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

BufferManager::BufferManager() : currentPosition(0) {
    buffer.resize(BUFFER_SIZE);
}

std::string BufferManager::makeKey(const std::string& filename, int pageNumber) {
    return filename + "::" + std::to_string(pageNumber);
}

std::pair<std::string, int> BufferManager::parseKey(const std::string& key) {
    size_t pos = key.find("::");
    if (pos == std::string::npos) {
        throw std::runtime_error("Formato de chave inválido: " + key);
    }
    std::string filename = key.substr(0, pos);
    std::string pageStr = key.substr(pos + 2);
    try {
        int pageNum = std::stoi(pageStr);
        return {filename, pageNum};
    } catch (const std::exception& e) {
        throw std::runtime_error("Erro ao processar número da página: " + key);
    }
}

Pagina& BufferManager::getPage(const std::string& filename, int pageNumber) {
    std::string key = makeKey(filename, pageNumber);
    
    // Verifica se a página já está no buffer
    auto it = paginaToBuffer.find(key);
    if (it != paginaToBuffer.end()) {
        return buffer[it->second];
    }
    
    // Se o buffer estiver cheio, precisa liberar espaço
    if (paginaToBuffer.size() >= BUFFER_SIZE) {
        // Implementação simples: substitui a página mais antiga
        std::string oldestKey = paginaToBuffer.begin()->first;
        auto [oldFilename, oldPageNum] = parseKey(oldestKey);
        writePage(oldFilename, oldPageNum);
        paginaToBuffer.erase(oldestKey);
    }
    
    // Verifica se o arquivo existe, se não, cria
    std::ifstream checkFile(filename);
    if (!checkFile) {
        std::ofstream createFile(filename);
        if (!createFile) {
            throw std::runtime_error("Não foi possível criar o arquivo: " + filename);
        }
        createFile.close();
    }
    
    // Carrega a página do disco
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + filename);
    }
    
    // Pula o cabeçalho
    std::string header;
    std::getline(file, header);
    
    // Pula as páginas anteriores
    for (int i = 0; i < pageNumber; i++) {
        for (int j = 0; j < MAX_TUPLAS_POR_PAGINA; j++) {
            std::string linha;
            std::getline(file, linha);
        }
    }
    
    // Lê a página atual
    buffer[currentPosition].limpar();
    for (int i = 0; i < MAX_TUPLAS_POR_PAGINA; i++) {
        std::string linha;
        if (std::getline(file, linha) && !linha.empty()) {
            buffer[currentPosition].adicionarTupla(Tupla(linha));
        }
    }
    file.close();
    
    // Atualiza o mapeamento
    paginaToBuffer[key] = currentPosition;
    int pos = currentPosition;
    currentPosition = (currentPosition + 1) % BUFFER_SIZE;
    
    return buffer[pos];
}

void BufferManager::writePage(const std::string& filename, int pageNumber) {
    std::string key = makeKey(filename, pageNumber);
    auto it = paginaToBuffer.find(key);
    if (it == paginaToBuffer.end()) {
        return; // Página não está no buffer
    }
    
    // Verifica se o arquivo existe, se não, cria
    std::ifstream checkFile(filename);
    if (!checkFile) {
        std::ofstream createFile(filename);
        if (!createFile) {
            throw std::runtime_error("Não foi possível criar o arquivo: " + filename);
        }
        createFile.close();
    }
    
    // Lê todo o arquivo
    std::vector<std::string> linhas;
    std::ifstream inFile(filename);
    std::string linha;
    while (std::getline(inFile, linha)) {
        linhas.push_back(linha);
    }
    inFile.close();
    
    // Escreve o arquivo de volta com a página atualizada
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("Não foi possível abrir o arquivo para escrita: " + filename);
    }
    
    // Escreve o cabeçalho
    if (!linhas.empty()) {
        outFile << linhas[0] << "\n";
    }
    
    // Escreve as páginas anteriores
    int linhasPorPagina = MAX_TUPLAS_POR_PAGINA;
    for (int i = 1; i < 1 + pageNumber * linhasPorPagina; i++) {
        if (i < linhas.size()) {
            outFile << linhas[i] << "\n";
        }
    }
    
    // Escreve a página atual
    for (const auto& t : buffer[it->second].tuplas) {
        outFile << t.toString() << "\n";
    }
    
    // Escreve as páginas posteriores
    for (int i = 1 + (pageNumber + 1) * linhasPorPagina; i < linhas.size(); i++) {
        outFile << linhas[i] << "\n";
    }
    
    outFile.close();
    
    // Remove a página do buffer
    paginaToBuffer.erase(key);
}

void BufferManager::markPageDirty(const std::string& filename, int pageNumber) {
    std::string key = makeKey(filename, pageNumber);
    auto it = paginaToBuffer.find(key);
    if (it != paginaToBuffer.end()) {
        buffer[it->second].marcarModificada();
    }
}

void BufferManager::clear() {
    // Escreve todas as páginas modificadas no disco
    for (const auto& pair : paginaToBuffer) {
        auto [filename, pageNumber] = parseKey(pair.first);
        if (buffer[pair.second].estaModificada()) {
            writePage(filename, pageNumber);
        }
    }
    
    // Limpa o buffer
    paginaToBuffer.clear();
    currentPosition = 0;
    for (auto& page : buffer) {
        page.limpar();
    }
} 