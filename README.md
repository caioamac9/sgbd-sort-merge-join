# Sort-Merge Join Implementation

Implementação de um algoritmo de junção Sort-Merge (Sort-Merge Join) para um sistema de gerenciamento de banco de dados.

## Características

- Implementação do algoritmo Sort-Merge Join entre duas tabelas
- Ordenação externa (external merge sort)
- Armazenamento de resultados em arquivos de texto
- Limite de 4 páginas em memória durante a execução
- Estrutura de dados otimizada para operações de I/O

## Estrutura do Projeto

```
.
├── main.cpp              # Ponto de entrada do programa
├── Tabela.h/cpp         # Implementação da classe Tabela
├── Pagina.h/cpp         # Implementação da classe Pagina
├── Tupla.h/cpp          # Implementação da classe Tupla
├── Operador.h/cpp       # Implementação do algoritmo de join
├── ExternalSorter.h/cpp # Implementação da ordenação externa
└── BufferManager.h/cpp  # Gerenciamento de buffer
```

## Requisitos

- Compilador C++17 ou superior
- Make (opcional, para build automatizado)

## Compilação

```bash
g++ -std=c++17 *.cpp -o sort_merge_join
```

## Execução

```bash
./sort_merge_join
```

## Estrutura de Dados

- **Tabela**: Lista de páginas, número de páginas, número de colunas
- **Pagina**: Vetor de 10 tuplas, número de tuplas ocupadas
- **Tupla**: Vetor de strings representando colunas

## Limitações

- Máximo de 4 páginas em memória
- 10 tuplas por página
- Arquivos de entrada/saída em formato texto

## Autor
Caioamac terror dos esmeralda
Luis Antonio ak cruzada
## Licença

Este projeto está sob a licença MIT. 
