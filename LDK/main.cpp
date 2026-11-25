
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
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

using namespace std;

//  Entrada de dados segura 

int lerInteiro(const string& mensagem) {
    int valor;
    while (true) {
        cout << mensagem;
        if (cin >> valor) return valor;
        if (cin.eof()) return 0;                                   // fim da entrada
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida. Digite um numero.\n";
    }
}

string lerLinha(const string& mensagem) {
    cout << mensagem;
    string s;
    getline(cin >> ws, s);   // ws: ignora o '\n' que sobrou do cin >>
    return s;
}

// Versão minúscula de um texto (para busca sem diferenciar maiúsculas)
string minusculo(string s) {
    for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

//  Estado do sistema 

struct Sistema {
    Grafo grafo;
    vector<Restaurante> restaurantes;
    IndicePratos indicePratos;          // lista de pratos; cada prato tem sua lista de restaurantes
    Cliente cliente;
    string pratoEscolhido;              // definido na opção 2
    queue<Pedido> filaPedidos;          // pedidos aguardando conclusão
    stack<Pedido> historico;            // pedidos finalizados (último em cima)
    int proximoIdPedido = 1;
};

const string ARQUIVO_ESTADO = "dados/estado.txt";

// Grava cliente, fila e historico no arquivo (chamada depois de cada mudanca)
void salvar(const Sistema& s) {
    salvarEstado(ARQUIVO_ESTADO, s.cliente, s.filaPedidos, s.historico);
}

//  Funções auxiliares de exibição 

void imprimirRota(const Grafo& g, const vector<int>& rota) {
    for (size_t i = 0; i < rota.size(); i++) {
        cout << "  " << setw(2) << i + 1 << ". " << g.getLocal(rota[i]).getNome();
        if (i == 0) cout << "  (origem)";
        else if (i + 1 == rota.size()) cout << "  (restaurante)";
        if (i > 0) {
            cout << "  [+" << fixed << setprecision(0)
                 << g.pesoAresta(rota[i - 1], rota[i]) << " m]";
        }
        cout << "\n";
    }
}

void imprimirResumo(const Pedido& p, const Sistema& s) {
    cout << fixed << setprecision(2);
    cout << "  Pedido #" << p.id << " | " << p.prato << " | "
         << s.restaurantes[p.idRestaurante].getNome() << "\n"
         << "  Distancia: " << p.distancia / 1000.0 << " km | Custo: R$ "
         << p.custo << " | Tempo: ~"
         << setprecision(0) << Pedido::estimarMinutos(p.distancia) << " min\n";
}

//  Opções do menu 

//  Cadastrar localização do cliente
void cadastrarCliente(Sistema& s) {
    string nome = lerLinha("Seu nome: ");
    while (true) {
        string busca = lerLinha("Onde voce esta? Digite parte do nome (ex: SQN 108, Conjunto, Torre): ");
        string b = minusculo(busca);

        // Procura locais cujo nome contém o texto digitado (ex: "108" acha SQN 108 e SQS 108)
        vector<int> achados;
        for (int i = 0; i < s.grafo.totalLocais(); i++)
            if (minusculo(s.grafo.getLocal(i).getNome()).find(b) != string::npos)
                achados.push_back(i);

        if (achados.empty()) { cout << "Nenhum local encontrado.\n"; continue; }
        if (achados.size() > 15) { cout << achados.size() << " resultados; seja mais especifico.\n"; continue; }

        for (int id : achados)
            cout << "  " << setw(3) << id << " | " << s.grafo.getLocal(id).getNome()
                 << " [" << s.grafo.getLocal(id).getTipo() << "]\n";
        int escolha = lerInteiro("Digite o id do seu local (-1 para buscar de novo): ");
        if (escolha >= 0 && s.grafo.idValido(escolha)) {
            s.cliente.cadastrar(nome, escolha);
            cout << "Localizacao cadastrada: " << s.grafo.getLocal(escolha).getNome() << "\n";
            salvar(s);
            return;
        }
    }
}

//  Fazer pedido (escolher tipo de prato)
void fazerPedido(Sistema& s) {
    vector<string> pratos = s.indicePratos.chaves();
    cout << "\n--- Tipos de prato ---\n";
    for (size_t i = 0; i < pratos.size(); i++) {
        const auto* ids = s.indicePratos.buscar(pratos[i]);
        cout << "  " << setw(2) << i + 1 << ". " << pratos[i]
             << " (" << ids->size() << " restaurantes)\n";
    }
    int n = lerInteiro("Escolha o prato (0 para cancelar): ");
    if (n < 1 || n > static_cast<int>(pratos.size())) { cout << "Cancelado.\n"; return; }
    s.pratoEscolhido = pratos[n - 1];
    cout << "Prato escolhido: " << s.pratoEscolhido
         << ". Use a opcao 3 para buscar o restaurante.\n";
}

//  Buscar restaurante mais próximo com o prato desejado
void buscarRestaurante(Sistema& s) {
    if (!s.cliente.temLocal()) { cout << "Cadastre sua localizacao primeiro (opcao 1).\n"; return; }
    if (s.pratoEscolhido.empty()) { cout << "Escolha um prato primeiro (opcao 2).\n"; return; }

    // acha direto os restaurantes que servem o prato
    const auto* ids = s.indicePratos.buscar(s.pratoEscolhido);
    if (!ids) { cout << "Nenhum restaurante serve esse prato.\n"; return; }

    // Dijkstra a partir do cliente: distância até TODOS os nós de uma vez
    ResultadoDijkstra r = dijkstra(s.grafo, s.cliente.getIdLocal());

    struct Candidato { int idRest; double dist; };
    vector<Candidato> cand;
    for (int id : *ids) {
        int local = s.restaurantes[id].getIdLocal();
        if (r.alcancavel(local)) cand.push_back({id, r.dist[local]});
    }
    if (cand.empty()) { cout << "Nenhum restaurante alcancavel.\n"; return; }

       cout << "Ordenar por: 1) distancia  2) avaliacao: ";
    int crit = lerInteiro("");

    // Diz se o candidato 'a' deve ficar ANTES do candidato 'b' na lista.
    //  - criterio 2: maior nota primeiro
    //  - senao: menor distancia primeiro
    auto deveVirAntes = [&](const Candidato& a, const Candidato& b) {
        if (crit == 2)
            return s.restaurantes[a.idRest].getAvaliacao() > s.restaurantes[b.idRest].getAvaliacao();
        return a.dist < b.dist;
    };

    // INSERTION SORT: a parte da esquerda (indices 0 .. i-1) ja esta ordenada.
    // A cada volta, pegamos o elemento i e o encaixamos na posicao certa dessa parte.
    for (size_t i = 1; i < cand.size(); i++) {
        Candidato atual = cand[i];      // elemento que vamos encaixar
        size_t j = i;

        // Empurra uma casa para a direita todo elemento que deve vir DEPOIS de 'atual'
        while (j > 0 && deveVirAntes(atual, cand[j - 1])) {
            cand[j] = cand[j - 1];
            j--;
        }
        cand[j] = atual;                // achou o lugar: coloca o elemento aqui
    }
    
    size_t mostrar = min<size_t>(5, cand.size());
    cout << fixed << "\n--- Restaurantes com " << s.pratoEscolhido << " ---\n";
    for (size_t i = 0; i < mostrar; i++) {
        const Restaurante& rest = s.restaurantes[cand[i].idRest];
        cout << "  " << i + 1 << ". " << rest.getNome() << " | "
             << setprecision(2) << cand[i].dist / 1000.0 << " km | nota "
             << setprecision(1) << rest.getAvaliacao() << "\n";
    }
    int esc = lerInteiro("Escolha (1 = primeiro da lista, 0 = cancelar): ");
    if (esc < 1 || esc > static_cast<int>(mostrar)) { cout << "Cancelado.\n"; return; }

    // Cria o pedido e coloca na FILA
    const Candidato& c = cand[esc - 1];
    Pedido p;
    p.id = s.proximoIdPedido++;
    p.cliente = s.cliente.getNome();
    p.prato = s.pratoEscolhido;
    p.idRestaurante = c.idRest;
    p.rota = r.caminhoAte(s.restaurantes[c.idRest].getIdLocal());
    p.distancia = c.dist;
    p.custo = Pedido::calcularCusto(c.dist);
    s.filaPedidos.push(p);
    salvar(s);

    cout << "\nPedido criado e adicionado a fila:\n";
    imprimirResumo(p, s);
    cout << "Veja a rota na opcao 4 e finalize na opcao 5.\n";
}

//  Visualizar mapa de conexões / rota calculada
void visualizarMapa(Sistema& s) {
    if (s.filaPedidos.empty()) {
        cout << "Nenhum pedido na fila (use a opcao 3).\n";
        return;
    }
    const Pedido& p = s.filaPedidos.front();
    cout << "\n--- Rota do pedido #" << p.id << " ---\n";
    imprimirRota(s.grafo, p.rota);
}

//  Cálculo do custo da rota (e conclusão do pedido)
void calcularCusto(Sistema& s) {
    if (s.filaPedidos.empty()) { cout << "Nenhum pedido na fila (use a opcao 3).\n"; return; }
    Pedido p = s.filaPedidos.front();

    cout << fixed << setprecision(2)
         << "\n--- Custo do pedido #" << p.id << " ---\n"
         << "  Taxa base:        R$ " << Pedido::TAXA_BASE << "\n"
         << "  Distancia:        " << p.distancia / 1000.0 << " km x R$ "
         << Pedido::POR_KM << "/km = R$ " << Pedido::POR_KM * p.distancia / 1000.0 << "\n"
         << "  TOTAL:            R$ " << p.custo << "\n";

    string r = lerLinha("Confirmar entrega? (s/n): ");
    s.filaPedidos.pop();                          // sai da fila 
    p.concluido = !r.empty() && (r[0] == 's' || r[0] == 'S');
    s.historico.push(p);                          // vai para o topo da PILHA do histórico
    cout << (p.concluido ? "Pedido concluido!\n" : "Pedido cancelado.\n");
    salvar(s);
}

//  Exibir histórico de pedidos
void exibirHistorico(const Sistema& s) {
    if (s.historico.empty()) { cout << "Historico vazio.\n"; return; }
    cout << "\n--- Historico (mais recente primeiro) ---\n";
    stack<Pedido> copia = s.historico;            // copia para poder desempilhar sem alterar o original
    while (!copia.empty()) {
        const Pedido& p = copia.top();
        cout << (p.concluido ? "[entregue]  " : "[cancelado] ") << "\n";
        imprimirResumo(p, s);
        copia.pop();
    }
}

// ---------------- Programa principal ----------------

void exibirMenu(const Sistema& s) {
    cout << "\n===== ENTREGAS PLANO PILOTO =====\n";
    if (s.cliente.temLocal())
        cout << "Cliente: " << s.cliente.getNome() << " em "
             << s.grafo.getLocal(s.cliente.getIdLocal()).getNome() << "\n";
    else
        cout << "Cliente: (localizacao nao cadastrada)\n";
    if (!s.pratoEscolhido.empty()) cout << "Prato escolhido: " << s.pratoEscolhido << "\n";
    if (!s.filaPedidos.empty()) cout << "Pedidos na fila: " << s.filaPedidos.size() << "\n";
    cout << "1. Cadastrar localizacao do cliente\n"
         << "2. Fazer pedido (escolher tipo de prato)\n"
         << "3. Buscar restaurante mais proximo com o prato desejado\n"
         << "4. Visualizar mapa de conexoes / rota calculada\n"
         << "5. Calculo do custo da rota\n"
         << "6. Exibir historico de pedidos\n"
         << "7. Sair\n";
}

int main() {
    Sistema s;

    // Carrega o mapa e os restaurantes dos arquivos CSV
    if (!s.grafo.carregarLocais("dados/locais.csv") ||
        !s.grafo.carregarArestas("dados/arestas.csv")) {
        cerr << "Falha ao carregar o mapa. Rode o programa na pasta do projeto.\n";
        return 1;
    }
    s.restaurantes = carregarRestaurantes("dados/restaurantes.csv");
    if (s.restaurantes.empty()) { cerr << "Nenhum restaurante carregado.\n"; return 1; }

    // guarda o id do restaurante
    // Usamos a POSICAO no vetor (i), nao o id do CSV, porque o resto do codigo
    // acessa s.restaurantes[id]. Assim nunca sai do vetor, mesmo que o CSV comece em 1.
    for (size_t i = 0; i < s.restaurantes.size(); i++)
        for (const string& prato : s.restaurantes[i].getPratos())
            s.indicePratos.inserir(prato, static_cast<int>(i));

    cout << "Mapa: " << s.grafo.totalLocais() << " locais | "
         << s.restaurantes.size() << " restaurantes | "
         << s.indicePratos.tamanho() << " tipos de prato\n";

    // Recupera cliente, fila e historico da execucao anterior (se houver arquivo)
    if (carregarEstado(ARQUIVO_ESTADO, s.grafo.totalLocais(),
                       static_cast<int>(s.restaurantes.size()),
                       s.cliente, s.filaPedidos, s.historico, s.proximoIdPedido))
        cout << "Dados anteriores carregados: " << s.historico.size()
             << " no historico, " << s.filaPedidos.size() << " na fila.\n";

    int opcao = 0;
    do {
        exibirMenu(s);
        opcao = lerInteiro("Escolha: ");
        switch (opcao) {
        case 1: cadastrarCliente(s); break;
        case 2: fazerPedido(s); break;
        case 3: buscarRestaurante(s); break;
        case 4: visualizarMapa(s); break;
        case 5: calcularCusto(s); break;
        case 6: exibirHistorico(s); break;
        case 7: cout << "Ate logo!\n"; break;
        default: cout << "Opcao invalida.\n";
        }
    } while (opcao != 7 && !cin.eof());

    return 0;
}
