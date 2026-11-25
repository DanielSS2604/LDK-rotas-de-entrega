
// Cliente.h

#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

class Cliente {
private:
    std::string nome;
    int idLocal;      // nó do grafo onde o cliente está (-1 = ainda não cadastrado)

public:
    Cliente() : nome("Cliente"), idLocal(-1) {}

    void cadastrar(const std::string& n, int local) { nome = n; idLocal = local; }
    bool temLocal() const { return idLocal >= 0; }
    int getIdLocal() const { return idLocal; }
    const std::string& getNome() const { return nome; }
};

#endif
