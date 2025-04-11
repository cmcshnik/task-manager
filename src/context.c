#include "context.h"

// Create new context instance.
// Returns NULL on error.
Context* NewContext(const Graph* dependency_graph, const ExecutionConfig* config) {
    if (!dependency_graph || !config) {
        errno = EINVAL;
        return NULL;
    }

    Context* context = malloc(sizeof(Context));
    if (!context) {
        errno = ENOMEM;
        return NULL;
    }

    context->tasks = malloc(sizeof(TaskInfo) * config->num_tasks);
    if (!context->tasks) {
        free(context);
        errno = ENOMEM;
        return NULL;
    }

    for (int i = 0; i < config->num_tasks; ++i) {
        context->tasks[i].task_status = TASK_STATUS_UNKNOWN;
        context->tasks[i].worker_status = 0;
    }

    context->dependency_graph = dependency_graph;
    context->config = config;

    return context;
}

// Free context instance.
// Ignores NULL instance and fields.
void FreeContext(Context* context) {
    if (!context) {
        return;
    }

    free(context->tasks);
    free(context);
}

void ClearLines(int n) {
    for (int i = 0; i < n; i++) {
        // Перемещаем курсор на строку вверх
        fprintf(stderr, "\033[F"); // ESC [F
        // Очищаем текущую строку
        fprintf(stderr, "\033[2K"); // ESC [2K
    }
    fflush(stderr); // Применяем изменения немедленно
}

// Render task statuses to stdout.
// Clears previous rendering if redraw is true.
void DrawContext(const Context* context, VerbosityType verbosity_type, bool redraw) {
    if (verbosity_type == VERBOSITY_TYPE_NONE) {
        return;
    } else if (verbosity_type == VERBOSITY_TYPE_TABLE) {
        if (redraw) {
            ClearLines(context->config->num_tasks);
        }
        
        char* task_name;
        int wait_status;
        TaskStatus task_status;

        for (int i = 0; i < context->config->num_tasks; ++i) {
            task_status = context->tasks[i].task_status;
            wait_status = context->tasks[i].worker_status;
            task_name = context->config->tasks[i]->name;

            if (task_status == TASK_STATUS_UNKNOWN) {
                fprintf(stderr, "%s:\x1b[37;1m STATUS UNKNOWN \033[0m\n", task_name);
            } else if (task_status == TASK_STATUS_QUEUED) {
                fprintf(stderr, "%s:\x1b[33;1m QUEUED \033[0m\n", task_name);
            } else if (task_status == TASK_STATUS_RUNNING) {
                fprintf(stderr, "%s:\x1b[34;1m RUNNING \033[0m\n", task_name);
            } else if (task_status == TASK_STATUS_SUCCESS) {
                fprintf(stderr, "%s:\x1b[32;1m SUCCESS, CODE %d\033[0m\n", task_name, WEXITSTATUS(wait_status));
            } else {
                if (WIFSIGNALED(wait_status)) {
                    fprintf(stderr, "%s:\x1b[31;1m FAILED, SIGNAL %d\033[0m\n", task_name, WTERMSIG(wait_status));
                } else {
                    fprintf(stderr, "%s:\x1b[31;1m FAILED \033[0m\n", task_name);
                }
            }
        }
    }
}