// main.cpp
// O main só cria o objeto do sistema e manda ele trabalhar.
// Toda a lógica está dentro da classe SistemaEntregas.

#include "SistemaEntregas.h"

int main() {
    SistemaEntregas sistema;
    if (!sistema.iniciar()) return 1;   // falha ao carregar mapa/restaurantes
    sistema.executar();                 // loop do menu
    return 0;
}
