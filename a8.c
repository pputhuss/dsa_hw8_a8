#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAXV 1000    // Maximum number of vertices
#define MAXN 100     // Maximum period N
#define INF INT_MAX

typedef struct {
    int to;
    int weights[MAXN];
} Edge;

typedef struct {
    Edge *edges;
    int edge_count;
} Vertex;

typedef struct {
    int node;
    int cost;
    int steps;
} PriorityQueueNode;

typedef struct {
    PriorityQueueNode *data;
    int size;
    int capacity;
} PriorityQueue;

// Global variables
Vertex graph[MAXV];
int V, N;

// Distance and predecessor tracking
int dist[MAXV][MAXN];
int prev_node[MAXV][MAXN];
int prev_time[MAXV][MAXN];

// Priority Queue Functions
PriorityQueue* create_priority_queue(int capacity) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    pq->data = malloc(capacity * sizeof(PriorityQueueNode));
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

void free_priority_queue(PriorityQueue *pq) {
    free(pq->data);
    free(pq);
}

void swap(PriorityQueueNode *a, PriorityQueueNode *b) {
    PriorityQueueNode temp = *a;
    *a = *b;
    *b = temp;
}

void push(PriorityQueue *pq, int node, int cost, int steps) {
    if (pq->size == pq->capacity) {
        pq->capacity *= 2;
        pq->data = realloc(pq->data, pq->capacity * sizeof(PriorityQueueNode));
    }

    pq->data[pq->size++] = (PriorityQueueNode){node, cost, steps};
    int i = pq->size - 1;

    // Bubble up
    while (i > 0 && pq->data[i].cost < pq->data[(i - 1) / 2].cost) {
        swap(&pq->data[i], &pq->data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

PriorityQueueNode pop(PriorityQueue *pq) {
    PriorityQueueNode top = pq->data[0];
    pq->data[0] = pq->data[--pq->size];

    // Bubble down
    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < pq->size && pq->data[left].cost < pq->data[smallest].cost)
            smallest = left;
        if (right < pq->size && pq->data[right].cost < pq->data[smallest].cost)
            smallest = right;

        if (smallest == i)
            break;

        swap(&pq->data[i], &pq->data[smallest]);
        i = smallest;
    }

    return top;
}

// Dijkstra's Algorithm
void dijkstra(int start) {
    PriorityQueue *pq = create_priority_queue(1000);

    for (int i = 0; i < V; i++) {
        for (int t = 0; t < N; t++) {
            dist[i][t] = INF;
            prev_node[i][t] = -1;
            prev_time[i][t] = -1;
        }
    }

    dist[start][0] = 0;
    push(pq, start, 0, 0);

    while (pq->size > 0) {
        PriorityQueueNode current = pop(pq);
        int u = current.node;
        int cost = current.cost;
        int steps = current.steps;
        int time_mod_N = steps % N;

        if (cost > dist[u][time_mod_N]) continue;

        for (int i = 0; i < graph[u].edge_count; i++) {
            Edge *edge = &graph[u].edges[i];
            int v = edge->to;
            int next_steps = steps + 1;
            int next_time_mod_N = next_steps % N;
            int weight = edge->weights[time_mod_N];
            int new_cost = cost + weight;

            if (new_cost < dist[v][next_time_mod_N]) {
                dist[v][next_time_mod_N] = new_cost;
                prev_node[v][next_time_mod_N] = u;
                prev_time[v][next_time_mod_N] = time_mod_N;
                push(pq, v, new_cost, next_steps);
            }
        }
    }

    free_priority_queue(pq);
}

// Reconstruct the shortest path
void reconstruct_path(int start, int end) {
    int min_dist = INF, min_time = -1;

    for (int t = 0; t < N; t++) {
        if (dist[end][t] < min_dist) {
            min_dist = dist[end][t];
            min_time = t;
        }
    }

    if (min_dist == INF) {
        printf("-1\n");
        return;
    }

    int path[MAXV];
    int path_length = 0;
    int u = end, t = min_time;

    while (u != -1) {
        path[path_length++] = u;
        int temp_u = prev_node[u][t];
        int temp_t = prev_time[u][t];
        u = temp_u;
        t = temp_t;
    }

    for (int i = path_length - 1; i >= 0; i--) {
        printf("%d%s", path[i], i > 0 ? " " : "\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        return 1;
    }

    fscanf(file, "%d %d", &V, &N);

    for (int i = 0; i < V; i++) {
        graph[i].edge_count = 0;
        graph[i].edges = NULL;
    }

    int vs, vt;
    while (fscanf(file, "%d %d", &vs, &vt) == 2) {
        Edge edge;
        edge.to = vt;
        for (int i = 0; i < N; i++) {
            fscanf(file, "%d", &edge.weights[i]);
        }

        graph[vs].edge_count++;
        graph[vs].edges = realloc(graph[vs].edges, graph[vs].edge_count * sizeof(Edge));
        graph[vs].edges[graph[vs].edge_count - 1] = edge;
    }

    fclose(file);

    int current_start = -1;
    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        int start, end;
        if (sscanf(line, "%d %d", &start, &end) != 2) continue;

        if (start != current_start) {
            current_start = start;
            dijkstra(start);
        }

        reconstruct_path(start, end);
    }

    for (int i = 0; i < V; i++) {
        free(graph[i].edges);
    }

    return 0;
}
