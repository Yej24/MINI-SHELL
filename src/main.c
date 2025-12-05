#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pipeline.h"
#include "executor.h"
#include "redirection.h"

#define MAX_INPUT 1024

int main() {
    char line[MAX_INPUT];

    while (1) {
        printf("mysh> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }
        line[strcspn(line, "\n")] = '\0'; // remove newline

        if (line[0] == '\0') continue;

        int num_cmds = 0;
        char ***commands = parse_pipeline(line, &num_cmds);

        if (num_cmds == 0) {
            free_commands(commands, num_cmds);
            continue;
        }

        // Built-in cd (only single command)
        if (num_cmds == 1 && commands[0][0] != NULL &&
            strcmp(commands[0][0], "cd") == 0) {
            char *path = commands[0][1] ? commands[0][1] : getenv("HOME");
            if (path == NULL) path = "/";
            if (chdir(path) != 0) perror("cd");

            free_commands(commands, num_cmds);
            continue;
        }

        // Execute pipeline (handles multi-pipes)
        execute_pipeline(commands, num_cmds);

        free_commands(commands, num_cmds);
    }

    return 0;
}
