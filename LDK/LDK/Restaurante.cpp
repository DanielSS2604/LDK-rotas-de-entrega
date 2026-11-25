
#include "Restaurante.h"

#include <fstream>
#include <iostream>
#include <sstream>

// Quebra um texto em pedacos usando um separador.
// Ex: "hamburguer|sorvete" com '|' -> ["hamburguer", "sorvete"]
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

// Formato de cada linha: id;nome;id_local;pratos;avaliacao
std::vector<Restaurante> carregarRestaurantes(const std::string& caminho) {
    std::vector<Restaurante> lista;
    std::ifstream arq(caminho);
    if (!arq.is_open()) {                       // arquivo nao abriu: devolve lista vazia
        std::cerr << "Erro: nao foi possivel abrir " << caminho << "\n";
        return lista;
    }

    std::string linha;
    std::getline(arq, linha);                   // descarta o cabeçalho
    int numLinha = 1;
    while (std::getline(arq, linha)) {          // le linha por linha
        numLinha++;
        if (linha.empty() || linha == "\r") continue;
        auto c = dividir(linha, ';');           // separa os campos
        if (c.size() < 5) {                     // linha incompleta: pula
            std::cerr << "Aviso: restaurante da linha " << numLinha << " ignorado\n";
            continue;
        }
        // try/catch: se tiver letra onde deveria ser numero, ignora so essa linha
        try {
            lista.emplace_back(std::stoi(c[0]), c[1], std::stoi(c[2]),
                               dividir(c[3], '|'), std::stod(c[4]));
        } catch (const std::exception&) {
            std::cerr << "Aviso: restaurante da linha " << numLinha << " ignorado (numero invalido)\n";
        }
    }
    return lista;
}
