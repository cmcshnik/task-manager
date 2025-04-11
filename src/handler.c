#include "handler.h"

// Tee output to several output file descriptors.
// Returns number of written bytes, or -1 if failed.
static ssize_t TeeOutput(const void* buf, size_t size, int num_outputs, ...) {
    va_list args;
    va_start(args, num_outputs);

    for (int i = 0; i < num_outputs; ++i) {
        int fd = va_arg(args, int);

        size_t len = size;
        while (len) {
            ssize_t diff = write(fd, buf + size - len, len);
            if (diff < 0) {
                return diff;
            }
            len -= diff;
        }
    }

    va_end(args);
    return size;
}

static int log_file_fd;
static int child_pid;
static char* task_name;

static void WriteToLogFile(const char* message) {
    TeeOutput(message, strlen(message), 1, log_file_fd);
}

static void HandleChildOutput(int pipe_fd, const char* fd_name) {
    char buffer[BUF_SIZE];
    ssize_t nbytes;

    nbytes = read(pipe_fd, buffer, sizeof(buffer) - 1);
    
    if (nbytes > 0) {
        WriteToLogFile("=== Task output to ");
        WriteToLogFile(fd_name);
        WriteToLogFile(" ===\n");

        // Читаем данные из канала
        while ((nbytes) > 0) {
            buffer[nbytes] = '\0'; 
            TeeOutput(buffer, strlen(buffer), 2, log_file_fd, STDOUT_FILENO);
            nbytes = read(pipe_fd, buffer, sizeof(buffer) - 1);
        }

        WriteToLogFile("=== End of task output to ");
        WriteToLogFile(fd_name);
        WriteToLogFile(" ===\n");

        close(pipe_fd); 
    }
}

static void FailedHandlingTask(const char* message) {
    TeeOutput(message, strlen(message), 2, log_file_fd, STDOUT_FILENO);
    close(log_file_fd);
    raise(SIGKILL);
}

static void SigAlarmHandler(int s) {
    kill(child_pid, SIGKILL);
    FailedHandlingTask("Process killed due to timeout\n");
}

void HandleTask(const TaskConfig* config) {
    signal(SIGALRM, SigAlarmHandler);
    alarm(config->timeout);

    FILE* log_file = fopen(config->log_path, "w");
    if (!log_file) {
        FailedHandlingTask("Error while opening log file occured\n");
    }

    log_file_fd = fileno(log_file);
    task_name = config->name;

    int pipe_stdout[2], pipe_stderr[2];

    if (pipe(pipe_stdout) == -1 || pipe(pipe_stderr) == -1) {
        FailedHandlingTask("Pipe creation error occured\n");
    }

    child_pid = fork();
    if (child_pid == -1) {
        FailedHandlingTask("Forking failed\n");
    }

    if (child_pid == 0) {
        close(pipe_stdout[0]); 
        dup2(pipe_stdout[1], STDOUT_FILENO);
        close(pipe_stdout[1]); 

        close(pipe_stderr[0]); 
        dup2(pipe_stderr[1], STDERR_FILENO);
        close(pipe_stderr[1]);

        if (config->type == TASK_TYPE_SLEEP) {
            WriteToLogFile("Feeling sleepy..\n");
            sleep(config->sleep_args->duration);
            WriteToLogFile("Slept well\n");
        } else if (config->type == TASK_TYPE_EXEC) {
            WriteToLogFile("Executing commands\n");
            execv(config->exec_args->binary_path, GetStringVectorData(config->exec_args->argv));
            WriteToLogFile("Successfully executed\n");
        }

        exit(0);
    } else {
        int status;
        wait4(child_pid, &status, 0, NULL);

        close(pipe_stdout[1]); 
        close(pipe_stderr[1]);

        HandleChildOutput(pipe_stdout[0], "stdout");
        HandleChildOutput(pipe_stderr[0], "stderr");

        //char* message;
        int code_size = snprintf(NULL, 0, "%d", WEXITSTATUS(status));;
        char code[code_size + 1];
        snprintf(code, code_size + 1, "%d", WEXITSTATUS(status));

        if (WIFEXITED(status)) {
            WriteToLogFile("Proccess ended normally with code ");
            WriteToLogFile(code);
        } else {
            WriteToLogFile("Proccess aborted with code ");
            WriteToLogFile(code);
        }

        close(log_file_fd);

        if (WIFSIGNALED(status)) {
            raise(WTERMSIG(status));
        }

        exit(status);
    }
}
