// ============================================================
// Persistencia.h
// Salva e carrega o estado do programa em um arquivo .txt, para
// não perder os dados quando o programa fecha:
//   - o cliente (nome e local);
//   - a fila de pedidos pendentes;
//   - o histórico (pilha) de pedidos entregues/cancelados.
//
// Formato do arquivo (uma linha por registro, campos separados por ';'):
//   CLIENTE;nome;idLocal
//   PEDIDO;id;cliente;prato;idRestaurante;status;distancia;custo;rota
//
//   status: E = entregue, C = cancelado, F = ainda na fila
//   rota  : ids dos nós separados por '|'  (ex: 4|7|12)
//
// As linhas do histórico vêm do pedido MAIS ANTIGO para o MAIS NOVO,
// então ao ler e empilhar na ordem, o mais novo volta para o topo.
// ============================================================
#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <queue>
#include <stack>
#include <string>

#include "Cliente.h"
#include "Pedido.h"

// Grava tudo no arquivo (sobrescreve). Retorna false se não conseguir abrir.
bool salvarEstado(const std::string& caminho,
                  const Cliente& cliente,
                  const std::queue<Pedido>& fila,
                  const std::stack<Pedido>& historico);

// Lê o arquivo e preenche cliente, fila, histórico e o próximo id de pedido.
// Retorna false se o arquivo não existir (normal na primeira execução).
// Linhas com ids inválidos são ignoradas, com aviso.
bool carregarEstado(const std::string& caminho,
                    int totalLocais, int totalRestaurantes,
                    Cliente& cliente,
                    std::queue<Pedido>& fila,
                    std::stack<Pedido>& historico,
                    int& proximoIdPedido);

#endif
