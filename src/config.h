#pragma once

#include <stdio.h>

#include "vector.h"
#include "utils.h"
#include "constants.h"

typedef enum TaskType {
    TASK_TYPE_SLEEP,
    TASK_TYPE_EXEC,
} TaskType;

typedef struct SleepTaskArgs {
    unsigned int duration;  // sleep duration in seconds
} SleepTaskArgs;

typedef struct ExecTaskArgs {
    const char* binary_path;  // path to executable to run
    StringVector* argv;       // array of cmd args to pass
} ExecTaskArgs;

typedef struct TaskConfig {
    char* name;                     // task name
    StringVector* requirements;     // list of names of tasks which need to be run before this one
    unsigned int timeout;           // timeout in seconds, 0 means no timeout
    char* log_path;                 // path to output logs, in format `{log_directory}/{task_name}.log`

    TaskType type;                  // task type
    union {
        SleepTaskArgs* sleep_args;  // SLEEP task args
        ExecTaskArgs* exec_args;    // EXEC task args
    };
} TaskConfig;

typedef struct ExecutionConfig {
    int max_concurrent_tasks;  // max number of tasks running in parallel

    size_t num_tasks;          // number of tasks
    TaskConfig** tasks;        // task list
} ExecutionConfig;


// Read and parse execution config from file.
// Returns NULL on error.
ExecutionConfig* ReadExecutionConfig(FILE* file, const char* log_directory);

// Free execution config instance.
// Ignores NULL config and fields.
void FreeExecutionConfig(ExecutionConfig* config);
