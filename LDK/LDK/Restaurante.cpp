// ============================================================
// Restaurante.cpp
// ============================================================
#include "Restaurante.h"

#include <fstream>
#include <iostream>
#include <sstream>

// Divide 'texto' toda vez que encontrar 'sep'; remove '\r' do fim (arquivos Windows)
static std::vector<std::string> dividir(const std::string& texto, char sep) {
    std::vector<std::string> partes;
    std::stringstream ss(texto);
    std::string p;
    while (std::getline(ss, p, sep)) {
        if (!p.empty() && p.back() == '\r') p.pop_back();
        partes.push_back(p);
    }
    return partes;
}

std::vector<Restaurante> Restaurante::carregarDoCsv(const std::string& caminho) {
    std::vector<Restaurante> lista;
    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir " << caminho << "\n";
        return lista;
    }

    std::string linha;
    std::getline(arq, linha);                   // descarta o cabeçalho
    int numLinha = 1;
    while (std::getline(arq, linha)) {
        numLinha++;
        if (linha.empty() || linha == "\r") continue;
        auto c = dividir(linha, ';');
        if (c.size() < 5) {
            std::cerr << "Aviso: restaurante da linha " << numLinha << " ignorado\n";
            continue;
        }
        try {
            lista.emplace_back(std::stoi(c[0]), c[1], std::stoi(c[2]),
                               dividir(c[3], '|'), std::stod(c[4]));
        } catch (const std::exception&) {
            std::cerr << "Aviso: restaurante da linha " << numLinha << " ignorado (numero invalido)\n";
        }
    }
    return lista;
}
