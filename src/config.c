#include "config.h"

typedef struct MainSection {
    char* max_concurrent_tasks;
    char* default_timeout;
} MainSection;

typedef struct TaskSection {
    char* name;
    StringVector* requires;
    char* timeout;
    char* type;
    char* sleep_duration;
    StringVector* exec_command;
} TaskSection;

typedef struct RawConfig {
    MainSection* main;

    size_t num_tasks;
    TaskSection** tasks;
} RawConfig;


MainSection* NewMainSection(void) {
    MainSection* main_section = malloc(sizeof(MainSection));
    if (!main_section) {
        errno = ENOMEM;
        return NULL;
    }

    main_section->default_timeout = NULL;
    main_section->max_concurrent_tasks = NULL;

    return main_section;
}

void FreeMainSection(MainSection* main_section) {
    if (main_section == NULL) {
        return;
    }
    
    free(main_section->default_timeout);
    free(main_section->max_concurrent_tasks);
    free(main_section);
}

TaskSection* NewTaskSection(void) {
    TaskSection* task_section = malloc(sizeof(TaskSection));
    if (!task_section) {
        errno = ENOMEM;
        return NULL;
    }

    task_section->requires = NewStringVector(1);
    if (!task_section->requires) {
        free(task_section);
        return NULL;
    }

    task_section->exec_command = NewStringVector(1);
    if (!task_section->exec_command) {
        FreeStringVector(task_section->requires);
        free(task_section);
        return NULL;
    }

    task_section->name = NULL;
    task_section->sleep_duration = NULL;
    task_section->timeout = NULL;
    task_section->type = NULL;
    return task_section;
}

void FreeTaskSection(TaskSection* task_section) {
    if (!task_section) {
        return;
    }

    FreeStringVector(task_section->exec_command);
    free(task_section->name);
    free(task_section->sleep_duration);
    free(task_section->timeout);
    free(task_section->type);
    FreeStringVector(task_section->requires);
    free(task_section);
}

RawConfig* NewRawConfig(void) {
    RawConfig* raw_config = malloc(sizeof(RawConfig));
    if (!raw_config) {
        errno = ENOMEM;
        return NULL;
    }

    raw_config->main = NULL;
    raw_config->num_tasks = 0;
    raw_config->tasks = malloc(sizeof(TaskSection*) * MAX_TASKS_COUNT);
    if (!raw_config->tasks) {
        errno = ENOMEM;
        FreeMainSection(raw_config->main);
        free(raw_config);
        return NULL;
    }

    return raw_config;
}

void FreeRawConfig(RawConfig* raw_config) {
    if (!raw_config) {
        return;
    }

    if (raw_config) {
        FreeMainSection(raw_config->main);
    }

    for (int i = 0; i < raw_config->num_tasks; ++i) {
        if (raw_config) {
            FreeTaskSection(raw_config->tasks[i]);
        }
    }
    
    if (raw_config->tasks) {
        free(raw_config->tasks);
    }
    free(raw_config);
    
}

void* FailedParsingRawConfig(RawConfig* raw_config, 
                             StringVector* vec, 
                             int config_line_number,
                             char* message,
                             int errno_code,
                             TaskSection* task_section,
                             MainSection* main_section
                            ) {
    errno = errno_code;
    if (strcmp(message, "") == 0) {
        perror("raw config parsing error");
    } else {
        fprintf(stderr, "raw config parsing error: ");
        perror(message);
    }

    if (config_line_number != -1) {
        fprintf(stderr, "In config file in line %d\n", config_line_number);
    }
    FreeTaskSection(task_section);
    FreeMainSection(main_section);
    FreeRawConfig(raw_config);
    FreeStringVector(vec);
    return NULL;
}

