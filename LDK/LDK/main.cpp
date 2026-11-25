
// O main só cria o objeto do sistema e manda ele trabalhar.
#include "SistemaEntregas.h"

int main() {
    SistemaEntregas sistema;
    if (!sistema.iniciar()) return 1;   // falha ao carregar mapa/restaurantes
    sistema.executar();                 // loop do menu
    return 0;
}
