#include <climits>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <vector>

using namespace std;

ifstream f("drumuri.in");
ofstream g("drumuri.out");

struct Graph {
    int V;
    // lista de adiacenta a grafului
    vector<list<pair<int, int>>> adj;  // perechi (vecin, cost)
};

Graph create_graph(int V) {
    Graph graph;
    graph.V = V;
    graph.adj.resize(V);
    return graph;
}

void add_edge(Graph &graph, int u, int v, int cost) {
    graph.adj[u].push_back({v, cost});
}

Graph create_reverse_graph(const Graph graph) {
    int V = graph.V;
    Graph graph_rev = create_graph(V);

    for (int u = 0; u < V; ++u) {
        for (const auto &neighbor : graph.adj[u]) {
            int v = neighbor.first;
            int w = neighbor.second;
            // adaugam muchia inversata in graful invers
            add_edge(graph_rev, v, u, w);
        }
    }

    return graph_rev;
}

// am definit o functie dijkstra pentru a calcula distanta
// minima de la un nod sursa la toate celelalte
// pentru a fi mai eficenta ca timp de executie, am ales
// o implementare cu priority_queue (STL)
vector<long long int> dijkstra(Graph &graph, int source) {
    int n = graph.V;
    vector<long long int> dist(n, 100000000000000);
    dist[source] = 0;
    priority_queue<pair<long long int, int>, vector<pair<long long int, int>>,
    greater<pair<long long int, int>>> pq;
    pq.push({0, source});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        for (const auto &neighbor : graph.adj[u]) {
            int v = neighbor.first;
            int weight = neighbor.second;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }

    return dist;
}

// functie pentru a calcula suma minima a costurilor muchiilor
// trebuie aleasca o submultime de muchii astfel incat sa avem
// un cost total minim
long long int minEdgeSum(Graph graph, vector<long long int> distX,
                         vector<long long int> distY,
                         vector<long long int> dist_toZ, int dest) {
    // am pornit cu suma minima ca fiind suma costurilor drumurilor
    // minime de la X la Z, respectiv de la Y la Z
    // ulterior vom verifica daca nu cumva exista o muchie
    // intermediara comuna prin care daca ne construim drumurile
    // ajungem la un cost total al sumbultimii mai mic
    long long int minSum = distX[dest] + distY[dest];
    // pentru fiecare muchie
    for (size_t i = 0; i < graph.V; ++i) {
        for (const auto &neighbor : graph.adj[i]) {
            int v = neighbor.first;
            int w = neighbor.second;
            // avem muchia I->V
            // comparam suma minima curenta cu
            // suma costurilor de la X la I, de la Y la I,
            // costul muchiei si de la V la Z
            long long int sum = distX[i] + distY[i] + w + dist_toZ[v];
            minSum = min(minSum, sum);
        }
    }
    return minSum;
}

int main() {
    int n, m;
    f >> n >> m;

    Graph graph = create_graph(n);

    // citim muchiile si costurile aferente
    for (int i = 0; i < m; ++i) {
        int u, v, cost;
        f >> u >> v >> cost;
        add_edge(graph, u - 1, v - 1, cost);
    }

    int x, y, z;
    f >> x >> y >> z;

    // calcularea celor mai scurte drumuri de la X si Y la toate celelalte
    // noduri
    vector<long long int> distX = dijkstra(graph, x - 1);
    vector<long long int> distY = dijkstra(graph, y - 1);

    // facem un graf "invers", care are toate muchiile in sens opus celor
    // initiale pentru ca ne dorim sa aflam costurile minime ale drumurilor de
    // la fiecare nod la nodul z
    Graph graph_rev = create_reverse_graph(graph);
    vector<long long int> dist_toZ = dijkstra(graph_rev, z - 1);

    // calcularea sumei minime a costurilor muchiilor
    long long int minSum = minEdgeSum(graph, distX, distY, dist_toZ, z - 1);

    g << minSum << endl;

    return 0;
}