// Parse raw config (main section and task sections).
// Validates config syntax (`key: value` pairs), key and section names, counts tasks.
// Returns NULL on error.
RawConfig* ParseRawConfig(FILE* file) {
    StringVector* vec;
    RawConfig* raw_config = NewRawConfig();
    if (!raw_config) {
        errno = ENOMEM;
        return NULL;
    }

    char buffer[BUF_SIZE];
    const char* first_token;
    bool main_section_occured = false;
    int vec_length;
    int status;
    int line_number = 0;

    while (fgets(buffer, BUF_SIZE, file)) {
        vec = SplitString(buffer, " ");
        if (!vec) {
            return FailedParsingRawConfig(raw_config, vec, -1, "", errno, NULL, NULL);
        }

        line_number++;
        vec_length = GetStringVectorLength(vec);
        if (vec_length != 0) {
            first_token = GetStringVectorElement(vec, 0);
        }
        
        // printf("%ld %s\n", GetStringVectorLength(vec), first_token);

        if ((vec_length == 0) || (first_token[0] == '#')) {
            FreeStringVector(vec);
            continue;

        } else if (strcmp(first_token, "[main]") == 0) {
            if (vec_length != 1 || main_section_occured) {
                return FailedParsingRawConfig(raw_config, vec, line_number, 
                                              "config parsing error: invalid main section header (or duplicate)", 
                                              EINVAL, NULL, NULL);
            }

            main_section_occured = true;

            // Getting ready for main section parsing
            FreeStringVector(vec);
            MainSection* main_section = NewMainSection();
            if (!main_section) {
                return FailedParsingRawConfig(raw_config, vec, -1, "config parsing error", errno, NULL, NULL);
            }

            // Parsing
            while (fgets(buffer, BUF_SIZE, file)) {
                vec = SplitString(buffer, " ");
                if (!vec) {
                    return FailedParsingRawConfig(raw_config, vec, -1, "config parsing error", errno, NULL, NULL);
                }

                line_number++;
                vec_length = GetStringVectorLength(vec);
                if (vec_length != 0) {
                    first_token = GetStringVectorElement(vec, 0);
                }
                
                if (vec_length == 0) {
                    FreeStringVector(vec);
                    break;

                } else if (first_token[0] == '#') {
                    FreeStringVector(vec);
                    continue;
                    
                } else if (strcmp(first_token, "max_concurrent_tasks:") == 0 ||
                           strcmp(first_token, "default_timeout:") == 0
                        ) {
                    
                    if (vec_length != 2) {
                        return FailedParsingRawConfig(raw_config, vec, line_number, 
                                                      "config parsing error: invalid main field", EINVAL, NULL, main_section);
                    }

                    char* tmp_string_ptr;

                    if (strcmp(first_token, "max_concurrent_tasks:") == 0) {
                        main_section->max_concurrent_tasks = strdup(GetStringVectorElement(vec, 1));
                        tmp_string_ptr = main_section->max_concurrent_tasks;
                    } else if (strcmp(first_token, "default_timeout:") == 0) {
                        main_section->default_timeout = strdup(GetStringVectorElement(vec, 1));
                        tmp_string_ptr = main_section->default_timeout;
                    }
                    
                    if (!tmp_string_ptr) {
                        return FailedParsingRawConfig(raw_config, vec, -1, "config parsing error", ENOMEM, NULL, main_section);
                    }

                    tmp_string_ptr = NULL;

                } else {
                    return FailedParsingRawConfig(raw_config, vec, line_number,
                                                  "config parsing error: unknown main field", EINVAL, NULL, main_section);
                }

                FreeStringVector(vec);
            }

            raw_config->main = main_section;

        } else if (strcmp(first_token, "[task]") == 0) {
            if (vec_length != 1) {
                return FailedParsingRawConfig(raw_config, vec, line_number, 
                                              "invalid task section header", EINVAL, NULL, NULL);
            }

            // Getting ready for task section parsing
            FreeStringVector(vec);
            TaskSection* task_section = NewTaskSection();
            if (!task_section) {
                return FailedParsingRawConfig(raw_config, vec, -1, "", errno, NULL, NULL);
            }

            // Parsing
            while (fgets(buffer, BUF_SIZE, file)) {
                vec = SplitString(buffer, " ");
                if (!vec) {
                    return FailedParsingRawConfig(raw_config, vec, -1, "", errno, NULL, NULL);
                }

                line_number++;
                vec_length = GetStringVectorLength(vec);
                if (vec_length != 0) {
                    first_token = GetStringVectorElement(vec, 0);
                }

                if (vec_length == 0) {
                    FreeStringVector(vec);
                    break;
                } else if (first_token[0] == '#') {
                    FreeStringVector(vec);
                    continue;
                    
                } else if (strcmp(first_token, "name:") == 0 || 
                           strcmp(first_token, "timeout:") == 0 ||
                           strcmp(first_token, "type:") == 0 ||
                           strcmp(first_token, "sleep_duration:") == 0
                        ) {
                    
                    if (vec_length != 2) {
                        return FailedParsingRawConfig(raw_config, vec, line_number,
                                                      "invalid task field", EINVAL, task_section, NULL);
                    }

                    char* tmp_string_ptr;

                    if (strcmp(first_token, "name:") == 0) {
                        if (task_section->name) {
                            return FailedParsingRawConfig(raw_config, vec, line_number,
                                                      "multipule name fields occured", EINVAL, task_section, NULL);
                        }

                        task_section->name = strdup(GetStringVectorElement(vec, 1));
                        tmp_string_ptr = task_section->name;
                    } else if (strcmp(first_token, "timeout:") == 0) {
                        if (task_section->timeout) {
                            return FailedParsingRawConfig(raw_config, vec, line_number,
                                                      "multipule timeout fields occured", EINVAL, task_section, NULL);
                        }

                        task_section->timeout = strdup(GetStringVectorElement(vec, 1));
                        tmp_string_ptr = task_section->timeout;
                    } else if (strcmp(first_token, "type:") == 0) {
                        if (task_section->type) {
                            return FailedParsingRawConfig(raw_config, vec, line_number,
                                                      "multipule type fields occured", EINVAL, task_section, NULL);
                        }

                        task_section->type = strdup(GetStringVectorElement(vec, 1));
                        tmp_string_ptr = task_section->type;
                    } else if (strcmp(first_token, "sleep_duration:") == 0) {
                        if (task_section->sleep_duration) {
                            return FailedParsingRawConfig(raw_config, vec, line_number,
                                                      "multipule sleep duration fields occured", EINVAL, task_section, NULL);
                        }

                        task_section->sleep_duration = strdup(GetStringVectorElement(vec, 1));
                        tmp_string_ptr = task_section->sleep_duration;
                    }

                    if (!tmp_string_ptr) {
                        return FailedParsingRawConfig(raw_config, vec, -1, "", ENOMEM, task_section, NULL);
                    }

                    tmp_string_ptr = NULL;

                } else if (strcmp(first_token, "requires:") == 0 ||
                           strcmp(first_token, "exec_command:") == 0
                        ) {
                    
                    if (vec_length == 1) {
                        return FailedParsingRawConfig(raw_config, vec, line_number, 
                                                      "invalid requires or exec_command task field", EINVAL, task_section, NULL);
                    }

                    StringVector* tmp_string_vector_ptr;
                    if (strcmp(first_token, "requires:") == 0) {
                        if (GetStringVectorLength(task_section->requires) != 0) {
                            return FailedParsingRawConfig(raw_config, vec, line_number,
                                                      "multipule requires fields occured", EINVAL, task_section, NULL);
                        }

                        tmp_string_vector_ptr = task_section->requires;
                    } else if (strcmp(first_token, "exec_command:") == 0) {
                        if (GetStringVectorLength(task_section->exec_command) != 0) {
                            return FailedParsingRawConfig(raw_config, vec, line_number,
                                                      "multipule exec_command fields occured", EINVAL, task_section, NULL);
                        }

                        tmp_string_vector_ptr = task_section->exec_command;
                    }

                    for (int k = 1; k < vec_length; ++k) {
                        status = AppendToStringVector(tmp_string_vector_ptr, GetStringVectorElement(vec, k));
                        if (!status) {
                            return FailedParsingRawConfig(raw_config, vec, -1, "config parsing error", errno, task_section, NULL);
                        }
                    }

                    tmp_string_vector_ptr = NULL;

                } else {
                    return FailedParsingRawConfig(raw_config, vec, line_number, 
                                                  "config parsing error: unknown task field", EINVAL, task_section, NULL);
                };

                FreeStringVector(vec);
            }

            raw_config->tasks[raw_config->num_tasks] = task_section;
            raw_config->num_tasks++;

        } else {
            return FailedParsingRawConfig(raw_config, vec, line_number, 
                                          "config parsing error: unknown record", EINVAL, NULL, NULL);
        }
    }

    return raw_config;
}

