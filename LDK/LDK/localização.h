// Localizacao.h

#ifndef LOCALIZACAO_H
#define LOCALIZACAO_H

#include <string>

class Localizacao {
private:
    int id;              // identificador único (também é o índice no grafo)
    std::string nome;    // ex: "Conjunto Nacional"
    std::string tipo;    // ex: "shopping", "quadra", "orgao_publico", "turistico"

public:
    // Construtor padrão (necessário para usar em std::vector sem inicializar)
    Localizacao() : id(-1), nome(""), tipo("") {}

    Localizacao(int id, const std::string& nome, const std::string& tipo)
        : id(id), nome(nome), tipo(tipo) {}

    // Getters (métodos const: não alteram o objeto)
    int getId() const { return id; }
    const std::string& getNome() const { return nome; }
    const std::string& getTipo() const { return tipo; }
};

#endif
