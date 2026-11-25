
// Dijkstra.h
// Algoritmo de Dijkstra: menor caminho de UM nó de origem para
// TODOS os outros nós do grafo (pesos nao-negativos).

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include "Grafo.h"

struct ResultadoDijkstra {
    static constexpr double INF = 1e18;   // "infinito": nó ainda não alcançado

    std::vector<double> dist;    // dist[v]     = menor distância origem -> v (metros)
    std::vector<int> anterior;   // anterior[v] = nó que vem antes de v no caminho mínimo

    bool alcancavel(int v) const { return dist[v] < INF; }

    // Reconstrói o caminho origem -> destino seguindo 'anterior' de trás para frente.
    // Retorna vetor vazio se o destino não for alcançável.
    std::vector<int> caminhoAte(int destino) const;
};

ResultadoDijkstra dijkstra(const Grafo& g, int origem);

#endif