void FreeTaskConfig(TaskConfig* config) {
    if (!config) {
        return;
    }
    
    free(config->name);
    FreeStringVector(config->requirements);
    free(config->log_path);

    if (config->type == TASK_TYPE_SLEEP) {
        if (config->sleep_args) {
            free(config->sleep_args);
        }  
    } else if (config->type == TASK_TYPE_EXEC) {
        
        if (config->exec_args) {
            if (config->exec_args->argv) {
                FreeStringVector(config->exec_args->argv);
            }

            free(config->exec_args);
        }
    }

    free(config);
}

void* FailedTaskConfigCreation(TaskConfig* config, const char* message, int error_code) {
    errno = error_code;
    if (strcmp(message, "") == 0) {
        perror("task config creation failed");
    } else {
        fprintf(stderr, "%s", "task config creation failed: ");
        perror(message);
    }

    FreeTaskConfig(config);
    return NULL;
}

TaskConfig* NewTaskConfig(TaskSection* task_section, int general_timeout, const char* log_directory) {
    TaskConfig* config = malloc(sizeof(TaskConfig));
    config->name = NULL;
    config->log_path = NULL;
    config->sleep_args = NULL;
    config->exec_args = NULL;
    config->requirements = NULL;
    config->type = 0;
    if (!config) {
        return FailedTaskConfigCreation(config, "memory error", ENOMEM);
    }

    // REQUIRED FIELDS: name, type: SLEEP --> sleep_duration, EXEC --> exec_command

    // Name
    if (!task_section->name) {
        return FailedTaskConfigCreation(config, "task name missing", EINVAL);
    }

    config->name = strdup(task_section->name);
    if (!config->name) {
        return FailedTaskConfigCreation(config, "memory error", ENOMEM);
    }

    // Task type
    if (task_section->type) {
        if (strcmp(task_section->type, "SLEEP") == 0) {
            config->type = TASK_TYPE_SLEEP;

            config->sleep_args = malloc(sizeof(SleepTaskArgs));
            if (!config->sleep_args) {
                return FailedTaskConfigCreation(config, "memory error", ENOMEM);
            }

            config->sleep_args->duration = MyAtoi(task_section->sleep_duration);
            if (config->sleep_args->duration == -1) {
                return FailedTaskConfigCreation(config, "invalid or missing sleep duration argument", EINVAL);
            }
        } else if (strcmp(task_section->type, "EXEC") == 0){
            int status;

            size_t argv_len = GetStringVectorLength(task_section->exec_command);
            if (argv_len == 0) {
                return FailedTaskConfigCreation(config, "invalid or missing exec arguments", EINVAL);
            }

            config->type = TASK_TYPE_EXEC;

            config->exec_args = malloc(sizeof(ExecTaskArgs));
            if (!config->exec_args) {
                return FailedTaskConfigCreation(config, "memory error", ENOMEM);
            }

            config->exec_args->binary_path = PATH_TO_EXECUTABLE;
            
            config->exec_args->argv = NewStringVector(argv_len);
            if (!config->exec_args->argv) {
                return FailedTaskConfigCreation(config, "memory error", EINVAL);
            }

            status = AppendToStringVector(config->exec_args->argv, PATH_TO_EXECUTABLE);
            if (!status) {
                return FailedTaskConfigCreation(config, "memory error", errno);
            }
            
            if (strcmp(PATH_TO_EXECUTABLE, "/bin/bash") == 0) {
                status = AppendToStringVector(config->exec_args->argv, "-c");
                if (!status) {
                    return FailedTaskConfigCreation(config, "memory error", errno);
                }
            }

            const char* buf_ptr;
            StringVector* buffer_for_gluing = NewStringVector(GetStringVectorLength(task_section->exec_command));
            if (!buffer_for_gluing) {
                return FailedTaskConfigCreation(config, "memory error", EINVAL);
            }

            for (int i = 0; i < argv_len; ++i) {
                buf_ptr = GetStringVectorElement(task_section->exec_command, i);

                status = AppendToStringVector(buffer_for_gluing, buf_ptr);
                if (!status) {
                    FreeStringVector(buffer_for_gluing);
                    return FailedTaskConfigCreation(config, "memory error", EINVAL);
                }
            }

            char* buf_ptr2 = GlueStringVectorWithDelimeter(buffer_for_gluing, " ");
            if (!buf_ptr2) {
                FreeStringVector(buffer_for_gluing);
                return FailedTaskConfigCreation(config, "memory error", EINVAL);
            }

            AppendToStringVector(config->exec_args->argv, buf_ptr2);

            FreeStringVector(buffer_for_gluing);
            free(buf_ptr2);

            // Appending NULL for execv command 
            status = AppendToStringVector(config->exec_args->argv, NULL);
            if (!status) {
                return FailedTaskConfigCreation(config, "memory error", EINVAL);
            }
        } else {
            return FailedTaskConfigCreation(config, "unknown task type", EINVAL);
        }
    } else {
        return FailedTaskConfigCreation(config, "missing task type", EINVAL);
    }

    // Requirements
    size_t rec_len = GetStringVectorLength(task_section->requires);
    int status;

    config->requirements = NewStringVector(rec_len);
    if (!config->requirements) {
        return FailedTaskConfigCreation(config, "memory error", errno);
    }

    const char* buf_ptr;

    for (int i = 0; i < rec_len; ++i) {
        buf_ptr = GetStringVectorElement(task_section->requires, i);
        status = AppendToStringVector(config->requirements, buf_ptr);
        if (!status) {
            return FailedTaskConfigCreation(config, "memory error", errno);
        }
    }

    // Timeout
    if (task_section->timeout) {
        config->timeout = MyAtoi(task_section->timeout);
        if (config->timeout == -1) {
            return FailedTaskConfigCreation(config, "invalid task timeout argument", EINVAL);
        }

        if (config->timeout > general_timeout || config->timeout == 0) {
            config->timeout = general_timeout;
        }
    } else {
        config->timeout = general_timeout;
    }
    
    // Log path
    char* log_file_name = malloc(sizeof(char) * (strlen(config->name) + 4 + 1));
    if (!log_file_name) {
        return FailedTaskConfigCreation(config, "memory error", errno);
    }

    log_file_name[0] = '\0';
    strcat(log_file_name, config->name);
    strcat(log_file_name, ".log");

    config->log_path = JoinPath(log_directory, log_file_name);
    if (!config->log_path) {
        return FailedTaskConfigCreation(config, "invalid log path argument", errno);
    }

    free(log_file_name);

    return config;
}

