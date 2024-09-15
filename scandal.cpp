#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <vector>

using namespace std;

struct Graph {
    int V;
    vector<list<int>> adj;
};

Graph createGraph(int V) {
    Graph graph;
    graph.V = V;
    graph.adj.resize(V);
    return graph;
}

void addEdge(Graph& graph, int u, int v) { graph.adj[u].push_back(v); }

vector<int> dfs_order;
vector<int> component;
vector<bool> visited;

// functie de DFS care parcurge graful original si adauga
// nodurile intr-o lista in ordinea terminarii explorarii
void dfs1(const Graph& graph, int node) {
    visited[node] = true;
    for (int neigh : graph.adj[node])
        if (!visited[neigh])
            dfs1(graph, neigh);
    dfs_order.push_back(node);
}

// functie de DFS care parcurge graful inversat si care
// marcheaza componentele tare conexe
void dfs2(const Graph& rev_graph, int node, int comp) {
    component[node] = comp;
    for (int neigh : rev_graph.adj[node])
        if (component[neigh] == -1)
            dfs2(rev_graph, neigh, comp);
}

// construim muchiile grafului initial si inversat
void add_implication(Graph& graph, Graph& rev_graph, int u, int v) {
    addEdge(graph, u, v);
    addEdge(rev_graph, v, u);
}

int main() {
    ifstream f("scandal.in");
    ofstream g("scandal.out");

    int N, M;
    f >> N >> M;

    Graph graph = createGraph(2 * N);
    Graph rev_graph = createGraph(2 * N);

    for (int i = 0; i < M; ++i) {
        int x, y, c;
        f >> x >> y >> c;
        // ca sa avem indexarea nodurilor de la 0
        --x;
        --y;

        // adaugam muchiile in graf, fiecarei relatii ii
        // corespund una sau doua implicatii
        if (c == 0) {
            add_implication(graph, rev_graph, 2 * x + 1, 2 * y);  // ¬x → y
            add_implication(graph, rev_graph, 2 * y + 1, 2 * x);  // ¬y → x
        } else if (c == 1) {
            add_implication(graph, rev_graph, 2 * x + 1, 2 * y + 1);  // ¬x → ¬y
            add_implication(graph, rev_graph, 2 * y, 2 * x);  // y → x
        } else if (c == 2) {
            add_implication(graph, rev_graph, 2 * y + 1, 2 * x + 1);  // ¬y → ¬x
            add_implication(graph, rev_graph, 2 * x, 2 * y);  // x → y
        } else if (c == 3) {
            add_implication(graph, rev_graph, 2 * x, 2 * y + 1);  // x → ¬y
            add_implication(graph, rev_graph, 2 * y, 2 * x + 1);  // y → ¬x
        }
    }

    f.close();

    // determinam componentele tare conexe folosind algoritmul Kosaraju
    visited.assign(2 * N, false);
    for (int i = 0; i < 2 * N; ++i)
        if (!visited[i]) dfs1(graph, i);

    component.assign(2 * N, -1);
    for (int i = 2 * N - 1, j = 0; i >= 0; i--)
        if (component[dfs_order[i]] == -1)
            dfs2(rev_graph, dfs_order[i], j++);

    // construim si afisam o solutie valida
    set<int> answer;
    for (int i = 0; i < N; ++i) {
        // daca doua variabile x si ¬x apar in aceeasi componenta tare
        // conexa, problema nu are solutie si se scrie 0 în fisierul de iesire
        if (component[2 * i] == component[2 * i + 1]) {
            g << "0" << endl;
            g.close();
            return 0;
        }
        // daca nu, se construieste o solutie valida
        // variabila x este adevarata daca x este in componenta cu index
        // mai mare decat ¬x
        if (component[2 * i] > component[2 * i + 1]) answer.insert(i + 1);
    }
    // in fisierul de iesire scriem numarul de invitati si indecsii lor
    g << answer.size() << "\n";
    for (int x : answer) g << x << "\n";

    g.close();

    return 0;
}
