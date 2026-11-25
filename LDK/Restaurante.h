// ============================================================
// Restaurante.h
// Um restaurante (franquia) vinculado a um nó do grafo.
// ============================================================
#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include <string>
#include <vector>

class Restaurante {
private:
    int id;
    std::string nome;
    int idLocal;                    // id do nó do grafo onde ele fica
    std::vector<std::string> pratos;// ex: {"hamburguer", "sorvete"}
    double avaliacao;               // nota de 0 a 5

public:
    Restaurante(int id, const std::string& nome, int idLocal,
                const std::vector<std::string>& pratos, double avaliacao)
        : id(id), nome(nome), idLocal(idLocal), pratos(pratos), avaliacao(avaliacao) {}

    int getId() const { return id; }
    const std::string& getNome() const { return nome; }
    int getIdLocal() const { return idLocal; }
    const std::vector<std::string>& getPratos() const { return pratos; }
    double getAvaliacao() const { return avaliacao; }
};

// Lê restaurantes.csv (id;nome;id_local;pratos;avaliacao). Pratos separados por '|'.
// Retorna lista vazia se o arquivo não abrir.
std::vector<Restaurante> carregarRestaurantes(const std::string& caminho);

#endif
