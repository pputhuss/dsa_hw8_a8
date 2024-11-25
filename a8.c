#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define INF INT_MAX //def infinity as max int val

//structure to represent edge with periodic weights
typedef struct {
    int vs, vt; //source and target vertices
    int *weights; //periodic weights in array form that change
} Edge;

//dijkstra's algorithm implementation for finding shortest path with periodic weights
void dijkstra(int V, int N, int **adjList, Edge **edges, int *edgeCount, int start, int end) {
    //allocating mem for dist, prev, and visited
    int *dist = malloc(V * sizeof(int)); //shortest dist
    int *prev = malloc(V * sizeof(int)); //prev vertex
    int *visited = calloc(V, sizeof(int)); //tracks visited vertices
    //used calloc to initialize all elements at 0
    
    //initialize distances to inf and prev vertices to -1
    for (int i = 0; i < V; i++) 
    {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0; //dist to start vertex is 0

    for (int i = 0; i < V; i++) 
    {
        int u = -1; //finding unvisted vertex with min dist
        for (int j = 0; j < V; j++) 
        {
            if (!visited[j] && (u == -1 || dist[j] < dist[u]))
                u = j;
        }
        
        //if no path exists, break
        if (dist[u] == INF) break;
        visited[u] = 1; //mark visited for curr node

        //check all edges from current vertex
        for (int j = 0; j < edgeCount[u]; j++) 
        {
            Edge edge = edges[u][j];
            int v = edge.vt;
            //select weight using the formula from pdf
            int weight = edge.weights[dist[u] % N];

            //update shortest path if a shorter path is found
            if (dist[u] + weight < dist[v]) 
            {
                dist[v] = dist[u] + weight;
                prev[v] = u;
            }
        }
    }

    //output the shortest path
    if (dist[end] == INF) 
    {
        printf("No path found\n");
    } 

    else 
    {
        //reconstruct and print path
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

    //free allocated memory
    free(dist);
    free(prev);
    free(visited);
}


//function to read graph data from file
void readGraph(const char *filename, int *V, int *N, int ***adjList, Edge ***edges, int **edgeCount) {
    FILE *file = fopen(filename, "r");
    if (!file) 
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    //read number of vertices and period of weights
    fscanf(file, "%d %d", V, N);

    //allocate memory for graph structures
    *adjList = malloc(*V * sizeof(int *));
    *edges = malloc(*V * sizeof(Edge *));
    *edgeCount = calloc(*V, sizeof(int));

    //initialize adjacency list and edges
    for (int i = 0; i < *V; i++) {
        (*adjList)[i] = NULL;
        (*edges)[i] = NULL;
    }

    //read edges/weights listed
    int vs, vt;
    while (fscanf(file, "%d %d", &vs, &vt) != EOF) 
    {
        Edge edge;
        edge.vs = vs;
        edge.vt = vt;
        edge.weights = malloc(*N * sizeof(int));

        for (int i = 0; i < *N; i++) 
        {
            fscanf(file, "%d", &edge.weights[i]);
        }
        //add edge to graph
        (*edgeCount)[vs]++;
        (*edges)[vs] = realloc((*edges)[vs], (*edgeCount)[vs] * sizeof(Edge));
        (*edges)[vs][(*edgeCount)[vs] - 1] = edge;
    }

    fclose(file);
}

//free memory allocated for the graph
void freeGraph(int V, int **adjList, Edge **edges, int *edgeCount) {
    for (int i = 0; i < V; i++) 
    {
        //free weights for each edge
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
    //check for correct command-line args
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <graph_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //vairables to store graph info
    int V, N;
    int **adjList, *edgeCount;
    Edge **edges;

    //read graph from file
    readGraph(argv[1], &V, &N, &adjList, &edges, &edgeCount);

    //read and process multiple start-end vertex pairs
    int start, end;
    while (scanf("%d %d", &start, &end) == 2) 
    {
        dijkstra(V, N, adjList, edges, edgeCount, start, end);
    }

    //free allocated mem
    freeGraph(V, adjList, edges, edgeCount);
    return 0;
}
