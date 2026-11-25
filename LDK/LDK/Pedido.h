// Pedido.h
// Um pedido: o que foi pedido, de qual restaurante, e a rota.
// Agora é uma CLASSE: os atributos são privados e só mudam por métodos.

#ifndef PEDIDO_H
#define PEDIDO_H

#include <string>
#include <vector>

class Pedido {
private:
    int id;
    std::string cliente;
    std::string prato;
    int idRestaurante;
    std::vector<int> rota;      // ids dos nós: cliente -> ... -> restaurante
    double distancia;           // metros
    double custo;               // reais
    bool concluido;             // true = entregue; false = cancelado

public:
    // Regra de preço (simulada): taxa fixa + valor por km rodado
    static constexpr double TAXA_BASE = 3.50;
    static constexpr double POR_KM = 1.80;
    static constexpr double VELOCIDADE_KMH = 30.0;   // para estimar o tempo

    static double calcularCusto(double metros) {
        return TAXA_BASE + POR_KM * (metros / 1000.0);
    }
    static double estimarMinutos(double metros) {
        return (metros / 1000.0) / VELOCIDADE_KMH * 60.0;
    }

    // Construtor padrão (necessário para usar em std::queue / std::stack sem inicializar)
    Pedido() : id(0), idRestaurante(-1), distancia(0), custo(0), concluido(false) {}

    // Construtor de um pedido NOVO: o custo é calculado aqui dentro,
    // ninguém de fora precisa lembrar de calcular.
    Pedido(int id, const std::string& cliente, const std::string& prato,
           int idRestaurante, const std::vector<int>& rota, double distancia)
        : id(id), cliente(cliente), prato(prato), idRestaurante(idRestaurante),
          rota(rota), distancia(distancia), custo(calcularCusto(distancia)),
          concluido(false) {}

    // Construtor para RECONSTRUIR um pedido lido do arquivo
    // (custo e status já vêm prontos do arquivo salvo)
    Pedido(int id, const std::string& cliente, const std::string& prato,
           int idRestaurante, const std::vector<int>& rota,
           double distancia, double custo, bool concluido)
        : id(id), cliente(cliente), prato(prato), idRestaurante(idRestaurante),
          rota(rota), distancia(distancia), custo(custo), concluido(concluido) {}

    // Getters (métodos const: não alteram o objeto)
    int getId() const { return id; }
    const std::string& getCliente() const { return cliente; }
    const std::string& getPrato() const { return prato; }
    int getIdRestaurante() const { return idRestaurante; }
    const std::vector<int>& getRota() const { return rota; }
    double getDistancia() const { return distancia; }
    double getCusto() const { return custo; }
    bool foiConcluido() const { return concluido; }

    // Único jeito de mudar o status: true = entregue, false = cancelado
    void finalizar(bool entregue) { concluido = entregue; }
};

#endif
