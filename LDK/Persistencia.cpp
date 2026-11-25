// Persistencia.cpp
#include "Persistencia.h"

#include <algorithm>    // std::reverse
#include <fstream>      // ofstream / ifstream (escrever e ler arquivos)
#include <iomanip>      // setprecision (casas decimais)
#include <iostream>     // cerr (avisos de erro)
#include <sstream>      // stringstream (usado em dividir)
#include <vector>

//  Auxiliares
// "static": a função só existe dentro deste .cpp.

// Divide 'texto' a cada 'sep'. Tira o '\r' do fim (arquivos do Windows).
// Ex.: dividir("a;b;c", ';') -> {"a", "b", "c"}
static std::vector<std::string> dividir(const std::string& texto, char sep) {
    std::vector<std::string> partes;
    std::stringstream ss(texto);            // trata a string como se fosse um arquivo
    std::string p;
    while (std::getline(ss, p, sep)) {      // lê até o próximo 'sep'
        if (!p.empty() && p.back() == '\r') p.pop_back();
        partes.push_back(p);
    }
    return partes;
}

// Troca ';' e '|' por espaço, para o nome digitado não quebrar o formato.
static std::string limpar(std::string s) {  // recebe cópia: não altera o original
    for (char& c : s)
        if (c == ';' || c == '|') c = ' ';
    return s;
}

// Escreve UMA linha de pedido no arquivo (status: E, C ou F).
static void escreverPedido(std::ofstream& arq, const Pedido& p, char status) {
    arq << "PEDIDO;" << p.id << ';' << limpar(p.cliente) << ';' << limpar(p.prato) << ';'
        << p.idRestaurante << ';' << status << ';'
        << p.distancia << ';' << p.custo << ';';
    for (size_t i = 0; i < p.rota.size(); i++) {
        if (i > 0) arq << '|';              // '|' só ENTRE os ids
        arq << p.rota[i];
    }
    arq << '\n';
}

// Salvar
bool salvarEstado(const std::string& caminho,
                  const Cliente& cliente,
                  const std::queue<Pedido>& fila,
                  const std::stack<Pedido>& historico) {
    std::ofstream arq(caminho);             // abre e apaga o conteúdo antigo
    if (!arq.is_open()) {
        std::cerr << "Aviso: nao foi possivel salvar em " << caminho << "\n";
        return false;
    }
    arq << std::fixed << std::setprecision(2);   // números com 2 casas decimais

    // 1) Cliente (só se já tiver local)
    if (cliente.temLocal())
        arq << "CLIENTE;" << limpar(cliente.getNome()) << ';' << cliente.getIdLocal() << '\n';

    // 2) Histórico (pilha): desempilhar sai do mais NOVO ao mais ANTIGO.
    //    Copia, guarda num vetor e inverte para gravar do mais antigo ao mais novo.
    std::vector<Pedido> pedidos;
    std::stack<Pedido> copiaPilha = historico;   // cópia: não esvazia a original
    while (!copiaPilha.empty()) {
        pedidos.push_back(copiaPilha.top());
        copiaPilha.pop();
    }
    std::reverse(pedidos.begin(), pedidos.end());
    for (const Pedido& p : pedidos)
        escreverPedido(arq, p, p.concluido ? 'E' : 'C');

    // 3) Fila: já sai do primeiro ao último (ordem certa)
    std::queue<Pedido> copiaFila = fila;
    while (!copiaFila.empty()) {
        escreverPedido(arq, copiaFila.front(), 'F');
        copiaFila.pop();
    }
    return true;
}

//Carregar
bool carregarEstado(const std::string& caminho,
                    int totalLocais, int totalRestaurantes,
                    Cliente& cliente,
                    std::queue<Pedido>& fila,
                    std::stack<Pedido>& historico,
                    int& proximoIdPedido) {
    std::ifstream arq(caminho);
    if (!arq.is_open()) return false;       // 1ª execução: ainda não há arquivo

    std::string linha;
    int numLinha = 0;                       // só para avisar qual linha deu erro
    int maiorId = 0;                        // maior id de pedido lido

    while (std::getline(arq, linha)) {      // uma linha por vez
        numLinha++;
        if (linha.empty() || linha == "\r") continue;   // pula linha em branco

        auto c = dividir(linha, ';');       // c[0] = tipo do registro
        try {
            // Linha do cliente 
            if (c[0] == "CLIENTE" && c.size() >= 3) {
                int local = std::stoi(c[2]);
                if (local >= 0 && local < totalLocais) cliente.cadastrar(c[1], local);

            //Linha de pedido 
            } else if (c[0] == "PEDIDO" && c.size() >= 8) {
                Pedido p;
                p.id = std::stoi(c[1]);             // stoi: texto -> int
                p.cliente = c[2];
                p.prato = c[3];
                p.idRestaurante = std::stoi(c[4]);
                char status = c[5].empty() ? '?' : c[5][0];
                p.distancia = std::stod(c[6]);      // stod: texto -> double
                p.custo = std::stod(c[7]);
                if (c.size() > 8)                   // rota: ids separados por '|'
                    for (const std::string& no : dividir(c[8], '|'))
                        p.rota.push_back(std::stoi(no));

                // Valida: os ids ainda existem nos CSVs atuais?
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
                    historico.push(p);              // o último lido fica no topo
                }
                maiorId = std::max(maiorId, p.id);
            }
        } catch (const std::exception&) {           // stoi/stod lançam exceção se não for número
            std::cerr << "Aviso: linha " << numLinha << " do arquivo salvo ignorada (numero invalido)\n";
        }
    }

    proximoIdPedido = maiorId + 1;          // continua a numeração de onde parou
    return true;
}
