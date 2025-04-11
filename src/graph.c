#include "graph.h"

// Create new graph instance.
// Returns NULL on error.
Graph* NewGraph(size_t num_vertices) {
    Graph* graph = malloc(sizeof(Graph));
    if (!graph) {
        errno = ENOMEM;
        return NULL;
    }

    graph->matrix_ = malloc(sizeof(int) * ( num_vertices * num_vertices ));
    if (!graph->matrix_) {
        free(graph);
        errno = ENOMEM;
        return NULL;
    }

    for (int i = 0; i < ( num_vertices * num_vertices ); ++i) {
        graph->matrix_[i] = 0;
    }

    graph->num_vertices_ = num_vertices;
    return graph;
}

// Free graph instance.
// Ignores NULL instance and gields.
void FreeGraph(Graph* graph) {
    if (graph == NULL) {
        return;
    }

    free(graph->matrix_);
    free(graph);
}

// Get number of vertices in a graph.
size_t GetGraphSize(const Graph* graph) {
    if (graph == NULL) {
        errno = EINVAL;
        return 0;
    }

    return graph->num_vertices_;
}

// Add directed edge to a graph.
// Loops and multiple edges are allowed.
// Column number - from where, row number - destination
bool AddDirectedEdge(Graph* graph, size_t from, size_t to) {
    if (graph == NULL) {
        errno = EINVAL;
        return false;
    }

    if ((from < 0 || GetGraphSize(graph) - 1 < from) || 
        (to < 0 || GetGraphSize(graph) - 1 < to)) {
        errno = ERANGE;
        return false;
    }

    graph->matrix_[ to * GetGraphSize(graph) + from] = 1;

    return true;
}

// Get list of successor vertices (immediately reachable from the provided one).
IntVector* GetSuccessors(const Graph* graph, size_t vertex) {
    if (graph == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (vertex < 0 || GetGraphSize(graph) - 1 < vertex) {
        errno = ERANGE;
        return NULL;
    }

    IntVector* res = NewIntVector(1);
    bool status;

    for (int i = 0; i < GetGraphSize(graph); ++i) {
        if (graph->matrix_[ i * GetGraphSize(graph) + vertex ] == 1) {
            status = AppendToIntVector(res, i);
            if (!status) {
                FreeIntVector(res);
                return NULL;
            }
        }
    }

    return res;
}

bool VertexHasSuccessors(const Graph* graph, size_t vertex) {
    if (graph == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (vertex < 0 || GetGraphSize(graph) - 1 < vertex) {
        errno = ERANGE;
        return NULL;
    }

    for (int i = 0; i < GetGraphSize(graph); ++i) {
        if (graph->matrix_[ i * GetGraphSize(graph) + vertex ] == 1) {
            return true;
        }
    }

    return false;
}

// Check whether a graph is acyclic.
// Returns false and sets errno variable on error.
// Can be implemented via either DFS of BFS.

static bool DfsForCycleChecking(const Graph* graph, int cur_vertex, IntMap* visited) {
    int value;

    if (GetIntMapValue(visited, cur_vertex, &value)) {
        if (value == 1) { // vertex is in current path => cycle
            return false;
        } else if (value == 2) { // vertex is processed => no cycle
            return true;
        }
    }

    int status;
    status = SetIntMapValue(visited, cur_vertex, 1, true);
    if (!status) {
        return false;
    }

    IntVector* successors = GetSuccessors(graph, cur_vertex);
    if (!successors) {
        return false;
    }

    int next_vertex;
    for (int i = 0; i < GetIntVectorLength(successors); ++i) {
        next_vertex = GetIntVectorElement(successors, i);
        
        if (!DfsForCycleChecking(graph, next_vertex, visited)) {
            FreeIntVector(successors);
            return false;
        }
    }

    status = SetIntMapValue(visited, cur_vertex, 2, true);
    if (!status) {
        return false;
    }

    FreeIntVector(successors);
    return true;
}

bool IsAcyclic(const Graph* graph) {
    if (graph == NULL) {
        errno = EINVAL;
        return false;
    }

    if (GetGraphSize(graph) == 0) {
        return true;
    }

    IntMap* visited = NewIntMap(graph->num_vertices_);
    if (visited == NULL) {
        return false;
    }

    for (int vertex = 0; vertex < GetGraphSize(graph); ++vertex) {
        if (!DfsForCycleChecking(graph, vertex, visited)) {
            FreeIntMap(visited);
            return false;
        }
    }

    FreeIntMap(visited);
    return true;
}

// Goes up the graph starting from a certain vertex, 
// changing the values in the cells of the graph by the specified number. 
// It is needed to track errors when completing tasks.
// On cyclic graph goes doesnt work
static void ChangeEdgeNumberHelp(const Graph* graph, size_t vertex, int n, int count) {
    if (count > GetGraphSize(graph)) {
        return;
    }

    for (int i = 0; i < GetGraphSize(graph); ++i) {
        if (graph->matrix_[ vertex * GetGraphSize(graph) + i ] == 1) {
            graph->matrix_[ vertex * GetGraphSize(graph) + i ] = n;
            ChangeEdgeNumberHelp(graph, i, n, count + 1);
        }
    }
}

void ChangeEdgeNumber(const Graph* graph, size_t vertex, int n) {
    if (graph == NULL) {
        errno = EINVAL;
        return;
    }

    if (vertex < 0 || GetGraphSize(graph) - 1 < vertex) {
        errno = ERANGE;
        return;
    }

    ChangeEdgeNumberHelp(graph, vertex, n, 0);
}


// static bool DfsForTopologicalSort(const Graph* graph, int cur_vertex, IntMap* visited, IntVector* result) {
//     int status;
//     status = SetIntMapValue(visited, cur_vertex, 1, true);
//     if (!status) {
//         return false;
//     }

//     int next_vertex;
//     int value;
//     const IntVector* successors = GetSuccessors(graph, cur_vertex);

//     for (int i = 0; i < GetIntVectorLength(successors); ++i) {
//         next_vertex = GetIntVectorElement(successors, i);
        
//         if (!GetIntMapValue(visited, next_vertex, &value)) {
//             status = DfsForTopologicalSort(graph, next_vertex, visited, result);
//             if (!status) {
//                 return false;
//             }
//         }
//     }

//     status = AppendToIntVector(result, cur_vertex);
//     if (!status) {
//         return false;
//     }

//     return true;
// }

// IntVector* TopologicalSort(const Graph* graph) {
//     if (graph == NULL) {
//         errno = EINVAL;
//         return NULL;
//     }

//     IntVector* result = NewIntVector(GetGraphSize(graph));
//     if (!result) {
//         return NULL;
//     }

//     IntMap* visited = NewIntMap(graph->num_vertices_);
//     if (visited == NULL) {
//         FreeIntVector(result);
//         return NULL;
//     }

//     int value;
//     for (int vertex = 0; vertex < GetGraphSize(graph); ++vertex) {
//         if (!GetIntMapValue(visited, vertex, &value)) {
//             if (!DfsForTopologicalSort(graph, vertex, visited, result)) {
//                 FreeIntMap(visited);
//                 FreeIntVector(result);
//                 return NULL;
//             }
//         }
//     }

//     ReverseIntVector(result);

//     FreeIntMap(visited);
//     return result;
// }
