// Persistencia.h
// Salva e carrega o estado do programa em um arquivo .txt:
// cliente, fila de pedidos pendentes e histórico (pilha).
// Agora é uma CLASSE: o objeto guarda o caminho do arquivo.
//
// Formato (uma linha por registro):
//   CLIENTE;nome;idLocal
//   PEDIDO;id;cliente;prato;idRestaurante;status;distancia;custo;rota
//
//   status: E = entregue, C = cancelado, F = na fila
//   rota  : ids dos nós separados por '|'  (ex: 4|7|12)
//
// O histórico é gravado do mais antigo ao mais novo,
// para o mais novo voltar ao topo da pilha ao carregar.

#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <fstream>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#include "Cliente.h"
#include "Pedido.h"

class Persistencia {
private:
    std::string caminho;    // arquivo onde o estado é gravado/lido

    // Auxiliares (só a própria classe usa)
    static std::vector<std::string> dividir(const std::string& texto, char sep);
    static std::string limpar(std::string s);
    static void escreverPedido(std::ofstream& arq, const Pedido& p, char status);

public:
    explicit Persistencia(const std::string& caminho) : caminho(caminho) {}

    // Grava o estado no arquivo (sobrescreve).
    // Retorna false se não conseguir abrir o arquivo.
    bool salvar(const Cliente& cliente,
                const std::queue<Pedido>& fila,
                const std::stack<Pedido>& historico) const;

    // Lê o arquivo e preenche cliente, fila, histórico e próximo id de pedido.
    // Retorna false se o arquivo não existir (normal na 1ª execução).
    // Linhas com ids inválidos são ignoradas, com aviso.
    bool carregar(int totalLocais, int totalRestaurantes,
                  Cliente& cliente,
                  std::queue<Pedido>& fila,
                  std::stack<Pedido>& historico,
                  int& proximoIdPedido) const;
};

#endif
