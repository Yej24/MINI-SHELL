#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "redirection.h"

int execute_pipeline(char ***commands, int num_cmds) {
    int pipes[num_cmds - 1][2];

    // Create all pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            return -1;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Redirect input from previous pipe
            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
            // Redirect output to next pipe
            if (i < num_cmds - 1) dup2(pipes[i][1], STDOUT_FILENO);

            // Close all pipes
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Handle redirection in child
            handle_redirection(commands[i]);

            // Execute the command
            execvp(commands[i][0], commands[i]);
            perror(commands[i][0]);
            exit(1);
        }
        else if (pid < 0) {
            perror("fork");
            return -1;
        }
    }

    // Parent closes all pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < num_cmds; i++) wait(NULL);

    return 0;
}
