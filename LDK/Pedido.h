
#ifndef PEDIDO_H
#define PEDIDO_H

#include <string>
#include <vector>

struct Pedido {
    int id = 0;
    std::string cliente;
    std::string prato;
    int idRestaurante = -1;
    std::vector<int> rota;      // ids dos nós: cliente -> ... -> restaurante
    double distancia = 0;       // metros
    double custo = 0;           // reais
    bool concluido = false;     // true = entregue; false = cancelado

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
};

#endif
