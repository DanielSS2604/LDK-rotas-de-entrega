// ============================================================
// Persistencia.cpp
// ============================================================
#include "Persistencia.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

// ---------- Auxiliares ----------

// Divide 'texto' em partes a cada 'sep'; tira o '\r' do fim (arquivos do Windows)
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

// Troca ';' e '|' por espaço, para um nome digitado não quebrar o formato do arquivo
static std::string limpar(std::string s) {
    for (char& c : s)
        if (c == ';' || c == '|') c = ' ';
    return s;
}

// Escreve UMA linha de pedido no arquivo
static void escreverPedido(std::ofstream& arq, const Pedido& p, char status) {
    arq << "PEDIDO;" << p.id << ';' << limpar(p.cliente) << ';' << limpar(p.prato) << ';'
        << p.idRestaurante << ';' << status << ';'
        << p.distancia << ';' << p.custo << ';';
    for (size_t i = 0; i < p.rota.size(); i++) {
        if (i > 0) arq << '|';
        arq << p.rota[i];
    }
    arq << '\n';
}

// ---------- Salvar ----------

bool salvarEstado(const std::string& caminho,
                  const Cliente& cliente,
                  const std::queue<Pedido>& fila,
                  const std::stack<Pedido>& historico) {
    std::ofstream arq(caminho);                     // abre e apaga o conteúdo antigo
    if (!arq.is_open()) {
        std::cerr << "Aviso: nao foi possivel salvar em " << caminho << "\n";
        return false;
    }
    arq << std::fixed << std::setprecision(2);

    if (cliente.temLocal())
        arq << "CLIENTE;" << limpar(cliente.getNome()) << ';' << cliente.getIdLocal() << '\n';

    // Pilha: desempilhando sai do mais NOVO para o mais ANTIGO.
    // Guardamos num vetor e invertemos, para gravar do mais antigo ao mais novo.
    std::vector<Pedido> pedidos;
    std::stack<Pedido> copiaPilha = historico;      // cópia, para não esvaziar a original
    while (!copiaPilha.empty()) {
        pedidos.push_back(copiaPilha.top());
        copiaPilha.pop();
    }
    std::reverse(pedidos.begin(), pedidos.end());
    for (const Pedido& p : pedidos)
        escreverPedido(arq, p, p.concluido ? 'E' : 'C');

    // Fila: desenfileirando já sai do primeiro para o último (ordem certa)
    std::queue<Pedido> copiaFila = fila;
    while (!copiaFila.empty()) {
        escreverPedido(arq, copiaFila.front(), 'F');
        copiaFila.pop();
    }
    return true;
}

// ---------- Carregar ----------

bool carregarEstado(const std::string& caminho,
                    int totalLocais, int totalRestaurantes,
                    Cliente& cliente,
                    std::queue<Pedido>& fila,
                    std::stack<Pedido>& historico,
                    int& proximoIdPedido) {
    std::ifstream arq(caminho);
    if (!arq.is_open()) return false;               // primeira execução: ainda não há arquivo

    std::string linha;
    int numLinha = 0;
    int maiorId = 0;

    while (std::getline(arq, linha)) {
        numLinha++;
        if (linha.empty() || linha == "\r") continue;

        auto c = dividir(linha, ';');
        try {
            if (c[0] == "CLIENTE" && c.size() >= 3) {
                int local = std::stoi(c[2]);
                if (local >= 0 && local < totalLocais) cliente.cadastrar(c[1], local);

            } else if (c[0] == "PEDIDO" && c.size() >= 8) {
                Pedido p;
                p.id = std::stoi(c[1]);
                p.cliente = c[2];
                p.prato = c[3];
                p.idRestaurante = std::stoi(c[4]);
                char status = c[5].empty() ? '?' : c[5][0];
                p.distancia = std::stod(c[6]);
                p.custo = std::stod(c[7]);
                if (c.size() > 8)
                    for (const std::string& no : dividir(c[8], '|'))
                        p.rota.push_back(std::stoi(no));

                // Confere se os ids ainda fazem sentido com os CSVs atuais
                bool valido = p.idRestaurante >= 0 && p.idRestaurante < totalRestaurantes;
                for (int no : p.rota)
                    if (no < 0 || no >= totalLocais) valido = false;
                if (!valido || (status != 'E' && status != 'C' && status != 'F')) {
                    std::cerr << "Aviso: pedido da linha " << numLinha << " ignorado (dados invalidos)\n";
                    continue;
                }

                if (status == 'F') {
                    fila.push(p);                   // pendente: volta para a fila
                } else {
                    p.concluido = (status == 'E');
                    historico.push(p);              // empilha: o último lido fica no topo
                }
                maiorId = std::max(maiorId, p.id);
            }
        } catch (const std::exception&) {
            std::cerr << "Aviso: linha " << numLinha << " do arquivo salvo ignorada (numero invalido)\n";
        }
    }

    proximoIdPedido = maiorId + 1;                  // continua a numeração de onde parou
    return true;
}
