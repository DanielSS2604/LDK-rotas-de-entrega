
#include "Grafo.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// ---------- Auxiliares ----------

std::vector<std::string> Grafo::dividirLinha(const std::string& linha) {
    std::vector<std::string> campos;
    std::stringstream ss(linha);
    std::string campo;
    while (std::getline(ss, campo, ';')) {
        // Remove '\r' que aparece em arquivos criados no Windows (CRLF)
        if (!campo.empty() && campo.back() == '\r') campo.pop_back();
        campos.push_back(campo);
    }
    return campos;
}

// ---------- Construção ----------

int Grafo::adicionarLocal(const Localizacao& l) {
    // O id do nó é a posição dele no vetor, então ignoramos o id do arquivo
    // e reconstruímos o objeto com o id correto.
    int novoId = totalLocais();
    locais.emplace_back(novoId, l.getNome(), l.getTipo(), l.getLat(), l.getLon());
    adj.emplace_back();                          // lista de vizinhos vazia
    indicePorNome[l.getNome()] = novoId;
    return novoId;
}

bool Grafo::adicionarAresta(int a, int b, double peso) {
    if (!idValido(a) || !idValido(b) || a == b || peso < 0) return false;
    adj[a].push_back({b, peso});                 // a -> b
    adj[b].push_back({a, peso});                 // b -> a (ruas de mão dupla)
    return true;
}

// ---------- Arquivos ----------

bool Grafo::carregarLocais(const std::string& caminho) {
    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir " << caminho << "\n";
        return false;
    }

    std::string linha;
    std::getline(arq, linha);                    // descarta o cabeçalho
    int numLinha = 1;

    while (std::getline(arq, linha)) {
        numLinha++;
        if (linha.empty() || linha == "\r") continue;

        auto c = dividirLinha(linha);
        if (c.size() < 5) {
            std::cerr << "Aviso: linha " << numLinha << " ignorada (campos faltando)\n";
            continue;
        }
        try {
            // c[0] = id do arquivo (não usado), c[1] = nome, c[2] = tipo
            adicionarLocal(Localizacao(0, c[1], c[2], std::stod(c[3]), std::stod(c[4])));
        } catch (const std::exception&) {
            std::cerr << "Aviso: linha " << numLinha << " ignorada (numero invalido)\n";
        }
    }
    return true;
}

bool Grafo::carregarArestas(const std::string& caminho) {
    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir " << caminho << "\n";
        return false;
    }

    std::string linha;
    std::getline(arq, linha);                    // cabeçalho
    int numLinha = 1;

    while (std::getline(arq, linha)) {
        numLinha++;
        if (linha.empty() || linha == "\r") continue;

        auto c = dividirLinha(linha);
        if (c.size() < 3 || c[2].empty()) {
            std::cerr << "Aviso: linha " << numLinha << " ignorada (sem distancia)\n";
            continue;
        }
        try {
            int a = std::stoi(c[0]);
            int b = std::stoi(c[1]);
            double peso = std::stod(c[2]);

            if (peso <= 0 || !adicionarAresta(a, b, peso)) {
                std::cerr << "Aviso: linha " << numLinha << " ignorada (aresta invalida)\n";
            }
        } catch (const std::exception&) {
            std::cerr << "Aviso: linha " << numLinha << " ignorada (numero invalido)\n";
        }
    }
    return true;
}

// ---------- Consultas ----------

int Grafo::buscarPorNome(const std::string& nome) const {
    auto it = indicePorNome.find(nome);
    return (it == indicePorNome.end()) ? -1 : it->second;
}

double Grafo::pesoAresta(int a, int b) const {
    if (!idValido(a) || !idValido(b)) return -1;
    for (const auto& ar : adj[a])
        if (ar.destino == b) return ar.peso;
    return -1;
}

// ---------- Exibição ----------

void Grafo::listarLocais() const {
    std::cout << "\n--- Locais cadastrados (" << totalLocais() << ") ---\n";
    for (const auto& l : locais) {
        std::cout << std::setw(3) << l.getId() << " | "
                  << l.getNome() << " [" << l.getTipo() << "]\n";
    }
}

void Grafo::imprimirConexoes() const {
    std::cout << "\n--- Mapa de conexoes ---\n";
    std::cout << std::fixed << std::setprecision(0);
    for (int i = 0; i < totalLocais(); i++) {
        std::cout << locais[i].getNome() << " (" << i << ")\n";
        for (const auto& a : adj[i]) {
            std::cout << "    -> " << locais[a.destino].getNome()
                      << " (" << a.destino << ") : " << a.peso << " m\n";
        }
    }
}
