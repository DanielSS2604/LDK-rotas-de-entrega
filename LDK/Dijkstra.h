// Dijkstra.h
// Algoritmo de Dijkstra: menor caminho de UM nó de origem para TODOS os outros nós do grafo (pesos não-negativos).

#ifndef DIJKSTRA_H      
#define DIJKSTRA_H

#include <vector>       
#include "Grafo.h"      // Precisamos da classe Grafo, que é o mapa (locais e ruas)

// é um "pacote de respostas". guardando tudo q descobrimos
struct ResultadoDijkstra {

    //INF signiica "infinito", se um no não for alcançável, a distância dele será INF(não tem caminho para ele).
    // constexpr significa q o compilador ja sabe o valor antes de rodar o programa
    //static = "uma so copia, compartilha por todos"
    static constexpr double INF = 1e18; 

    // dist[v] = é a menor distancia conhecida da origem até o nó v
    //ex: dist[2]= 320.0 -> o caminho mais curto até o nó 5 tem 320 m.
    std::vector<double> dist;

    // anterior[v] = nó que vem imediatamente ANTES de v no caminho mínimo.
    // Serve para reconstruir a rota.
    // -1 significa "sem anterior": é a própria origem ou um nó que não foi alcançado.
    std::vector<int> anterior;

    // Retorna true se o nó v foi alcançado (se dist[v] ainda não é "infinito").
    bool alcancavel(int v) const { return dist[v] < INF; }

    // Monta a rota completa origem -> destino usando o vetor 'anterior'.
    // Retorna vetor vazio se o destino não for alcançável.
    std::vector<int> caminhoAte(int destino) const;
};

// Executa o Dijkstra a partir de 'origem' e devolve as distâncias e os caminhos.
// (implementada no Dijkstra.cpp)
ResultadoDijkstra dijkstra(const Grafo& g, int origem);

#endif
