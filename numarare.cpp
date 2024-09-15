#include <bits/stdc++.h>
#include <algorithm>
#include <iostream>

using namespace std;

ifstream f("numarare.in");
ofstream g("numarare.out");

struct Graph {
    int V;
    // lista de adicenta a grafului
    vector<list<int>> adj;
    // vector in care retinem gradul interior al nodurilor
    vector<int> in_degree;
};

Graph create_graph(int V) {
    Graph graph;
    graph.V = V;
    graph.adj.resize(V);
    // initializam toate gradele interioare cu 0
    graph.in_degree.resize(V, 0);
    return graph;
}

void add_edge(Graph &graph, int u, int v) {
    graph.adj[u].push_back(v);
    // incrementam gradul interior al nodului v pentru ca
    // el primeste o muchie de la nodul u
    graph.in_degree[v]++;
}

// functia de sortare topologica inversa
vector<int> topological_sorting(Graph &graph) {
    // vectorul in care vom stoca sortarea topologica
    vector<int> result;
    // coada cu nodurile care vor trebui prelucrate
    queue<int> q;

    // initial, adaugam nodurile cu gradul interior 0 in coada
    for (int i = 0; i < graph.V; ++i) {
        if (graph.in_degree[i] == 0) {
            q.push(i);
        }
    }

    // folosim algoritmul lui Kahn pentru sortare topologica
    // cat timp mai avem noduri de prelucrat
    while (!q.empty()) {
        // il selectam pe primul din coada
        int u = q.front();
        q.pop();
        // adaugam nodul in sortarea topologica
        result.push_back(u);

        // parcurgem vecinii nodului curent si le scadem gradul
        // interior pe masura ce ii gasim
        for (int v : graph.adj[u]) {
            graph.in_degree[v]--;
            // atunci cand gradul interior devine 0, putem
            // aduga nodul in coada
            if (graph.in_degree[v] == 0) {
                q.push(v);
            }
        }
    }

    return result;
}

int common_paths(int source, int destination, Graph &graph1, Graph &graph2) {
    // vectorul dp pentru a memora numarul de cai catre
    // fiecare nod (rezultatele partiale)
    vector<int> dp(graph1.V, 0);
    // sortam topologic nodurile
    vector<int> sorted_nodes = topological_sorting(graph1);

    // numarul de cai destinatie -> destinatie = 1
    dp[destination] = 1;

    // actualizam numarul de cai pentru fiecare nod
    for (int i = sorted_nodes.size() - 1; i >= 0; --i) {
        int u = sorted_nodes[i];
        for (int v : graph1.adj[u]) {
            // actualizam numarul de cai pentru fiecare nod
            auto found = find(graph2.adj[u].begin(), graph2.adj[u].end(), v);
            // daca muchia corespunzatoare exista si in graful 2,
            // adunam caile posibile
            if (found != graph2.adj[u].end()) {
                dp[u] += dp[v];
                dp[u] %= 1000000007;
            }
        }
    }

    // numarul de cai sursa -> destinatie (rezultatul final cautat)
    return dp[source];
}

int main() {
    int N, M, u, v;
    // citim numarul de noduri si de muchii
    f >> N >> M;

    // acum trebuie sa cream grafurile si sa
    // citim listele de adiacenta
    // primul graf:
    Graph graph1 = create_graph(N);
    for (int i = 1; i <= M; i++) {
        f >> u >> v;
        add_edge(graph1, u - 1, v - 1);
    }
    // al doilea graf:
    Graph graph2 = create_graph(N);
    for (int i = 1; i <= M; i++) {
        f >> u >> v;
        add_edge(graph2, u - 1, v - 1);
    }

    // pentru sortarea topologica inversa (avem nevoie de ea pentru
    // a evalua dp[u] pentru fiecare nod u o singura data) ne trebuie
    // un mod de a retine care noduri nu au niciun parinte (adica
    // au gradul interior 0)
    g << common_paths(0, N - 1, graph1, graph2) << endl;
}
