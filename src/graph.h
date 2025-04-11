#pragma once

#include "vector.h"
#include "queue.h"
#include "map.h"
#include "utils.h"

typedef struct Graph {
    int* matrix_;
    size_t num_vertices_;
} Graph;

// Create new graph instance.
// Returns NULL on error.
Graph* NewGraph(size_t num_vertices);

// Free graph instance.
// Ignores NULL instance and gields.
void FreeGraph(Graph* graph);

// Get number of vertices in a graph.
size_t GetGraphSize(const Graph* graph);

// Add directed edge to a graph.
// Loops and multiple edges are allowed.
bool AddDirectedEdge(Graph* graph, size_t from, size_t to);

// Get list of successor vertices (immediately reachable from the provided one).
IntVector* GetSuccessors(const Graph* graph, size_t vertex);

// Check whether a graph is acyclics.
// Returns false and sets errno variable on error.
// Can be implemented via either DFS of BFS.
bool IsAcyclic(const Graph* graph);

// Checking, if vertex has successors
bool VertexHasSuccessors(const Graph* graph, size_t vertex);

void ChangeEdgeNumber(const Graph* graph, size_t vertex, int n);

// Topological sort
// IntVector* TopologicalSort(const Graph* graph);
