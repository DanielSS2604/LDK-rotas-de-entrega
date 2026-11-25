// Dijkstra.h
// Algoritmo de Dijkstra: menor caminho de UM nó de origem para TODOS os outros nós do grafo (pesos não-negativos).
// Agora é uma CLASSE: ao criar o objeto, o algoritmo já roda e guarda as respostas.
// Uso:  Dijkstra d(grafo, origem);   d.distanciaAte(v);   d.caminhoAte(v);

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include "Grafo.h"      // Precisamos da classe Grafo, que é o mapa (locais e ruas)

class Dijkstra {
private:
    // INF significa "infinito": se um nó não for alcançável, a distância dele será INF (não tem caminho para ele).
    // constexpr significa que o compilador já sabe o valor antes de rodar o programa
    // static = "uma só cópia, compartilhada por todos os objetos"
    static constexpr double INF = 1e18;

    const Grafo& grafo;          // mapa onde o algoritmo roda (só leitura, não é copiado)

    // dist[v] = a menor distância conhecida da origem até o nó v
    // ex: dist[5] = 320.0 -> o caminho mais curto até o nó 5 tem 320 m.
    std::vector<double> dist;

    // anterior[v] = nó que vem imediatamente ANTES de v no caminho mínimo.
    // Serve para reconstruir a rota.
    // -1 significa "sem anterior": é a própria origem ou um nó que não foi alcançado.
    std::vector<int> anterior;

    // O algoritmo em si (chamado pelo construtor)
    void executar(int origem);

public:
    // Cria o objeto e já executa o Dijkstra a partir de 'origem'
    Dijkstra(const Grafo& g, int origem);

    // Retorna true se o nó v foi alcançado (se dist[v] ainda não é "infinito").
    bool alcancavel(int v) const { return grafo.idValido(v) && dist[v] < INF; }

    // Menor distância (metros) origem -> v. Retorna -1 se v não for alcançável.
    double distanciaAte(int v) const { return alcancavel(v) ? dist[v] : -1; }

    // Monta a rota completa origem -> destino usando o vetor 'anterior'.
    // Retorna vetor vazio se o destino não for alcançável.
    std::vector<int> caminhoAte(int destino) const;
};

#endif
