/*
 * lab6.c
 * Lab 6: Dijkstra's Algorithm
 * David Lynge
 * dlynge
 * ECE 2230, Fall 2018
 * 
 * PURPOSE
 * This file contains the functions necessary to manage graphs and computer shortest paths.
 * 
 * ASSUMPTIONS
 * We assume that the weights in the graph will be significantly lower than FLT_MAX
 * 
 * BUGS
 * None known
 */
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "graph.h"

extern int verbose;

// Initializes a graph with 'size' nodes and returns a pointer to it
Graph *graph_construct(int size)
{
    if (size < 1)
        return NULL;

    Graph *G;
    int i, j;

    G = (Graph *)malloc(sizeof(Graph));
    G->N = size;
    G->A = (double **)calloc(size, sizeof(double *));
    for (i = 0; i < size; i++)
    {
        G->A[i] = (double *)calloc(size, sizeof(double));
        for (j = 0; j < size; j++)
            G->A[i][j] = __FLT_MAX__;
        G->A[i][i] = 0;
    }

    return G;
}

// Frees up memory for 'G' and takes care of dangling pointers
void graph_destruct(Graph **G)
{
    if (G == NULL || *G == NULL)
        return;

    int i;

    for (i = 0; i < (*G)->N; i++)
        free((*G)->A[i]);
    free((*G)->A);
    free(*G);
    *G = NULL;
}


// Adds edge with 'weight', 'dest', and 'source'
void graph_add_edge(Graph *G, int source, int dest, double weight)
{
    if (G == NULL || source >= G->N || dest >= G->N || source < 0 || dest < 0)
        return;

    G->A[source][dest] = weight;
}


// Returns the lowest cost to each node and its predecessor from 'source' in 'G'
Costs graph_shortest_path(Graph G, int source)
{
    Costs C;                // Cost to each node from source
    int size = G.N;         // Number of nodes in the graph
    int countAdded = 1;     // Number of nodes confirmed
    int confirmed[size];    // Array of confirmed nodes
    double minLength;       // Minimum length from source to unconfirmed nodes
    int node;               // The current node being looped through    
    double newCost;         // Used when updating neighboring costs
    int i;

    // Initialization
    C.N = size;
    C.pathLength = (double *)calloc(size, sizeof(double));
    C.prevNode = (int *)calloc(size, sizeof(int));
    C.srcNode = source;

    for (i = 0; i < size; i++)
    {
        confirmed[i] = 0;
        C.pathLength[i] = G.A[source][i];
        C.prevNode[i] = source;
    }
    confirmed[source] = 1;
    C.pathLength[source] = 0;
    C.prevNode[source] = -1;

    if (verbose)
        printf("Starting at node %d\n", source);
    
    // Loop through every node
    while (countAdded != size)
    {
        // Find closest unconfirmed node
        minLength = __DBL_MAX__;
        for (i = 0; i < size; i++)
            if (!confirmed[i] && C.pathLength[i] < minLength)
            {
                minLength = C.pathLength[i];
                node = i;
            }
        
        // Confirm the closest node
        confirmed[node] = 1;
        countAdded++;
        if (verbose)
        {
            if (minLength == __FLT_MAX__)
                printf(" %d added with cost inf\n", node);
            else
                printf(" %d added with cost %g\n", node, minLength);
        }

        // Update the cost matrix to neighboring nodes
        for (i = 0; i < size; i++)
            if (!confirmed[i] && G.A[node][i] < __FLT_MAX__)
            {
                newCost = C.pathLength[node] + G.A[node][i];
                if (newCost < C.pathLength[i])
                {
                    C.pathLength[i] = newCost;
                    C.prevNode[i] = node;
                    if (verbose)
                    {
                        if (newCost == __FLT_MAX__)
                            printf("\t%d has lower cost inf with predecessor %d\n", i, node);
                        else
                            printf("\t%d has lower cost %g with predecessor %d\n", i, newCost, node);
                    }
                }
            }

    }
    if (verbose)
        printf("Found %d nodes, including the source\n", countAdded);
    return C;
}

// Free the matricies within the cost struct
void graph_free_costs(Costs *C)
{
    free(C->pathLength);
    C->pathLength = NULL;
    free(C->prevNode);
    C->prevNode = NULL;
}

// Recursive part to print a path
void graph_print_path_rec(Costs C, int node)
{
    // Base case
    if (node == C.srcNode) 
    {
        printf("%d", node);
        return;
    }

    graph_print_path_rec(C, C.prevNode[node]);
    printf(" -- %d", node);
}

// Print the path to 'dstNode' given the costs 'C'
void graph_print_path(Costs C, int dstNode)
{
    // No path
    if (C.pathLength[dstNode] >= __FLT_MAX__)
        printf("\nThere is no path from %d to %d\n", C.srcNode, dstNode);
    else
    {
        printf("\nThe cost of the path from %d to %d is %g\n", C.srcNode, dstNode, C.pathLength[dstNode]);
        graph_print_path_rec(C, dstNode);
        printf("\n");
    }
}

