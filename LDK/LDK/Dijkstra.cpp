// Dijkstra.cpp

#include "Dijkstra.h"

#include <algorithm>        // std::reverse
#include <functional>       // std::greater (faz o heap sair o MENOR primeiro)
#include <queue>            // std::priority_queue
#include <utility>          // std::pair

// Construtor: prepara os vetores e já roda o algoritmo
Dijkstra::Dijkstra(const Grafo& g, int origem) : grafo(g) {
    const int n = g.totalLocais();          // quantidade de nós do grafo
    dist.assign(n, INF);                    // todas as distâncias começam "infinitas"
    anterior.assign(n, -1);                 // ninguém tem anterior ainda
    executar(origem);
}

// reconstrói origem -> destino
std::vector<int> Dijkstra::caminhoAte(int destino) const {
    std::vector<int> caminho;

    // Se o destino não existe ou não é alcançável, devolve vazio
    if (!alcancavel(destino))
        return caminho;

    // começa do destino e vai voltando até chegar em -1 (origem)
    for (int v = destino; v != -1; v = anterior[v])
        caminho.push_back(v);

    // Estava de trás para frente, então inverte: origem -> destino
    std::reverse(caminho.begin(), caminho.end());
    return caminho;
}

// Algoritmo de Dijkstra
void Dijkstra::executar(int origem) {
    if (!grafo.idValido(origem)) return;        // origem inválida: tudo continua "inalcançável"

    // Fila de prioridade (min-heap): guarda pares.
    using Par = std::pair<double, int>;         // (distância, nó)
    std::priority_queue<Par, std::vector<Par>, std::greater<Par>> fila; // fila onde sai primeiro o par de menor distância (greater inverte o padrão, que é sair o maior)

    dist[origem] = 0;                           // distância da origem até ela mesma é 0
    fila.push({0, origem});                     // começa a busca pela origem

    while (!fila.empty()) {
        auto [d, u] = fila.top();               // pega o nó mais perto ainda não processado
        fila.pop();

        // Entrada desatualizada: já achamos um caminho melhor para u depois de inseri-la
        if (d > dist[u]) continue;

        // RELAXAMENTO: tenta melhorar a distância de cada vizinho passando por u
        for (const Aresta& a : grafo.vizinhos(u)) {
            double nova = d + a.peso;           // distância até o vizinho passando por u
            if (nova < dist[a.destino]) {       // achou um caminho melhor? substitui a distância e o anterior
                dist[a.destino] = nova;
                anterior[a.destino] = u;
                fila.push({nova, a.destino});   // reavalia esse vizinho depois
            }
        }
    }
}
