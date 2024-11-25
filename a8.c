#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define INF INT_MAX

// Graph structure
typedef struct {
    int vs, vt; // Source and target vertices
    int *weights; // Periodic weights
} Edge;

// Dijkstra's Algorithm for dynamic weights
void dijkstra(int V, int N, int **adjList, Edge **edges, int *edgeCount, int start, int end) {
    int *dist = malloc(V * sizeof(int));
    int *prev = malloc(V * sizeof(int));
    int *visited = calloc(V, sizeof(int));
    
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0;

    for (int i = 0; i < V; i++) {
        int u = -1;
        for (int j = 0; j < V; j++) {
            if (!visited[j] && (u == -1 || dist[j] < dist[u]))
                u = j;
        }
        
        if (dist[u] == INF) break;
        visited[u] = 1;

        for (int j = 0; j < edgeCount[u]; j++) {
            Edge edge = edges[u][j];
            int v = edge.vt;
            int weight = edge.weights[dist[u] % N];

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
            }
        }
    }

    // Output the shortest path
    if (dist[end] == INF) {
        printf("No path found\n");
    } else {
        int *path = malloc(V * sizeof(int));
        int pathIndex = 0;
        for (int at = end; at != -1; at = prev[at]) {
            path[pathIndex++] = at;
        }
        for (int i = pathIndex - 1; i >= 0; i--) {
            printf("%d%s", path[i], i > 0 ? " " : "\n");
        }
        free(path);
    }

    free(dist);
    free(prev);
    free(visited);
}


// Read graph from file
void readGraph(const char *filename, int *V, int *N, int ***adjList, Edge ***edges, int **edgeCount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", V, N);

    *adjList = malloc(*V * sizeof(int *));
    *edges = malloc(*V * sizeof(Edge *));
    *edgeCount = calloc(*V, sizeof(int));

    for (int i = 0; i < *V; i++) {
        (*adjList)[i] = NULL;
        (*edges)[i] = NULL;
    }

    int vs, vt;
    while (fscanf(file, "%d %d", &vs, &vt) != EOF) {
        Edge edge;
        edge.vs = vs;
        edge.vt = vt;
        edge.weights = malloc(*N * sizeof(int));
        for (int i = 0; i < *N; i++) {
            fscanf(file, "%d", &edge.weights[i]);
        }

        (*edgeCount)[vs]++;
        (*edges)[vs] = realloc((*edges)[vs], (*edgeCount)[vs] * sizeof(Edge));
        (*edges)[vs][(*edgeCount)[vs] - 1] = edge;
    }

    fclose(file);
}

// Free memory allocated for the graph
void freeGraph(int V, int **adjList, Edge **edges, int *edgeCount) {
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < edgeCount[i]; j++) {
            free(edges[i][j].weights);
        }
        free(edges[i]);
    }
    free(adjList);
    free(edges);
    free(edgeCount);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <graph_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int V, N;
    int **adjList, *edgeCount;
    Edge **edges;

    readGraph(argv[1], &V, &N, &adjList, &edges, &edgeCount);

    int start, end;
    while (scanf("%d %d", &start, &end) == 2) {
        dijkstra(V, N, adjList, edges, edgeCount, start, end);
    }

    freeGraph(V, adjList, edges, edgeCount);
    return 0;
}
