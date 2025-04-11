#include <stdio.h>
#include <dirent.h>

#include "src/vector.h"
#include "src/queue.h"
#include "src/map.h"
#include "src/graph.h"
#include "src/config.h"
#include "src/master.h"


const int FLAGS_AMOUNT = 4;
const char* flags[] = {
    "--config", "-c",
    "--log", "-l",
    "--verbosity-type", "-v",
    "--sleep-duration", "-s"
};

typedef struct CmdArgs {
    char* config_path;
    char* log_folder;
    int verbosity_type;
    int sleep_duration;
} CmdArgs;

static void CheckingSecondArgument(int cur, int argc, char** argv) {
    if (cur == argc) {
        errno = EINVAL;
        perror("Second argument is missing");
        exit(1);
    }
    
    for (int i = 0; i < FLAGS_AMOUNT; ++i) {
        if (strcmp(argv[cur], flags[i]) == 0) {
            errno = EINVAL;
            perror("Second argument is missing");
            exit(1);
        }
    }
}

CmdArgs ParseArgs(int argc, char** argv) {
    CmdArgs args;
    int i = 1;

    args.config_path = NULL;
    args.log_folder = NULL;
    args.verbosity_type = VERBOSITY_TYPE_TABLE;
    args.sleep_duration = 1;

    while (i < argc) {
        if (strcmp(argv[i], flags[0]) == 0 || strcmp(argv[i], flags[1]) == 0) {
            i++;
            CheckingSecondArgument(i, argc, argv);

            args.config_path = argv[i];
        } else if (strcmp(argv[i], flags[2]) == 0 || strcmp(argv[i], flags[3]) == 0) {
            i++;
            CheckingSecondArgument(i, argc, argv);

            args.log_folder = argv[i];
        } else if (strcmp(argv[i], flags[4]) == 0 || strcmp(argv[i], flags[5]) == 0) {
            i++;
            CheckingSecondArgument(i, argc, argv);

            if (strcmp(argv[i], "TABLE") == 0) {
                args.verbosity_type = VERBOSITY_TYPE_TABLE;
            } else if (strcmp(argv[i], "NONE") == 0) {
                args.verbosity_type = VERBOSITY_TYPE_NONE;
            } else if (strcmp(argv[i], "GRAPH") == 0) {
                args.verbosity_type = VERBOSITY_TYPE_GRAPH;
            } else {
                errno = EINVAL;
                perror("Unknown verbosuty type");
                exit(1);
            }
        } else if (strcmp(argv[i], flags[6]) == 0 || strcmp(argv[i], flags[7]) == 0) {
            i++;
            CheckingSecondArgument(i, argc, argv);

            args.sleep_duration = MyAtoi(argv[i]);
            if (args.sleep_duration == -1) {
                errno = EINVAL;
                perror("Wrong sleep duration agrument");
                exit(1);
            } 
        }

        i++;
    }

    if (args.log_folder != NULL && args.config_path != NULL) {
        return args;
    }

    errno = EINVAL;
    perror("Config or log path is missing");
    exit(1);
}

// valgrind --leak-check=full --track-origins=yes
int main(int argc, char** argv) {
    errno = 0;

    CmdArgs args = ParseArgs(argc, argv);
    MasterArgs master_args;
    master_args.config_path = args.config_path;
    master_args.log_path = args.log_folder;
    master_args.drawer_sleep_duration = args.sleep_duration;
    master_args.verbosity_type = args.verbosity_type;

    MasterResult res = RunMaster(&master_args);
    fprintf(stderr, "\nMaster aborted with code %d: %s\n", res.status, res.message);
    
    return 0;
}
