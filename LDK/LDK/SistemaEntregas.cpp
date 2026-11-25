
#include "SistemaEntregas.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

//  Entrada de dados segura

int SistemaEntregas::lerInteiro(const string& mensagem) {
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

string SistemaEntregas::lerLinha(const string& mensagem) {
    cout << mensagem;
    string s;
    getline(cin >> ws, s);   // ws: ignora o '\n' que sobrou do cin >>
    return s;
}

string SistemaEntregas::minusculo(string s) {
    for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

//  Funções auxiliares de exibição

void SistemaEntregas::imprimirRota(const vector<int>& rota) const {
    for (size_t i = 0; i < rota.size(); i++) {
        cout << "  " << setw(2) << i + 1 << ". " << grafo.getLocal(rota[i]).getNome();
        if (i == 0) cout << "  (origem)";
        else if (i + 1 == rota.size()) cout << "  (restaurante)";
        if (i > 0) {
            cout << "  [+" << fixed << setprecision(0)
                 << grafo.pesoAresta(rota[i - 1], rota[i]) << " m]";
        }
        cout << "\n";
    }
}

void SistemaEntregas::imprimirResumo(const Pedido& p) const {
    cout << fixed << setprecision(2);
    cout << "  Pedido #" << p.getId() << " | " << p.getPrato() << " | "
         << restaurantes[p.getIdRestaurante()].getNome() << "\n"
         << "  Distancia: " << p.getDistancia() / 1000.0 << " km | Custo: R$ "
         << p.getCusto() << " | Tempo: ~"
         << setprecision(0) << Pedido::estimarMinutos(p.getDistancia()) << " min\n";
}

void SistemaEntregas::exibirMenu() const {
    cout << "\n===== ENTREGAS PLANO PILOTO =====\n";
    if (cliente.temLocal())
        cout << "Cliente: " << cliente.getNome() << " em "
             << grafo.getLocal(cliente.getIdLocal()).getNome() << "\n";
    else
        cout << "Cliente: (localizacao nao cadastrada)\n";
    if (!pratoEscolhido.empty()) cout << "Prato escolhido: " << pratoEscolhido << "\n";
    if (!filaPedidos.empty()) cout << "Pedidos na fila: " << filaPedidos.size() << "\n";
    cout << "1. Cadastrar localizacao do cliente\n"
         << "2. Fazer pedido (escolher tipo de prato)\n"
         << "3. Buscar restaurante mais proximo com o prato desejado\n"
         << "4. Rota calculada\n"
         << "5. Calculo do custo da rota\n"
         << "6. Exibir historico de pedidos\n"
         << "7. Sair\n";
}

//  Opções do menu

//  Cadastrar localização do cliente
void SistemaEntregas::cadastrarCliente() {
    string nome = lerLinha("Seu nome: ");
    while (true) {
        string busca = lerLinha("Onde voce esta? Digite parte do nome (ex: SQN 108, Conjunto, Torre): ");
        string b = minusculo(busca);

        // Procura locais cujo nome contém o texto digitado (ex: "108" acha SQN 108 e SQS 108)
        vector<int> achados;
        for (int i = 0; i < grafo.totalLocais(); i++)
            if (minusculo(grafo.getLocal(i).getNome()).find(b) != string::npos)
                achados.push_back(i);

        if (achados.empty()) { cout << "Nenhum local encontrado.\n"; continue; }
        if (achados.size() > 15) { cout << achados.size() << " resultados; seja mais especifico.\n"; continue; }

        for (int id : achados)
            cout << "  " << setw(3) << id << " | " << grafo.getLocal(id).getNome()
                 << " [" << grafo.getLocal(id).getTipo() << "]\n";
        int escolha = lerInteiro("Digite o id do seu local (-1 para buscar de novo): ");
        if (escolha >= 0 && grafo.idValido(escolha)) {
            cliente.cadastrar(nome, escolha);
            cout << "Localizacao cadastrada: " << grafo.getLocal(escolha).getNome() << "\n";
            salvar();
            return;
        }
    }
}

//  Fazer pedido (escolher tipo de prato)
void SistemaEntregas::fazerPedido() {
    vector<string> pratos = indicePratos.chaves();
    cout << "\n--- Tipos de prato ---\n";
    for (size_t i = 0; i < pratos.size(); i++) {
        const auto* ids = indicePratos.buscar(pratos[i]);
        cout << "  " << setw(2) << i + 1 << ". " << pratos[i]
             << " (" << ids->size() << " restaurantes)\n";
    }
    int n = lerInteiro("Escolha o prato (0 para cancelar): ");
    if (n < 1 || n > static_cast<int>(pratos.size())) { cout << "Cancelado.\n"; return; }
    pratoEscolhido = pratos[n - 1];
    cout << "Prato escolhido: " << pratoEscolhido
         << ". Use a opcao 3 para buscar o restaurante.\n";
}

// INSERTION SORT: a parte da esquerda (indices 0 .. i-1) ja esta ordenada.
// A cada volta, pegamos o elemento i e o encaixamos na posicao certa dessa parte.
void SistemaEntregas::ordenarCandidatos(vector<Candidato>& cand, int criterio) const {
    // Diz se o candidato 'a' deve ficar ANTES do candidato 'b' na lista.
    //  - criterio 2: maior nota primeiro
    //  - senao: menor distancia primeiro
    auto deveVirAntes = [&](const Candidato& a, const Candidato& b) {
        if (criterio == 2)
            return restaurantes[a.idRest].getAvaliacao() > restaurantes[b.idRest].getAvaliacao();
        return a.dist < b.dist;
    };

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
}

//  Buscar restaurante mais próximo com o prato desejado
void SistemaEntregas::buscarRestaurante() {
    if (!cliente.temLocal()) { cout << "Cadastre sua localizacao primeiro (opcao 1).\n"; return; }
    if (pratoEscolhido.empty()) { cout << "Escolha um prato primeiro (opcao 2).\n"; return; }

    // acha direto os restaurantes que servem o prato
    const auto* ids = indicePratos.buscar(pratoEscolhido);
    if (!ids) { cout << "Nenhum restaurante serve esse prato.\n"; return; }

    // Dijkstra a partir do cliente: distância até TODOS os nós de uma vez
    // (o objeto já executa o algoritmo ao ser criado)
    Dijkstra dij(grafo, cliente.getIdLocal());

    vector<Candidato> cand;
    for (int id : *ids) {
        int local = restaurantes[id].getIdLocal();
        if (dij.alcancavel(local)) cand.push_back({id, dij.distanciaAte(local)});
    }
    if (cand.empty()) { cout << "Nenhum restaurante alcancavel.\n"; return; }

    int crit = lerInteiro("Ordenar por: 1) distancia  2) avaliacao: ");
    ordenarCandidatos(cand, crit);

    size_t mostrar = min<size_t>(5, cand.size());
    cout << fixed << "\n--- Restaurantes com " << pratoEscolhido << " ---\n";
    for (size_t i = 0; i < mostrar; i++) {
        const Restaurante& rest = restaurantes[cand[i].idRest];
        cout << "  " << i + 1 << ". " << rest.getNome() << " | "
             << setprecision(2) << cand[i].dist / 1000.0 << " km | nota "
             << setprecision(1) << rest.getAvaliacao() << "\n";
    }
    int esc = lerInteiro("Escolha (1 = primeiro da lista, 0 = cancelar): ");
    if (esc < 1 || esc > static_cast<int>(mostrar)) { cout << "Cancelado.\n"; return; }

    // Cria o pedido e coloca na FILA (o construtor do Pedido já calcula o custo)
    const Candidato& c = cand[esc - 1];
    Pedido p(proximoIdPedido++, cliente.getNome(), pratoEscolhido, c.idRest,
             dij.caminhoAte(restaurantes[c.idRest].getIdLocal()), c.dist);
    filaPedidos.push(p);
    salvar();

    cout << "\nPedido criado e adicionado a fila:\n";
    imprimirResumo(p);
    cout << "Veja a rota na opcao 4 e finalize na opcao 5.\n";
}

//  rota calculada
void SistemaEntregas::visualizarMapa() const {
    if (filaPedidos.empty()) {
        cout << "Nenhum pedido na fila (use a opcao 3).\n";
        return;
    }
    const Pedido& p = filaPedidos.front();
    cout << "\n--- Rota do pedido #" << p.getId() << " ---\n";
    imprimirRota(p.getRota());
}

//  Cálculo do custo da rota (e conclusão do pedido)
void SistemaEntregas::calcularCusto() {
    if (filaPedidos.empty()) { cout << "Nenhum pedido na fila (use a opcao 3).\n"; return; }
    Pedido p = filaPedidos.front();     // cópia: o original sai da fila logo abaixo

    cout << fixed << setprecision(2)
         << "\n--- Custo do pedido #" << p.getId() << " ---\n"
         << "  Taxa base:        R$ " << Pedido::TAXA_BASE << "\n"
         << "  Distancia:        " << p.getDistancia() / 1000.0 << " km x R$ "
         << Pedido::POR_KM << "/km = R$ " << Pedido::POR_KM * p.getDistancia() / 1000.0 << "\n"
         << "  TOTAL:            R$ " << p.getCusto() << "\n";

    string r = lerLinha("Confirmar entrega? (s/n): ");
    filaPedidos.pop();                          // sai da fila
    p.finalizar(!r.empty() && (r[0] == 's' || r[0] == 'S'));
    historico.push(p);                          // vai para o topo da PILHA do histórico
    cout << (p.foiConcluido() ? "Pedido concluido!\n" : "Pedido cancelado.\n");
    salvar();
}

//  Exibir histórico de pedidos
void SistemaEntregas::exibirHistorico() const {
    if (historico.empty()) { cout << "Historico vazio.\n"; return; }
    cout << "\n--- Historico (mais recente primeiro) ---\n";
    stack<Pedido> copia = historico;            // copia para poder desempilhar sem alterar o original
    while (!copia.empty()) {
        const Pedido& p = copia.top();
        cout << (p.foiConcluido() ? "[entregue]  " : "[cancelado] ") << "\n";
        imprimirResumo(p);
        copia.pop();
    }
}

// ---------------- Programa principal ----------------

bool SistemaEntregas::iniciar() {
    // Carrega o mapa e os restaurantes dos arquivos CSV
    if (!grafo.carregarLocais("dados/locais.csv") ||
        !grafo.carregarArestas("dados/arestas.csv")) {
        cerr << "Falha ao carregar o mapa. Rode o programa na pasta do projeto.\n";
        return false;
    }
    restaurantes = Restaurante::carregarDoCsv("dados/restaurantes.csv");
    if (restaurantes.empty()) { cerr << "Nenhum restaurante carregado.\n"; return false; }

    // guarda o id do restaurante
    // Usamos a POSICAO no vetor (i), nao o id do CSV, porque o resto do codigo
    // acessa restaurantes[id]. Assim nunca sai do vetor, mesmo que o CSV comece em 1.
    for (size_t i = 0; i < restaurantes.size(); i++)
        for (const string& prato : restaurantes[i].getPratos())
            indicePratos.inserir(prato, static_cast<int>(i));

    cout << "Mapa: " << grafo.totalLocais() << " locais | "
         << restaurantes.size() << " restaurantes | "
         << indicePratos.tamanho() << " tipos de prato\n";

    // Recupera cliente, fila e historico da execucao anterior (se houver arquivo)
    if (persistencia.carregar(grafo.totalLocais(),
                              static_cast<int>(restaurantes.size()),
                              cliente, filaPedidos, historico, proximoIdPedido))
        cout << "Dados anteriores carregados: " << historico.size()
             << " no historico, " << filaPedidos.size() << " na fila.\n";
    return true;
}

void SistemaEntregas::executar() {
    int opcao = 0;
    do {
        exibirMenu();
        opcao = lerInteiro("Escolha: ");
        switch (opcao) {
        case 1: cadastrarCliente(); break;
        case 2: fazerPedido(); break;
        case 3: buscarRestaurante(); break;
        case 4: visualizarMapa(); break;
        case 5: calcularCusto(); break;
        case 6: exibirHistorico(); break;
        case 7: cout << "Ate logo!\n"; break;
        default: cout << "Opcao invalida.\n";
        }
    } while (opcao != 7 && !cin.eof());
}
