#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#define V 5 // number of desired nodes/vertices

// Selects the unvisited node with the smallest distance
int min_distance(int dist[], bool visited[]) {
	int min = INT_MAX, min_index;
	for (int v = 0; v < V; v++) {
		if (!visited[v] && dist[v] <= min) {
			min = dist[v];
			min_index = v;
		}
	}
	return min_index;
}

// Repeatedly finds the unvisited node with smallest tentative distance and 
// updates distances of its neighbors when shorter path is found
void dijkstra(int graph[V][V], int src) {
	int dist[V];
	bool visited[V];

	for (int i = 0; i < V; i++) { 
		dist[i] = INT_MAX;
		visited[i] = false;
	}
	dist[src] = 0; 

	for (int count = 0; count < V - 1; count++) {
		int u = min_distance(dist, visited);
		visited[u] = true;
		for (int v = 0; v < V; v++) {
			if (!visited[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
				dist[v] = dist[u] + graph[u][v];
			}
		}
	}
	// print shortest distance from source vertex to all other vertices
	printf("Vertex \t Distance from Source: %d\n", src);
	for (int i = 0; i < V; i++) {
		if (dist[i] == INT_MAX) {
			printf("%d \t INF\n", i);
		} else {
			printf("%d \t %d\n", i, dist[i]); 
		}
	}
}

int main() {
	int graph[V][V] = {{0, 10, 0, 0, 5}, {0, 0, 1, 0, 2}, {0, 0, 0, 4, 0}, {7, 0, 6, 0, 0}, {0, 3, 9, 2, 0}};
	dijkstra(graph, 0);
	return 0;
}

