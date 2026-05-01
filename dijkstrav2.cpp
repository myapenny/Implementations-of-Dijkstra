#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <chrono>
#include <cstdlib>

using namespace std;

const int INF = INT_MAX;

// GRAPH
vector<vector<pair<int, int> > > adj;

// EDGE COUNT 
int getEdgeCount() 
{
    int count = 0;
    for (int i = 0; i < (int)adj.size(); i++) {
        count += adj[i].size();
    }
    return count / 2;
}

// MEMORY ESTIMATE
double estimateMemoryKB(int V) {
    long long edgeCount = getEdgeCount() * 2; // undirected graph

    long long adjMemory = edgeCount * sizeof(pair<int,int>);
    long long distMemory = V * sizeof(int);
    long long heapMemory = V * sizeof(pair<int,int>);

    long long totalBytes = adjMemory + distMemory + heapMemory;

    return totalBytes / 1024.0; // convert to KB
}

// DIJKSTRA
void dijkstra(int V, int source, vector<int> &dist) {
    dist.assign(V, INF);

    priority_queue<
        pair<int,int>,
        vector<pair<int,int> >,
        greater<pair<int,int> >
    > pq;

    dist[source] = 0;
    pq.push(make_pair(0, source));

    while (!pq.empty()) {
        pair<int,int> top = pq.top();
        pq.pop();

        int d = top.first;
        int u = top.second;

        if (d > dist[u]) continue;

        for (int i = 0; i < (int)adj[u].size(); i++) {
            int v = adj[u][i].first;
            int w = adj[u][i].second;

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push(make_pair(dist[v], v));
            }
        }
    }
}

// GRAPH UTIL 
void addEdge(int u, int v, int w) 
{
    adj[u].push_back(make_pair(v, w));
    adj[v].push_back(make_pair(u, w));
}

void clearGraph(int V) {
    adj.clear();
    adj.resize(V);
}

//GRAPHS 

// Sparse Graph 1
void buildSparse1() 
{
    clearGraph(6);
    addEdge(0,1,4);
    addEdge(0,2,2);
    addEdge(1,3,5);
    addEdge(2,3,1);
    addEdge(3,4,3);
    addEdge(4,5,2);
}

// Sparse Graph 2
void buildSparse2() 
{
    clearGraph(7);
    addEdge(0,1,3);
    addEdge(0,2,6);
    addEdge(1,3,2);
    addEdge(1,4,5);
    addEdge(2,4,4);
    addEdge(3,5,7);
    addEdge(4,6,1);
}

// Dense Graph 1
void buildDense1() 
{
    clearGraph(5);
    for (int i = 0; i < 5; i++)
        for (int j = i + 1; j < 5; j++)
            addEdge(i, j, (i + j) % 5 + 1);
}

// Dense Graph 2
void buildDense2() 
{
    clearGraph(6);
    for (int i = 0; i < 6; i++)
        for (int j = i + 1; j < 6; j++)
            addEdge(i, j, (i * j) % 7 + 1);
}


void buildDense3(int V) 
{
    clearGraph(V);

    srand(42); // same seed as Version 1 for consistency

    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            int weight = (rand() % 15) + 1; // weights 1–15
            addEdge(i, j, weight);
        }
    }
}

// TEST 
void testGraph(string name, int V) 
{
    cout << "\n----------------------\n";
    cout << "Graph: " << name << endl;
    cout << "Vertices: " << V << endl;
    cout << "Edges: " << getEdgeCount() << endl;

    double totalTime = 0;
    vector<int> dist;

    for (int i = 0; i < 5; i++) {
        auto start = chrono::high_resolution_clock::now();

        dijkstra(V, 0, dist);

        auto end = chrono::high_resolution_clock::now();

        totalTime += chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0;
    }

    cout << "Average Time: " << totalTime / 5 << " ms\n";

    cout << "Estimated Memory Usage: " << estimateMemoryKB(V) << " KB\n";

    cout << "Distances (first 10 nodes): ";
    for (int i = 0; i < V && i < 10; i++) {
        cout << dist[i] << " ";
    }
    cout << "\n";
}

// MAIN 
int main() 
{
    srand(time(NULL));

    buildSparse1();
    testGraph("Sparse Graph 1", 6);

    buildSparse2();
    testGraph("Sparse Graph 2", 7);

    buildDense1();
    testGraph("Dense Graph 1", 5);

    buildDense2();
    testGraph("Dense Graph 2", 6);

    buildDense3(100);
    testGraph("Dense Graph 3", 100);

    return 0;
}
