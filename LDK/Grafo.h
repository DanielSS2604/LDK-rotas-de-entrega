
#ifndef GRAFO_H
#define GRAFO_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Localizacao.h"

// Uma aresta que SAI de um nó: para onde vai e quanto custa (metros)
struct Aresta {
    int destino;
    double peso;
};

class Grafo {
private:
    std::vector<Localizacao> locais;          // locais[i] é o nó de id i
    std::vector<std::vector<Aresta>> adj;     // adj[i] = arestas que saem do nó i
    std::unordered_map<std::string, int> indicePorNome; // busca rápida por nome

    // Quebra uma linha CSV em campos usando o separador ';'
    static std::vector<std::string> dividirLinha(const std::string& linha);

public:
    // Distância em metros entre duas coordenadas (fórmula de Haversine)
    static double haversine(double lat1, double lon1, double lat2, double lon2);

    // ---- Construção do grafo ----
    int adicionarLocal(const Localizacao& l);          // retorna o id
    bool adicionarAresta(int a, int b, double peso);   // bidirecional; false se inválida

    // ---- Leitura de arquivos CSV ----
    // locais.csv  : id;nome;tipo;lat;lon
    bool carregarLocais(const std::string& caminho);
    // arestas.csv : id_origem;id_destino;distancia_m  (se 0 ou vazio, usa Haversine)
    bool carregarArestas(const std::string& caminho);

    // ---- Consultas ----
    int totalLocais() const { return static_cast<int>(locais.size()); }
    bool idValido(int id) const { return id >= 0 && id < totalLocais(); }
    const Localizacao& getLocal(int id) const { return locais[id]; }
    const std::vector<Aresta>& vizinhos(int id) const { return adj[id]; }
    int buscarPorNome(const std::string& nome) const; // -1 se não achar
    // Peso (metros) da aresta a-b; retorna -1 se os nós não forem vizinhos
    double pesoAresta(int a, int b) const;

    // ---- Exibição ----
    void listarLocais() const;
    void imprimirConexoes() const;
};

#endif
