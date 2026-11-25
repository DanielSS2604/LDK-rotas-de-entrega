// SistemaEntregas.h
// Classe principal: guarda o estado do sistema (antiga struct Sistema)
// e tem como MÉTODOS todas as opções do menu (antigas funções do main).

#ifndef SISTEMA_ENTREGAS_H
#define SISTEMA_ENTREGAS_H

#include <queue>
#include <stack>
#include <string>
#include <vector>

#include "Cliente.h"
#include "Dijkstra.h"
#include "Grafo.h"
#include "IndicePratos.h"
#include "Pedido.h"
#include "Persistencia.h"
#include "Restaurante.h"

class SistemaEntregas {
private:
    // Restaurante candidato a atender o pedido: id + distância até o cliente
    struct Candidato { int idRest; double dist; };

    //  Estado do sistema
    Grafo grafo;
    std::vector<Restaurante> restaurantes;
    IndicePratos indicePratos;              // lista de pratos; cada prato tem sua lista de restaurantes
    Cliente cliente;
    std::string pratoEscolhido;             // definido na opção 2
    std::queue<Pedido> filaPedidos;         // pedidos aguardando conclusão
    std::stack<Pedido> historico;           // pedidos finalizados (último em cima)
    int proximoIdPedido;
    Persistencia persistencia;              // salva/carrega o estado em arquivo

    //  Entrada de dados segura
    static int lerInteiro(const std::string& mensagem);
    static std::string lerLinha(const std::string& mensagem);
    // Versão minúscula de um texto (para busca sem diferenciar maiúsculas)
    static std::string minusculo(std::string s);

    //  Funções auxiliares de exibição
    void imprimirRota(const std::vector<int>& rota) const;
    void imprimirResumo(const Pedido& p) const;
    void exibirMenu() const;

    // Ordena os candidatos com INSERTION SORT (criterio 2: nota; senão: distância)
    void ordenarCandidatos(std::vector<Candidato>& cand, int criterio) const;

    // Grava cliente, fila e histórico no arquivo (chamada depois de cada mudança)
    void salvar() const { persistencia.salvar(cliente, filaPedidos, historico); }

    //  Opções do menu
    void cadastrarCliente();        // 1
    void fazerPedido();             // 2
    void buscarRestaurante();       // 3
    void visualizarMapa() const;    // 4
    void calcularCusto();           // 5
    void exibirHistorico() const;   // 6

public:
    SistemaEntregas() : proximoIdPedido(1), persistencia("dados/estado.txt") {}

    // Carrega o mapa, os restaurantes e o estado salvo. Retorna false se o mapa falhar.
    bool iniciar();

    // Loop do menu principal
    void executar();
};

#endif
