#include "master.h"
#include "context.h"

typedef struct ResourceManager {
    FILE* input_file;
    ExecutionConfig* config;
    StringMap* string_map;
    Graph* graph;
    IntMap* pid_to_idx;
    Queue* queue;
    Context* context;
} ResourceManager;

typedef struct RenderStruct {
    const Context* context;
    VerbosityType verbosity_type;
    bool redraw;
    unsigned int drawer_sleep_duration;
    bool* do_render;
} RenderStruct;

static void* RenderFunc(void* arg) {
    RenderStruct* args = (RenderStruct*)arg;

    while (*args->do_render) {
        DrawContext(args->context, args->verbosity_type, args->redraw);
        sleep(args->drawer_sleep_duration);
    }

    return NULL;
}

static void CleanupResources(ResourceManager* manager) {
    if (!manager) {
        return;
    }

    if (manager->input_file) {
        fclose(manager->input_file);
    }
    FreeExecutionConfig(manager->config);
    FreeStringMap(manager->string_map);
    FreeGraph(manager->graph);
    FreeQueue(manager->queue);
    FreeIntMap(manager->pid_to_idx);
    FreeContext(manager->context);
}

static bool FailingTaskUpperNeighbors(
    const Graph* graph, 
    int completed_task_idx,
    Context* context) 
{
    if (!graph || !context) {
        return false;
    }

    context->tasks[completed_task_idx].task_status = TASK_STATUS_FAILED;
    int graph_size = GetGraphSize(graph);

    for (int i = 0; i < graph_size; ++i) {
        if (graph->matrix_[ completed_task_idx * graph_size + i ] == 1) {
            FailingTaskUpperNeighbors(graph, i, context);
        }
    }

    return true;
}

static MasterResult AbortMaster(const char* message, int error_code, ResourceManager* rm) {
    MasterResult res = {
        .status = error_code,
        .message = message
    };

    CleanupResources(rm);
    return res;
}

