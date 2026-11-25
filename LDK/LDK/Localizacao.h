
// Localizacao.h

#ifndef LOCALIZACAO_H
#define LOCALIZACAO_H

#include <string>

class Localizacao {
private:
    int id;              // identificador único (também é o índice no grafo)
    std::string nome;    // ex: "Conjunto Nacional"
    std::string tipo;    // ex: "shopping", "quadra", "orgao_publico", "turistico"
    double lat;          // latitude  (graus)
    double lon;          // longitude (graus)

public:
    // Construtor padrão (necessário para usar em std::vector sem inicializar)
    Localizacao() : id(-1), nome(""), tipo(""), lat(0.0), lon(0.0) {}

    Localizacao(int id, const std::string& nome, const std::string& tipo,
                double lat, double lon)
        : id(id), nome(nome), tipo(tipo), lat(lat), lon(lon) {}

    // Getters (métodos const: não alteram o objeto)
    int getId() const { return id; }
    const std::string& getNome() const { return nome; }
    const std::string& getTipo() const { return tipo; }
    double getLat() const { return lat; }
    double getLon() const { return lon; }
};

#endif
