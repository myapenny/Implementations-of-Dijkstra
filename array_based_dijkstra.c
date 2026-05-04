#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

// Data structures for adjacent list
struct AdjListNode {
	int dest;
	int weight;
	struct AdjListNode* next;
};

struct AdjList {
	struct AdjListNode* head;
};

struct GraphList {
	int V;
	struct AdjList* array;
};

struct AdjListNode* newAdjListNode(int dest, int weight) {
	struct AdjListNode* newNode = (struct AdjListNode*)malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}

struct GraphList* createGraphList(int V) {
	struct GraphList* graph = (struct GraphList*)malloc(sizeof(struct GraphList));
	graph->V = V;
	graph->array = (struct AdjList*)malloc(V * sizeof(struct AdjList));
	for (int i = 0; i < V; ++i) {
		graph->array[i].head = NULL;
	}
	return graph;
}

// Undirected graph to add edge in both directions
void addEdgeList(struct GraphList* graph, int src, int dest, int weight) {
	// Add src to dest
	struct AdjListNode* newNode = newAdjListNode(dest, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;

	// Add dest to src
	newNode = newAdjListNode(src, weight);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}

// Data structures for adjacency matrix
struct GraphMatrix {
	int V;
	int** matrix;
};

struct GraphMatrix* createGraphMatrix(int V) {
	struct GraphMatrix* graph = (struct GraphMatrix*)malloc(sizeof(struct GraphMatrix));
	graph->V = V;
	graph->matrix = (int**)malloc(V * sizeof(int*));
	for (int i = 0; i < V; i++) {
		graph->matrix[i] = (int*)calloc(V, sizeof(int)); // Initialize with 0
	}
	return graph;
}

// Sets weight in both matrix coordinates
void addEdgeMatrix(struct GraphMatrix* graph, int src, int dest, int weight) {
	graph->matrix[src][dest] = weight;
	graph->matrix[dest][src] = weight;
}

// Selects the unvisited node with the smallest distance
int min_distance(int dist[], bool visited[], int V) {
	int min = INT_MAX, min_index = -1;
	for (int v = 0; v < V; v++) {
		if (!visited[v] && dist[v] <= min) {
			min = dist[v];
			min_index = v;
		}
	}
	return min_index;
}

// Version 1: Matrix
void dijkstra_matrix(struct GraphMatrix* graph, int src, int dist[]) {
	int V = graph->V;
	bool visited[V];

	for (int i = 0; i < V; i++) { 
		dist[i] = INT_MAX;
		visited[i] = false;
	}
	dist[src] = 0; 

	for (int count = 0; count < V - 1; count++) {
		int u = min_distance(dist, visited, V);
		if (u == -1) break; // disconnected graph -> break
		visited[u] = true;

		for (int v = 0; v < V; v++) {
			if (!visited[v] && graph->matrix[u][v] && dist[u] != INT_MAX && dist[u] + graph->matrix[u][v] < dist[v]) {
				dist[v] = dist[u] + graph->matrix[u][v];
			}
		}
	}
}

// Version 2: List
void dijkstra_list(struct GraphList* graph, int src, int dist[]) {
	int V = graph->V;
	bool visited[V];

	for (int i = 0; i < V; i++) {
		dist[i] = INT_MAX;
		visited[i] = false;
	}
	dist[src] = 0;

	for (int count = 0; count < V - 1; count++) {
		int u = min_distance(dist, visited, V);
		if (u == -1) break; // disconnected graph -> break
		visited[u] = true;

		struct AdjListNode* pCrawl = graph->array[u].head;
		while (pCrawl != NULL) {
			int v = pCrawl->dest;
			if (!visited[v] && dist[u] != INT_MAX && dist[u] + pCrawl->weight < dist[v]) {
				dist[v] = dist[u] + pCrawl->weight;
			}
			pCrawl = pCrawl -> next;
		}
	}
}

// Testing and metrics
typedef struct 
{
	int src, dest, weight;
} Edge;

void evaluate_graph(int V, Edge edges[], int num_edges, int source) {
	printf("---------------------------------------------\n");
	printf("Evaluating graph with Vertices: %d, Edges: %d,\n", V, num_edges);

	// calculate graph density to choose correct algorithm
	// adj matrix for very dense graphs
	// adj list on spares graphs
	float max_edges = (V * (V - 1)) / 2.0; // max # of possible edges in an undirected graph
	float density = (num_edges / max_edges);

	bool is_dense = density > 0.5; // 50% threshold for dense vs sparse
	printf("Graph Density: %.2f%% -> Classified as %s\n", density * 100, is_dense ? "Dense" : "Sparse");

	// Building both graphs for memory calculations and correctness checks
	struct GraphMatrix* gMatrix = createGraphMatrix(V);
	struct GraphList* gList = createGraphList(V);

	for (int i = 0; i < num_edges; i++) {
		addEdgeMatrix(gMatrix, edges[i].src, edges[i].dest, edges[i].weight);
		addEdgeList(gList, edges[i].src, edges[i].dest, edges[i].weight);
	}

	// to store distances
	int dist_primary[V];
	int dist_verify[V];

	// Runtime evaluation for 5 trials
	clock_t start, end;
	double time_taken, total_time = 0;

	if (is_dense) {
		printf("Algorithm Choice: Version 1 (adjacency matrix)\n");
		for (int i = 0; i < 5; i++) {
			start = clock();
			dijkstra_matrix(gMatrix, source, dist_primary);
			end = clock();
			total_time += ((double)(end - start)) / CLOCKS_PER_SEC;
		}
		// Run the other version once for correctness verification
		dijkstra_list(gList, source, dist_verify);
	} else {
		printf("Algorithm Choice: Version 2 (adjacency list)\n");
		for (int i = 0; i < 5; i++) {
			start = clock();
			dijkstra_list(gList, source, dist_primary);
			end = clock();
			total_time += ((double)(end - start)) / CLOCKS_PER_SEC;
		}
		// Run the other version once for correctness verification
		dijkstra_matrix(gMatrix, source, dist_verify);
	}

	printf("Average Runtime (5 trials): %f ms\n", (total_time * 1000.0) / 5.0);

	// Memory usage
	long mem_matrix = sizeof(struct GraphMatrix) + (V * sizeof(int*)) + (V * V * sizeof(int));
	// each undirected edge creates 2 nodes in the list
	long mem_list = sizeof (struct GraphList) + (V * sizeof(struct AdjList)) + (2 * num_edges * sizeof(struct AdjListNode));
	float kilobytes = 1000.0;

	if (is_dense) {
		printf("Memory Used (matrix): %.2f KB\n", (mem_matrix / kilobytes));
	} else {
		printf("Memory Used (list): %.2f KB\n", (mem_list / kilobytes));
	}

	// Correctness Check
	bool is_correct = true;
	for (int i = 0; i < V; i++) {
		if (dist_primary[i] != dist_verify[i]) {
			is_correct = false;
			break;
		}
	}
	printf("Correctness Verified: %s\n", is_correct ? "Yes" : "No");

	if (V > 10) {
		// printing the first 10 distances
		printf("\nFirst 10 Distances from Source %d:\n", source);
		for (int i = 0; i < 10; i++) {
			if (dist_primary[i] == INT_MAX) printf("Vertex %d \t INF\n", i);
			else printf("Vertex %d \t %d\n", i , dist_primary[i]);
		}
	}	else {
		//final distances
		printf("\nFinal Distances from Source %d:\n", source);
		for (int i = 0; i < V; i++) {
			if (dist_primary[i] == INT_MAX) printf("Vertex %d \t INF\n", i);
			else printf("Vertex %d \t %d\n", i , dist_primary[i]);
		}
	}

	// freeing allocated memory 
	for (int i = 0; i < V; i++) free(gMatrix->matrix[i]);
	free(gMatrix->matrix);
	free(gMatrix);

	for (int i = 0; i < V; i++) {
		struct AdjListNode* curr = gList->array[i].head;
		while (curr) {
			struct AdjListNode* temp = curr;
			curr = curr->next;
			free(temp);
		}
	}
	free(gList->array);
	free(gList);
}

int main() {
	
	// Sparse Graph 1 (represented with numbers instead of letters)
	int V_sparse_1 = 6;
	Edge sparse_edges_1[] = {
		{0, 1, 4}, {0, 2, 2}, // src 0 goes to dest 1 with weight 4 and src 0 goes to dest 2 with weight 2, etc...
		{1, 3, 5},
		{2, 3, 1},
		{3, 4, 3},
		{4, 5, 2}
	};
	int num_sparse_edges_1 = sizeof(sparse_edges_1) / sizeof(sparse_edges_1[0]);
	evaluate_graph(V_sparse_1, sparse_edges_1, num_sparse_edges_1, 0);
	
	// Sparse Graph 2 
	int V_sparse_2 = 7;
	Edge sparse_edges_2[] = {
		{0, 1, 3}, {0, 2, 6},
		{1, 3, 2}, {1, 4, 5},
		{2, 4, 4},
		{3, 5, 7},
		{4, 6, 1}
	};
	int num_sparse_edges_2 = sizeof(sparse_edges_2) / sizeof(sparse_edges_2[0]);
	evaluate_graph(V_sparse_2, sparse_edges_2, num_sparse_edges_2, 0);

	// Dense Graph 1
	int V_dense_1 = 5;
	Edge dense_edges_1[] = {
		{0, 1, 2}, {0, 2, 5}, {0, 3, 1}, {0, 4, 4},
		{1, 2, 3}, {1, 3, 2}, {1, 4, 6},
		{2, 3, 3}, {2, 4, 1},
		{3, 4, 2}
	};
	int num_dense_edges_1 = sizeof(dense_edges_1) / sizeof(dense_edges_1[0]);
	evaluate_graph(V_dense_1, dense_edges_1, num_dense_edges_1, 0);

	// Dense Graph 2
	int V_dense_2 = 6;
	Edge dense_edges_2[] = {
		{0, 1, 3}, {0, 2, 2}, {0, 3, 6}, {0, 4, 5}, {0, 5, 4},
		{1, 2, 1}, {1, 3, 2}, {1, 4, 4}, {1, 5, 7},
		{2, 3, 3}, {2, 4, 6}, {2, 5, 5},
		{3, 4, 2}, {3, 5, 4},
		{4, 5, 1}
	};
	int num_dense_edges_2 = sizeof(dense_edges_2) / sizeof(dense_edges_2[0]);
	evaluate_graph(V_dense_2, dense_edges_2, num_dense_edges_2, 0);

	// Dense Graph 3 (Large)
	int V_dense_3 = 100;
	int num_dense_edges_3 = 0;
	int max_edges_3 = ((V_dense_3) * (V_dense_3 - 1)) / 2;
	Edge* dense_edges_3 = malloc(max_edges_3 * sizeof(Edge));

	srand(42); 
	for (int i = 0; i < V_dense_3; i++) {
		for (int j = i+1; j < V_dense_3; j++) {
			dense_edges_3[num_dense_edges_3].src = i;
			dense_edges_3[num_dense_edges_3].dest = j;
			dense_edges_3[num_dense_edges_3].weight = (rand() % 15) + 1; // weight 1 - 15
			num_dense_edges_3++;
		}
	}
	evaluate_graph(V_dense_3, dense_edges_3, num_dense_edges_3, 0);
	free(dense_edges_3);

	// For theoretical experimentation
	
}