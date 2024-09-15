#include <limits.h>
#include <unordered_map>
#include <vector>
#include <list>
#include <stack>
#include <iostream>
#include <fstream>

using namespace std;

struct Graph {
    int V;
    // lista de adicenta a grafului, pentru fiecare nod avem
    // o lista cu vecini
    vector<list<int>> adj;
};

Graph createGraph(int V) {
    Graph graph;
    graph.V = V;
    graph.adj.resize(V);
    return graph;
}

void addEdge(Graph& graph, int u, int v) {
    graph.adj[u].push_back(v);
}

// am folosit algoritmul de sortare topologica pe baza de DFS
void topologicalSortUtil(int v, vector<bool>& visited, stack<int>& Stack,
const Graph& graph) {
    visited[v] = true;

    // se face o parcurgere DFS pentru determinarea timpilor de falizare
    for (const auto& neighbor : graph.adj[v]) {
        int u = neighbor;
        if (!visited[u]) {
            topologicalSortUtil(u, visited, Stack, graph);
        }
    }

    Stack.push(v);
}

stack<int> topologicalSort(const Graph& graph) {
    stack<int> Stack;
    vector<bool> visited(graph.V, false);

    for (int i = 0; i < graph.V; i++) {
        if (!visited[i]) {
            topologicalSortUtil(i, visited, Stack, graph);
        }
    }

    return Stack;
}

// functie pentru a gasi distanta cea mai lunga de la nodul sursa la
// toate celelalte (de fapt, numarul maxim de orase dintre sursa si
// destinatii)
vector<int> longest_path(const Graph& graph, int s) {
    // facem sortarea topologica si o retinem in Stack
    stack<int> Stack = topologicalSort(graph);
    vector<int> dist(graph.V, -1);
    dist[s] = 1;

    while (!Stack.empty()) {
        int u = Stack.top();
        Stack.pop();

        if (dist[u] != -1) {
            for (const auto& neighbor : graph.adj[u]) {
                int v = neighbor;
                // daca "cea mai lunga cale" (distanta)
                // catre un oras conectat este mai mica
                // decat distanta actuala a orasului + 1
                // (adica conexiunea adauga un alt hop),
                // distanta orasului conectat este actualizata
                if (dist[v] < dist[u] + 1) {
                    dist[v] = dist[u] + 1;
                }
            }
        }
    }

    return dist;
}

int main() {
    ifstream f("trenuri.in");
    ofstream g("trenuri.out");

    // am ales ca ulterior sa transform numele de orase in indecsi
    // pentru a lucra mai usor cu acestea
    unordered_map<string, int> indexes;
    int index = 0;
    string start_city, destination_city;
    f >> start_city >> destination_city;
    indexes[start_city] = index++;
    indexes[destination_city] = index++;

    int m;
    f >> m;

    // vectorul cu muchiile (rutele) existente
    vector<pair<string, string>> routes(m);
    for (int i = 0; i < m; ++i) {
        string city1, city2;
        // citim pe rand rutele directe
        f >> city1 >> city2;
        // daca orasul nu are deja indice, il adaugam
        if (indexes.find(city1) == indexes.end()) {
            indexes[city1] = index++;
        }
        if (indexes.find(city2) == indexes.end()) {
            indexes[city2] = index++;
        }
        // marcam muchia
        routes[i] = make_pair(city1, city2);
    }
    // numarul de orase
    int n = indexes.size();
    // cream graful si ii adaugam muchiile
    Graph graph = createGraph(n);
    for (const auto& route : routes) {
        addEdge(graph, indexes[route.first], indexes[route.second]);
    }
    // calculam distantele maxime de la sursa la toate celelalte orase
    vector<int> dist = longest_path(graph, indexes[start_city]);
    // si afisam valoarea pentru destinatia (orasul) cautata
    g << dist[indexes[destination_city]] << endl;

    return 0;
}
