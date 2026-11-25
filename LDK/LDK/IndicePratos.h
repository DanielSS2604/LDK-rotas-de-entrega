
#ifndef INDICE_PRATOS_H
#define INDICE_PRATOS_H

#include <list>
#include <string>
#include <vector>

class IndicePratos {
private:
    struct Prato {
        std::string nome;               // ex: "hamburguer"
        std::list<int> restaurantes;    // ids dos restaurantes que servem o prato
    };

    std::list<Prato> pratos;            // lista de pratos, ordenada por nome

public:
    // Associa o restaurante ao prato (cria o prato se ainda não existir)
    void inserir(const std::string& nome, int idRestaurante) {
        // Anda na lista até achar a posição do prato (ou o ponto onde ele deve entrar)
        auto it = pratos.begin();
        while (it != pratos.end() && it->nome < nome) ++it;

        // Parou em um prato com o mesmo nome: só acrescenta o restaurante
        if (it != pratos.end() && it->nome == nome) {
            it->restaurantes.push_back(idRestaurante);
            return;
        }

        // Prato novo: insere ANTES de 'it', mantendo a ordem alfabética
        Prato novo;
        novo.nome = nome;
        novo.restaurantes.push_back(idRestaurante);
        pratos.insert(it, novo);
    }

    // Retorna a lista de restaurantes do prato, ou nullptr se o prato não existe
    const std::list<int>* buscar(const std::string& nome) const {
        for (const Prato& p : pratos)
            if (p.nome == nome) return &p.restaurantes;
        return nullptr;
    }

    // Nomes de todos os pratos (já em ordem alfabética, não precisa de sort)
    std::vector<std::string> chaves() const {
        std::vector<std::string> nomes;
        for (const Prato& p : pratos) nomes.push_back(p.nome);
        return nomes;
    }

    size_t tamanho() const { return pratos.size(); }
};

#endif
