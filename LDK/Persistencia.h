// Persistencia.h
// Salva e carrega o estado do programa em um arquivo .txt:
// cliente, fila de pedidos pendentes e histórico (pilha).
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

#include <queue>
#include <stack>
#include <string>

#include "Cliente.h"
#include "Pedido.h"

// Grava o estado no arquivo (sobrescreve).
// Retorna false se não conseguir abrir o arquivo.
bool salvarEstado(const std::string& caminho,
                  const Cliente& cliente,
                  const std::queue<Pedido>& fila,
                  const std::stack<Pedido>& historico);

// Lê o arquivo e preenche cliente, fila, histórico e próximo id de pedido.
// Retorna false se o arquivo não existir (normal na 1ª execução).
// Linhas com ids inválidos são ignoradas, com aviso.
bool carregarEstado(const std::string& caminho,
                    int totalLocais, int totalRestaurantes,
                    Cliente& cliente,
                    std::queue<Pedido>& fila,
                    std::stack<Pedido>& historico,
                    int& proximoIdPedido);

#endif
