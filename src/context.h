#pragma once

#include "config.h"
#include "graph.h"

typedef enum VerbosityType {
    VERBOSITY_TYPE_NONE,   // do not render task statuses
    VERBOSITY_TYPE_TABLE,  // render task statuses as a table with each row having format `Task #<idx> (<name>): <status> (<fail-reason>)`
    VERBOSITY_TYPE_GRAPH,  // unused
} VerbosityType;

typedef enum TaskStatus {
    TASK_STATUS_UNKNOWN,  // task hasn't been queued yet or its finish status wasn't detected properly
    TASK_STATUS_QUEUED,   // task is ready to start but awaits in a queue
    TASK_STATUS_RUNNING,  // task is currently running
    TASK_STATUS_SUCCESS,  // task has successfully finished with exit status 0
    TASK_STATUS_FAILED,   // task has failed due to non-zero exit status or any signal
} TaskStatus;

typedef struct TaskInfo {
    TaskStatus task_status;  // high level task status
    int worker_status;       // detailed worker status
} TaskInfo;

typedef struct Context {
    TaskInfo* tasks;

    const ExecutionConfig* config;  // for additional task info, such as name
    const Graph* dependency_graph;  // for VERBOSITY_TYPE_GRAPH, currently unused
} Context;


// Create new context instance.
// Returns NULL on error.
Context* NewContext(const Graph* dependency_graph, const ExecutionConfig* config);

// Free context instance.
// Ignores NULL instance and fields.
void FreeContext(Context* context);

// Render task statuses to stdout.
// Clears previous rendering if redraw is true.
void DrawContext(const Context* context, VerbosityType verbosity_type, bool redraw);
