#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64
#define MAX_INPUT 1024

// Function to parse input into arguments

void parse_input(char *input, char **args) {
    int i = 0;
    char *token;

    // Use " \t\n\r" to cover common whitespace
    token = strtok(input, " \t\n\r");

    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n\r");
    }

    args[i] = NULL; // NULL-terminate for execvp
}
// Function to handle a single pipe
void handle_pipe(char *input) {
    char *cmd1 = strtok(input, "|");
    char *cmd2 = strtok(NULL, "|");

    if (cmd2 == NULL) {
        fprintf(stderr, "mysh: invalid pipe command\n");
        return;
    }

    // Trim leading spaces
    while (*cmd1 == ' ') cmd1++;
    while (*cmd2 == ' ') cmd2++;

    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];

    parse_input(cmd1, args1);
    parse_input(cmd2, args2);

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // First child: write to pipe
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execvp(args1[0], args1);
        fprintf(stderr, "mysh: command not found: %s\n", args1[0]);
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Second child: read from pipe
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        execvp(args2[0], args2);
        fprintf(stderr, "mysh: command not found: %s\n", args2[0]);
        exit(1);
    }

    // Parent closes both ends
    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        printf("mysh> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0)
            break;

        // Handle pipe first
        if (strchr(input, '|') != NULL) {
            handle_pipe(input);
            continue;
        }

        // Parse input once
        parse_input(input, args);

        if (args[0] == NULL)
            continue;

        // Built-in cd
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL)
                chdir(getenv("HOME"));
            else if (chdir(args[1]) != 0)
                perror("cd");
            continue;
        }

        // Fork and execute command
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            execvp(args[0], args);
            // If execvp fails
            fprintf(stderr, "mysh: command not found: %s\n", args[0]);
            exit(1);
        } else {
            wait(NULL);
        }
    }

    return 0;
}
