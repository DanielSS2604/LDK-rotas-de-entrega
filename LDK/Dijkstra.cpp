
// Dijkstra.cpp

#include "Dijkstra.h"

#include <algorithm>
#include <functional>
#include <queue>
#include <utility>

std::vector<int> ResultadoDijkstra::caminhoAte(int destino) const {
    std::vector<int> caminho;
    if (destino < 0 || destino >= static_cast<int>(dist.size()) || !alcancavel(destino))
        return caminho;
    for (int v = destino; v != -1; v = anterior[v])
        caminho.push_back(v);                       // monta do destino para a origem...
    std::reverse(caminho.begin(), caminho.end());   // ...e inverte para origem -> destino
    return caminho;
}

ResultadoDijkstra dijkstra(const Grafo& g, int origem) {
    const int n = g.totalLocais();
    ResultadoDijkstra r;
    r.dist.assign(n, ResultadoDijkstra::INF);
    r.anterior.assign(n, -1);
    if (!g.idValido(origem)) return r;

    // Fila de prioridade (min-heap): sempre sai o nó com MENOR distância conhecida.
    using Par = std::pair<double, int>;             // (distância, nó)
    std::priority_queue<Par, std::vector<Par>, std::greater<Par>> fila;

    r.dist[origem] = 0;
    fila.push({0, origem});

    while (!fila.empty()) {
        auto [d, u] = fila.top();
        fila.pop();

        // Entrada velha: já achamos um caminho melhor para u depois de inseri-la
        if (d > r.dist[u]) continue;

        // Relaxamento: tenta melhorar a distância de cada vizinho passando por u
        for (const Aresta& a : g.vizinhos(u)) {
            double nova = d + a.peso;
            if (nova < r.dist[a.destino]) {
                r.dist[a.destino] = nova;
                r.anterior[a.destino] = u;
                fila.push({nova, a.destino});
            }
        }
    }
    return r;
}
