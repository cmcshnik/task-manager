#pragma once

#include <pthread.h>

#include "context.h"
#include "config.h"
#include "handler.h"

typedef struct MasterArgs {
    char* config_path;  // path to execution config
    char* log_path;     // path to log directory

    // use the following fields only in case you want to implement verbose task status rendering
    VerbosityType verbosity_type;        // task status rendering mode
    unsigned int drawer_sleep_duration;  // time interval between two consecutive renderings
} MasterArgs;

// NOTE: feel free to rearrange this
typedef enum MasterStatus {
    MASTER_STATUS_SUCCESS = 0,         // everything's fine (though some worker processes could have failed)
    MASTER_STATUS_BAD_FILE = 1,       // file opening failed
    MASTER_STATUS_CONFIG_ERROR = 2,         // config validation failed
    MASTER_STATUS_INTERNAL_ERROR = 3,  // dynamic memory fails, syscall fails etc.
    // <-- YOUR CODE HERE -->
} MasterStatus;

typedef struct MasterResult {
    MasterStatus status;  // master process status
    const char* message;        // message to be printed out to stderr, or NULL
} MasterResult;

// Run master process.
MasterResult RunMaster(const MasterArgs* args);