MasterResult RunMaster(const MasterArgs* args) {
    ResourceManager rm = {
        .input_file = NULL,
        .config = NULL,
        .graph = NULL,
        .string_map = NULL,
        .pid_to_idx = NULL,
        .queue = NULL,
        .context = NULL
    };

    // TODO:
    // * parse config
    // * map task names to indices
    // * build dependency graph
    // * initialize task queue
    // * process tasks

    // Parsing config
    FILE* config_file = fopen(args->config_path, "r");
    if (!config_file) {
        return AbortMaster("reading file error", MASTER_STATUS_BAD_FILE, &rm);
    }
    rm.input_file = config_file;

    ExecutionConfig* config = ReadExecutionConfig(config_file, args->log_path);
    if (!config) {
        return AbortMaster("config construction error", MASTER_STATUS_CONFIG_ERROR, &rm);
    }
    rm.config = config;

    if (config->num_tasks == 0) {
        fprintf(stderr, "No tasks to be executed\n");
        return AbortMaster("Success", MASTER_STATUS_SUCCESS, &rm);
    }

    // Mapping task names to indices
    StringMap* string_map = NewStringMap(config->num_tasks * 2); // Multiplication by two for the purpose of incomplete filling of the map
    if (!string_map) {
        return AbortMaster("string map construction error", MASTER_STATUS_INTERNAL_ERROR, &rm);
    }
    rm.string_map = string_map;

    int status;

    int value;
    for (int i = 0; i < config->num_tasks; ++i) {
        status = SetStringMapValue(string_map, config->tasks[i]->name, i, false);
        if (!status) {
            return AbortMaster("tasks with the same name occured", MASTER_STATUS_CONFIG_ERROR, &rm);
        }
    }

    // Clarifying, that all reqirements exists
    size_t req_len;
    for (int i = 0; i < config->num_tasks; ++i) {
        req_len = GetStringVectorLength(config->tasks[i]->requirements); 

        for (int k = 0; k < req_len; ++k) {
            const char* required = GetStringVectorElement(config->tasks[i]->requirements, k);

            if (strcmp(required, "none") == 0) {
                if (req_len == 1) {
                    break;
                } else {
                    return AbortMaster("none occured in task requirements", MASTER_STATUS_CONFIG_ERROR, &rm);
                }
            } 

            status = GetStringMapValue(
                string_map,
                required,
                &value
            );

            if (!status) {
                return AbortMaster("invalid task requirements", MASTER_STATUS_CONFIG_ERROR, &rm);
            }
        }
    }

    // Building dependency graph
    Graph* graph = NewGraph(config->num_tasks);
    if (!graph) {
        return AbortMaster("graph construction error", MASTER_STATUS_INTERNAL_ERROR, &rm);
    }
    rm.graph = graph;

    int graph_size = GetGraphSize(graph);

    for (int i = 0; i < config->num_tasks; i++) {
        req_len = GetStringVectorLength(config->tasks[i]->requirements); 
    
        for (int k = 0; k < req_len; ++k) {
            const char* required = GetStringVectorElement(config->tasks[i]->requirements, k);

            if (strcmp(required, "none") == 0) {
                break;
            }

            GetStringMapValue(
                string_map,
                required,
                &value
            );

            status = AddDirectedEdge(graph, i, value);
            if (!status) {
                return AbortMaster("graph construction error", MASTER_STATUS_INTERNAL_ERROR, &rm);
            }
        }
    }

    // Clarifying, that there are no cycles
    if (!IsAcyclic(graph)) {
        return AbortMaster("cycle in requirements exists", MASTER_STATUS_CONFIG_ERROR, &rm);
    }

    // Creating context and thread for table rendering
    Context* context = NewContext(graph, config);
    if (!context) {
        return AbortMaster("context creation error", MASTER_STATUS_INTERNAL_ERROR, &rm);    
    }
    rm.context = context;

    bool do_render = true;
    RenderStruct render_struct = {
        .context = context,
        .drawer_sleep_duration = args->drawer_sleep_duration,
        .redraw = true,
        .verbosity_type = args->verbosity_type,
        .do_render = &do_render
    };

    DrawContext(context, args->verbosity_type, false);

    pthread_t thread;
    pthread_create(&thread, NULL, RenderFunc, &render_struct);

    // Initialiaing queue
    Queue* queue = NewQueue();
    if (!queue) {
        return AbortMaster("queue construction error", MASTER_STATUS_INTERNAL_ERROR, &rm);
    }
    rm.queue = queue;

    for (int i = 0; i < graph_size; ++i) {
        if (!VertexHasSuccessors(graph, i)) {
            status = Push(queue, i);
            if (!status) {
                return AbortMaster("queue pushing error", MASTER_STATUS_INTERNAL_ERROR, &rm);
            }

            context->tasks[i].task_status = TASK_STATUS_QUEUED;
        }
    }

    // Processing tasks
    IntMap* pid_to_idx = NewIntMap(config->num_tasks * 2);
    if (!pid_to_idx) {
        return AbortMaster("map creation error", MASTER_STATUS_INTERNAL_ERROR, &rm);    
    }
    rm.pid_to_idx = pid_to_idx;

    int currently_working = 0, wait_status, front_value, completed_process_idx;
    pid_t pid;

    while (true) {
        
        // Forking all processes that are ready to work
        while ( (currently_working != config->max_concurrent_tasks) && (!IsEmpty(queue)) ) {
            status = Front(queue, &front_value);
            if (!status) {
                return AbortMaster("queue getting front elem error", MASTER_STATUS_INTERNAL_ERROR, &rm);
            }

            status = Pop(queue);
            if (!status) {
                return AbortMaster("queue popping error", MASTER_STATUS_INTERNAL_ERROR, &rm);
            }  
            
            pid = fork();
            if (pid == -1) {
                return AbortMaster("fork error", MASTER_STATUS_INTERNAL_ERROR, &rm);
            }

            if (pid == 0) {
                HandleTask(config->tasks[front_value]);
            } else {
                status = SetIntMapValue(pid_to_idx, pid, front_value, false);
                if (!status) {
                    return AbortMaster("int map setting value error", MASTER_STATUS_INTERNAL_ERROR, &rm);
                }

                context->tasks[front_value].task_status = TASK_STATUS_RUNNING;
            }

            currently_working++;
        }

        // Waiting for one process to stop working and processing it
        pid = wait(&wait_status);
        status = GetIntMapValue(pid_to_idx, pid, &completed_process_idx);
        if (!status) {
            return AbortMaster("int map getting value error", MASTER_STATUS_INTERNAL_ERROR, &rm);
        }

        context->tasks[completed_process_idx].worker_status = wait_status;
        currently_working--;

        // Dependency resolution
        if (WIFEXITED(wait_status)) {
            for (int i = 0; i < graph_size; ++i) {
                if (graph->matrix_[ completed_process_idx * graph_size + i ] == 1) {
                    graph->matrix_[ completed_process_idx * graph_size + i ] = 0;

                    if ((context->tasks[i].task_status != TASK_STATUS_FAILED) && 
                        !VertexHasSuccessors(graph, i))
                    {
                        status = Push(queue, i);
                        if (!status) {
                            return AbortMaster("queue pushing error", MASTER_STATUS_INTERNAL_ERROR, &rm);
                        }
                        
                        context->tasks[i].task_status = TASK_STATUS_QUEUED;
                    }
                }
            }

            context->tasks[completed_process_idx].task_status = TASK_STATUS_SUCCESS;
        } else {
            FailingTaskUpperNeighbors(graph, completed_process_idx, context);
        }

        if (currently_working == 0 && IsEmpty(queue)) {
            do_render = false;
            pthread_join(thread, NULL);
            DrawContext(context, args->verbosity_type, true);
            break;
        }
    };

    return AbortMaster("Success", MASTER_STATUS_SUCCESS, &rm);
}
