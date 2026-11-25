// Dijkstra.h
// Algoritmo de Dijkstra: menor caminho de UM nó de origem para TODOS os outros nós do grafo (pesos não-negativos).

#ifndef DIJKSTRA_H      
#define DIJKSTRA_H

#include <vector>       
#include "Grafo.h"      // Precisamos da classe Grafo, que é o mapa (locais e ruas)

struct ResultadoDijkstra {

    //IFN signidica "infinito", se um no não for alcançável, a distância dele será INF(não tem caminho para ele).
    // constexper significa q o compilador ja sabe o valor antes de rodar o programa
    //static = "uma so copia, compartilha por todos"
    static constexpr double INF = 1e18; 

    // dist[v] = é a menor distancia conhecida da origem até o nó v
    //ex: dist[2]= 2.7m, significa q a menor distancia da origem até o nó 2 é 2.7 metros
    std::vector<double> dist;

    // anterior[v] = nó que vem imediatamente ANTES de v no caminho mínimo.
    // Serve para reconstruir a rota.
    //caso o anterior[v]= -1, significa q é a prorpia origem, ou anterior[v] n tem caminho para a origem
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
