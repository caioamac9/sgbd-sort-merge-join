#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "Pagina.h"
#include <string>
#include <vector>
#include <unordered_map>

class BufferManager {
private:
    static const int BUFFER_SIZE = 4;
    std::vector<Pagina> buffer;
    std::unordered_map<std::string, int> paginaToBuffer; // Mapeia nome do arquivo + número da página para posição no buffer
    int currentPosition;
    
    std::string makeKey(const std::string& filename, int pageNumber);
    std::pair<std::string, int> parseKey(const std::string& key);

public:
    BufferManager();
    
    // Obtém uma página do buffer, carregando do disco se necessário
    Pagina& getPage(const std::string& filename, int pageNumber);
    
    // Escreve uma página no disco e libera o espaço no buffer
    void writePage(const std::string& filename, int pageNumber);
    
    // Marca uma página como modificada
    void markPageDirty(const std::string& filename, int pageNumber);
    
    // Limpa o buffer
    void clear();
    
    // Retorna o número de páginas no buffer
    int getBufferSize() const { return BUFFER_SIZE; }
};

#endif 