// Print the network diameter for graph 'G'
void graph_print_diameter(Graph G)
{
    Costs C, maxC;
    Costs OldC[G.N];
    int maxDstNode;
    double maxLength = 0;
    int i, j;
    int isConnected = 1;

    for (i = 0; i < G.N; i++)
    {
        C = graph_shortest_path(G, i);
        OldC[i] = C;
        for (j = 0; j < G.N; j++)
            if (C.pathLength[j] != __FLT_MAX__ && C.pathLength[j] > maxLength)
            {
                maxC = C;
                maxLength = C.pathLength[j];
                maxDstNode = j;
            }
            else if (C.pathLength[j] == __FLT_MAX__)
                isConnected = 0;
    }

    if (!isConnected)
        printf("\nThe graph is not connected.\n");
    else
    {
        printf("\nThe graph is connected and the network diamter is %g from nodes %d to %d\n",
            maxLength, maxC.srcNode, maxDstNode);

        graph_print_path_rec(maxC, maxDstNode);
        printf("\n");
    }

    for (i = 0; i < G.N; i++)
        graph_free_costs(&OldC[i]);
}

// Removes a path from the source defined in 'C' to destination 'node'
void graph_remove_path(Graph *G, Costs C, int node)
{
    // Base case
    if (node == C.srcNode) 
        return;

    graph_remove_path(G, C, C.prevNode[node]);
    G->A[C.prevNode[node]][node] = __FLT_MAX__;
}

// Recursive part to print disjoint paths
void graph_print_disjoint_rec(Graph *G, int src, int dst)
{
    Costs C = graph_shortest_path(*G, src);

    if (C.pathLength[dst] >= __FLT_MAX__)
    {
        graph_free_costs(&C);
        return;
    }

    graph_print_path(C, dst);

    // Remove path
    graph_remove_path(G, C, dst);
    printf("Path removed from graph\n");
    graph_free_costs(&C);

    graph_print_disjoint_rec(G, src, dst);
}

// Print multiple link-disjoint paths for graph 'G' from 'src' to 'dst'
void graph_print_disjoint(Graph G, int src, int dst)
{
    printf("\nLink-disjoint paths from %d to %d:\n", src, dst);
    graph_print_disjoint_rec(&G, src, dst);
    printf("\nNo more paths\n");
}

// Assign weights for graph 'G' of type 2
void graph_strong_weights(Graph *G)
{
    int i, j;

    for (i = 0; i < G->N; i++)
            for (j = 0; j < G->N; j++)
                if (i != j)
                    G->A[i][j] = abs(i - j) + (i - j + 2) * (i - j + 2) + 3 * j;
                else
                    G->A[i][j] = 0;
}

// Assign weights for graph 'G' of type 3
void graph_random_weights(Graph *G, int R)
{
    Pos v[G->N];
    int i, j;
    double C = sqrt(R / (M_PI * G->N));
    double M = log10(1 + (1 / C) * (1 / C));
    double D, F;
    int max, min;
    int adj;
    double avg;

    for (i = 0; i < G->N; i++)
    {
        v[i].x = drand48();
        v[i].y = drand48();
    }

    v[0].x = 0.0;
    v[0].y = 0.5;
    v[G->N - 1].x = 1.0;
    v[G->N - 1].x = 0.5;
    
    // Calculate weights
    for (i = 0; i < G->N; i++)
        for (j = 0; j < G->N; j++)
        {
            D = sqrt(pow(v[i].x - v[j].x, 2) + pow(v[i].y - v[j].y, 2));

            if (i == j)
                G->A[i][j] = 0;
            else if (D <= C)
            {
                F = log10(1 + pow(1 / (D + C / 1000), 2));
                G->A[i][j] = M / F;
            }
            else
                G->A[i][j] = __FLT_MAX__;
        }
        
    // Print stats (max, min, average adjacent verticies)
    max = 0;
    min = INT_MAX;
    avg = 0;

    for (i = 0; i < G->N; i++)
    {
        adj = 0;
        for (j = 0; j < G->N; j++)
            if (G->A[i][j] > 0 && G->A[i][j] != __FLT_MAX__)
                adj++;

        if (adj > max)
            max = adj;
        else if (adj < min)
            min = adj;
        
        avg += adj;
    }

    avg /= G->N;

    printf("\nAvg neighbors: %g\nMax neighbors: %d\nMin neighbors: %d\n",
     avg, max, min);

}


// Collection of asserts to verify that a graph 'G' is properly configured
void graph_dbg_validate(Graph G)
{
    assert(G.A != NULL);
    assert(G.N > 0);
    
    int i, j;

    for (i = 0; i < G.N; i++)
    {
        assert(G.A[i] != NULL);
        for (j = 0; j < G.N; j++)
            if (i == j)
                assert(G.A[i][j] == 0);
            else
                assert(G.A[i][j] >= 0);
    }
}

// Prints the adjacency matrix for a graph 'G' 
void graph_dbg_print(Graph G)
{
    graph_dbg_validate(G);

    int i, j;

    printf("\nPrinting %dx%d Adjacency Matrix...\n", G.N, G.N);
    for (i = 0; i < G.N; i++)
    {
        printf("%3d : \t", i);
        for (j = 0; j < G.N; j++)
        {
            if (G.A[i][j] == __FLT_MAX__)
                printf("inf ");
            else
                printf("%3.0f ", G.A[i][j]);
        }
        printf("\n");
    }

}