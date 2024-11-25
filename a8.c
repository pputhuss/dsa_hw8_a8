#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_VERTICES 1000
#define MAX_PERIOD 100

typedef struct {
    int dest;
    int* weights;
} Edge;

typedef struct {
    int vertices;
    int period;
    Edge* graph[MAX_VERTICES];
    int graph_size[MAX_VERTICES];
} PeriodicGraph;

typedef struct {
    int vertex;
    int cost;
    int step;
    int* path;
    int path_length;
} State;

// Heap implementation for priority queue
typedef struct {
    State* array;
    int size;
    int capacity;
} PriorityQueue;

// Utility comparison function for priority queue
int compare_states(State a, State b) {
    return a.cost > b.cost;
}

// Heap operations
void swap(State* a, State* b) {
    State temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(PriorityQueue* pq, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < pq->size && compare_states(pq->array[smallest], pq->array[left]))
        smallest = left;

    if (right < pq->size && compare_states(pq->array[smallest], pq->array[right]))
        smallest = right;

    if (smallest != index) {
        swap(&pq->array[index], &pq->array[smallest]);
        heapify(pq, smallest);
    }
}

void push(PriorityQueue* pq, State state) {
    if (pq->size == pq->capacity) return;
    
    pq->array[pq->size] = state;
    int current = pq->size;
    pq->size++;

    while (current > 0 && compare_states(pq->array[(current-1)/2], pq->array[current])) {
        swap(&pq->array[current], &pq->array[(current-1)/2]);
        current = (current-1)/2;
    }
}

State pop(PriorityQueue* pq) {
    if (pq->size <= 0) {
        State empty = {0};
        return empty;
    }

    State root = pq->array[0];
    pq->array[0] = pq->array[pq->size - 1];
    pq->size--;
    heapify(pq, 0);

    return root;
}

int* find_shortest_path(PeriodicGraph* graph, int start, int end) {
    int dist[MAX_VERTICES][MAX_PERIOD][MAX_PERIOD];
    memset(dist, 0x3f, sizeof(dist));

    PriorityQueue pq;
    pq.array = malloc(MAX_VERTICES * MAX_PERIOD * sizeof(State));
    pq.size = 0;
    pq.capacity = MAX_VERTICES * MAX_PERIOD;

    int* initial_path = malloc(sizeof(int));
    initial_path[0] = start;

    State initial_state = {start, 0, 0, initial_path, 1};
    push(&pq, initial_state);
    dist[start][0][0] = 0;

    while (pq.size > 0) {
        State current = pop(&pq);

        if (current.vertex == end) {
            return current.path;
        }

        for (int i = 0; i < graph->graph_size[current.vertex]; i++) {
            Edge* edge = &graph->graph[current.vertex][i];
            int next = edge->dest;
            int new_step = (current.step + 1) % graph->period;
            int edge_cost = edge->weights[current.step];
            int new_cost = current.cost + edge_cost;

            if (new_cost < dist[next][new_step][new_step]) {
                dist[next][new_step][new_step] = new_cost;

                int* new_path = malloc((current.path_length + 1) * sizeof(int));
                memcpy(new_path, current.path, current.path_length * sizeof(int));
                new_path[current.path_length] = next;

                State new_state = {
                    next, 
                    new_cost, 
                    new_step, 
                    new_path, 
                    current.path_length + 1
                };
                push(&pq, new_state);

                free(current.path);
            }
        }
    }

    return NULL;
}

void read_graph(PeriodicGraph* graph, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        exit(1);
    }

    fscanf(file, "%d %d", &graph->vertices, &graph->period);

    int source, dest;
    for (int i = 0; i < graph->vertices; i++) {
        graph->graph_size[i] = 0;
        graph->graph[i] = malloc(graph->vertices * sizeof(Edge));
    }

    while (fscanf(file, "%d %d", &source, &dest) == 2) {
        Edge* edge = &graph->graph[source][graph->graph_size[source]];
        edge->dest = dest;
        edge->weights = malloc(graph->period * sizeof(int));
        
        for (int i = 0; i < graph->period; i++) {
            fscanf(file, "%d", &edge->weights[i]);
        }
        graph->graph_size[source]++;
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <graph_file>\n", argv[0]);
        return 1;
    }

    PeriodicGraph graph;
    read_graph(&graph, argv[1]);

    int start, end;
    while (scanf("%d %d", &start, &end) == 2) {
        int* path = find_shortest_path(&graph, start, end);
        
        if (path) {
            int length = 0;
            while (path[length] != end) length++;
            length++;

            for (int i = 0; i < length; i++) {
                printf("%d", path[i]);
                if (i < length - 1) printf(" ");
            }
            printf("\n");
            
            free(path);
        }
    }

    // Free allocated memory
    for (int i = 0; i < graph.vertices; i++) {
        for (int j = 0; j < graph.graph_size[i]; j++) {
            free(graph.graph[i][j].weights);
        }
        free(graph.graph[i]);
    }

    return 0;
}
