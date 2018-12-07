/*
 * lab6.c
 * Lab 6: Dijkstra's Algorithm
 * David Lynge
 * dlynge
 * ECE 2230, Fall 2018
 * 
 * PURPOSE
 * This file contains the drivers to use and test the graph.c package for computing shortest paths.
 * 
 * OPTIONS
 *     -g [1|2|3] : Graph type
 *         1 - Use a pre-defined weakly connected directed graph. Options -n and -a are not used.
 *         2 - Use a strongly connected direct graph with N verticies. Use option -n to set N.
 *         3 - Use a randomly-generated graph with N (-n) verticies.
 *     -n N : Number of verticies
 *     -a R : Approximate number of adjacent verticies 0 < R < N
 *     -h [1|2|3] : Graph operation
 *         1 - Returns the shortest path
 *         2 - Returns the network diameter
 *         3 - Returns multiple link-disjoint paths
 *     -s S : Number of source vertex 0 <= S < N
 *     -d D : Number of the destination vertex 0 <= D < N
 *     -v : Enable verbose output
 *     -r 12345 : seed the random number generator
 * 
 * ASSUMPTIONS
 * We assume that the user inputs values that are reasonable for each option.
 * Also, we assume that the graph has at least one edge.
 * 
 * BUGS
 * None known
 */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include "graph.h"

int verbose = 0;

// structure for command line parameters 
typedef struct {
    int seed;
    int graphType;
    int nodes;
    int adjNodes;
    int graphOp;
    int srcNode;
    int dstNode;
} driverParams;

void getCommandLine(int argc, char **argv, driverParams *ep);

int main(int argc, char **argv)
{
    // Determine the command line arguments
    driverParams ep;
    getCommandLine(argc, argv, &ep);

    srand48(ep.seed);

    Graph *G;
    Costs C;

    clock_t start, end;

    // Communicate the command line arguments
    printf("\n\n------------------------------\n");
    printf("Seed: %d\n", ep.seed);
    printf("Graph type: %d\n", ep.graphType);
    printf("Graph operation: %d\n", ep.graphOp);
    printf("------------------------------\n");

    // Graph type
    if (ep.graphType == 1)
    {
        G = graph_construct(7);
        graph_add_edge(G,1,2,3);
        graph_add_edge(G,1,6,5);
        graph_add_edge(G,2,3,7);
        graph_add_edge(G,2,6,10);
        graph_add_edge(G,2,4,3);
        graph_add_edge(G,3,5,1);
        graph_add_edge(G,3,4,5);
        graph_add_edge(G,4,5,6);
        graph_add_edge(G,5,6,7);
        graph_add_edge(G,6,0,4);
        graph_add_edge(G,6,3,8);
        graph_add_edge(G,6,4,9);
    }
    else if (ep.graphType == 2)
    {
        if (ep.nodes < 1)
        {
            printf("\nError in creating graph with %d nodes\n", ep.nodes);
            exit(1);
        }

        G = graph_construct(ep.nodes);
    
        graph_strong_weights(G);
    }
    else if (ep.graphType == 3)
    {
        if (ep.nodes < 1 || ep.adjNodes < 1 || ep.adjNodes >= ep.nodes)
        {
            printf("\nError in creating graph with %d nodes and %d adjacent nodes\n", 
             ep.nodes, ep.adjNodes);
            exit(1);
        }

        G = graph_construct(ep.nodes);
        
        graph_random_weights(G, ep.adjNodes);
    }
    else
    {
        printf("\nError in determining graph type\n");
        exit(1);
    }

    if (verbose)
        graph_dbg_print(*G);

    // Graph operation
    start = clock();
    if (ep.graphOp == 1)    // Shortest path
    {
        if (ep.dstNode < 0 || ep.srcNode < 0 || ep.dstNode >= G->N || ep.srcNode >= G->N)
        {
            printf("\nError in determining source and destination node\n");
            graph_destruct(&G);
            exit(1);
        }

        C = graph_shortest_path(*G, ep.srcNode);
        graph_print_path(C, ep.dstNode);
        graph_free_costs(&C);
    }
    else if (ep.graphOp == 2)   // Network diameter
    {
        graph_print_diameter(*G);
        
    }
    else if (ep.graphOp == 3)
    {
        if (ep.dstNode < 0 || ep.srcNode < 0 || ep.dstNode >= G->N || ep.srcNode >= G->N)
        {
            printf("\nError in determining source and destination node\n");
            graph_destruct(&G);
            exit(1);
        }
        graph_print_disjoint(*G, ep.srcNode, ep.dstNode);
    }
    else
    {
        printf("\nError in determining graph operation\n");
        graph_destruct(&G);
        exit(1);
    }
    end = clock();

    printf("Operation time: %g\n", 1000 * (double)(end - start) / CLOCKS_PER_SEC);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    
    graph_dbg_validate(*G);

    graph_destruct(&G);

    return 0;
}

// Read in command line arguments 'argv'/'argc' and parse them into 'ep'
void getCommandLine(int argc, char **argv, driverParams *ep)
{
    /* The geopt function creates three global variables:
     *    optopt--if an unknown option character is found
     *    optind--index of next element in argv 
     *    optarg--argument for option that requires argument 
     *
     * The third argument to getopt() specifies the possible argument flags
     *   If an argument flag is followed by a colon, then an argument is 
     *   expected. E.g., "x:y" means -x must have an argument but not -y
     */
    int c;
    int index;

    ep->seed = 1765640879;
    ep->graphType = 1;
    ep->nodes = -1;
    ep->adjNodes = -1;
    ep->graphOp = 1;
    ep->srcNode = -1;
    ep->dstNode = -1;

    while ((c = getopt(argc, argv, "g:n:a:h:s:d:r:v")) != -1)
        switch(c) {
            case 'g': ep->graphType = atoi(optarg);     break;
            case 'n': ep->nodes = atoi(optarg);         break;
            case 'a': ep->adjNodes = atoi(optarg);      break;
            case 'h': ep->graphOp = atoi(optarg);       break;
            case 's': ep->srcNode = atoi(optarg);       break;
            case 'd': ep->dstNode = atoi(optarg);       break;
            case 'v': verbose = 1;                      break;
            case 'r': ep->seed = atoi(optarg);          break;
            case '?':
                  if (isprint(optopt))
                      fprintf(stderr, "Unknown option %c.\n", optopt);
                  else
                      fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                  printf("Lab6 command line options\n");
                  printf("General options ---------\n");
                  printf("\t-g [1|2|3] : Graph type\n");
                  printf("\t    1 - Use a pre-defined weakly connected directed graph. Options -n and -a are not used.\n");
                  printf("\t    2 - Use a strongly connected direct graph with N verticies. Use option -n to set N.\n");
                  printf("\t    3 - Use a randomly-generated graph with N (-n) verticies.\n");
                  printf("\t-n N : Number of verticies\n");
                  printf("\t-a R : Approximate number of adjacent verticies 0 < R < N\n");
                  printf("\t-h [1|2|3] : Graph operation\n");
                  printf("\t    1 - Returns the shortest path\n");
                  printf("\t    2 - Returns the network diameter\n");
                  printf("\t    3 - Returns multiple link-disjoint paths\n");
                  printf("\t-s S : Number of source vertex 0 <= S < N\n");
                  printf("\t-d D : Number of the destination vertex 0 <= D < N\n");
                  printf("\t-v : Enable verbose output\n");
                  printf("\t-r 12345 : seed the random number generator\n");
                  exit(1);
        }
    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
}