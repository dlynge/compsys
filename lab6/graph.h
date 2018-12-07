/*
 * graph.h
 * Lab 6: Dijkstra's Algorithm
 * David Lynge
 * dlynge
 * ECE 2230, Fall 2018
 * 
 * PURPOSE
 * This file contains the public function prototypes and definitions required for graph.c
 */

// ----- Definitions -----

typedef struct 
{
    int N;          // Number of nodes in the graph
    double **A;     // Adjacency matrix
} Graph;

typedef struct
{
    int N;                  // Number of nodes in the graph
    double *pathLength;     // Array of path lengths to each node
    int *prevNode;          // Array of predecessors to each node
    int srcNode;            // The source of the path
} Costs;

typedef struct
{
    double x, y;
} Pos;

// ----- Prototypes -----

// Initializes a graph with 'size' nodes and returns a pointer to it
Graph *graph_construct(int size);

// Frees up memory for 'G' and takes care of dangling pointers
void graph_destruct(Graph **G);

// Adds edge with 'weight', 'dest', and 'source'
void graph_add_edge(Graph *G, int source, int dest, double weight);

// Returns the lowest cost to each node and its predecessor from 'source' in 'G'
Costs graph_shortest_path(Graph G, int source);

// Free the matricies within the cost struct
void graph_free_costs(Costs *C);

// Print the path to 'dstNode' given the costs 'C'
void graph_print_path(Costs C, int dst_node);

// Print the network diameter for graph 'G'
void graph_print_diameter(Graph G);

// Print multiple link-disjoint paths
void graph_print_disjoint(Graph G, int src, int dst);

// Assign weights for graph type 2
void graph_strong_weights(Graph *G);

// Assign weights for graph type 3
void graph_random_weights(Graph *G, int R);

// ----- Debugging -----

// Collection of asserts to verify that a graph 'G' is properly configured
void graph_dbg_validate(Graph G);

// Prints the adjacency matrix for a graph 'G' 
void graph_dbg_print(Graph G);