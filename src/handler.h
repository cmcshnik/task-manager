#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <signal.h>

#include "config.h"
#include "constants.h"

// Handle a task in a worker.
// Can be implemented by forking even further.
// In that case exit status should be forwarded upwards to the master process.
// All output (both stdout and stderr) should be piped and "teed" in both a log file and stderr.
void HandleTask(const TaskConfig* config);