void FreeExecutionConfig(ExecutionConfig* config) {
    if (!config) {
        return;
    }
   
    for (int i = 0; i < config->num_tasks; ++i) {
        FreeTaskConfig(config->tasks[i]);
    } 

    free(config->tasks);
    free(config);
}

void* ExecutionConfigCreationFailed(ExecutionConfig* config, char* message, int error_code) {
    errno = error_code;
    if (strcmp(message, "") == 0) {
        perror("execution config creation failed");
    } else {
        fprintf(stderr, "execution config creation failed: ");
        perror(message);
    }
    FreeExecutionConfig(config);   
    return NULL;
}

ExecutionConfig* NewExecutionConfig(RawConfig* raw_config, const char* log_directory) {
    ExecutionConfig* exec_config = malloc(sizeof(ExecutionConfig));
    if (!exec_config) {
        errno = ENOMEM;
        return NULL;
    }

    size_t num_tasks = raw_config->num_tasks;
    exec_config->num_tasks = num_tasks;

    exec_config->tasks = malloc(sizeof(TaskConfig*) * num_tasks);
    if (!exec_config->tasks) {
        free(exec_config);
        errno = ENOMEM;
        return NULL;
    }

    for (int i = 0; i < num_tasks; ++i) {
        exec_config->tasks[i] = NULL;
    }

    int general_timeout = DEFAULT_TIMEOUT;
    int max_concurrent_tasks = DEFAULT_MAX_CUNCURRENT_TASKS;

    // Main section
    if (raw_config->main) {
        if (raw_config->main->max_concurrent_tasks) {
            max_concurrent_tasks = MyAtoi(raw_config->main->max_concurrent_tasks);
            if (max_concurrent_tasks == -1) {
                return ExecutionConfigCreationFailed(exec_config, "invalid argument for max concurrent tasks", EINVAL);
            }
        } else {
            max_concurrent_tasks = DEFAULT_MAX_CUNCURRENT_TASKS;
        }

        if (raw_config->main->default_timeout) {
            general_timeout = MyAtoi(raw_config->main->default_timeout);
            if (general_timeout == -1) {
                return ExecutionConfigCreationFailed(exec_config, "invalid argument for general timeout", EINVAL);
            }
        } else {
            general_timeout = DEFAULT_TIMEOUT;
        }
    }

    exec_config->max_concurrent_tasks = max_concurrent_tasks;

    for (int i = 0; i < num_tasks; ++i) {
        TaskConfig* new_task_config = NewTaskConfig(raw_config->tasks[i], general_timeout, log_directory);
        if (!new_task_config) {
            exec_config->tasks[i] = NULL;
            return ExecutionConfigCreationFailed(exec_config, "failed creating one of the task configs", errno);
        }

        exec_config->tasks[i] = new_task_config;
    }

    return exec_config;
}

void* FailedReadingExecutionConfig(
    char* message,
    int error_code,
    RawConfig* raw_config
) {
    
    errno = error_code;
    if (strcmp(message, "") == 0) {
        perror("reading execution config error");
    } else {
        fprintf(stderr, "%s", "reading execution config error: ");
        perror(message);
    }
    
    FreeRawConfig(raw_config);
    return NULL;
}

// Parse ExecutionConfig.
// Validates required fields, value types and restrictions.
// Returns NULL on error.
ExecutionConfig* ReadExecutionConfig(FILE* file, const char* log_directory) {
    RawConfig* raw_config = ParseRawConfig(file);
    if (!raw_config) {
        return FailedReadingExecutionConfig("", errno, NULL);
    }

    ExecutionConfig* exec_config = NewExecutionConfig(raw_config, log_directory);
    if (!exec_config) {
        return FailedReadingExecutionConfig("", errno, raw_config);
    }

    FreeRawConfig(raw_config);

    return exec_config;
}