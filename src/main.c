#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "redirection.h"

#define MAX_ARGS 64
#define MAX_INPUT 1024


// Parse input with quotes

void parse_input(char *input, char **args) {
    int argc = 0;
    char *p = input;

    while (*p != '\0' && argc < MAX_ARGS - 1) {

        // Skip whitespace
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;

        char *start;

        // Case 1: Double quoted
        if (*p == '"') {
            p++;
            start = p;

            while (*p != '"' && *p != '\0') p++;

            int length = p - start;
            args[argc] = malloc(length + 1);
            strncpy(args[argc], start, length);
            args[argc][length] = '\0';

            if (*p == '"') p++; // skip closing quote
        }

        // Case 2: Single quoted
        else if (*p == '\'') {
            p++;
            start = p;

            while (*p != '\'' && *p != '\0') p++;

            int length = p - start;
            args[argc] = malloc(length + 1);
            strncpy(args[argc], start, length);
            args[argc][length] = '\0';

            if (*p == '\'') p++;
        }

        // Case 3: Normal word
        else {
            start = p;

            while (*p != ' ' && *p != '\t' && *p != '\0')
                p++;

            int length = p - start;
            args[argc] = malloc(length + 1);
            strncpy(args[argc], start, length);
            args[argc][length] = '\0';
        }

        argc++;
    }

    args[argc] = NULL;
}


// Pipe handling

void free_args(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
}

void handle_pipe(char *input) {
    char *cmd1 = strtok(input, "|");
    char *cmd2 = strtok(NULL, "|");

    if (cmd2 == NULL) {
        fprintf(stderr, "mysh: invalid pipe command\n");
        return;
    }

    // Trim spaces
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
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execvp(args1[0], args1);
        fprintf(stderr, "mysh: command not found: %s\n", args1[0]);
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        execvp(args2[0], args2);
        fprintf(stderr, "mysh: command not found: %s\n", args2[0]);
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    //Free allocated arguments
    free_args(args1);
    free_args(args2);
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

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0)
            break;

        // Handle pipe
        if (strchr(input, '|') != NULL) {
            handle_pipe(input);
            continue;
        }

        // Normal command
        parse_input(input, args);

        if (args[0] == NULL)
            continue;

        // Built-in cd
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL)
                chdir(getenv("HOME"));
            else if (chdir(args[1]) != 0)
                perror("cd");
            free_args(args);
            continue;
        }

        // Fork and exec
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            free_args(args);
            continue;
        }

        if (pid == 0) {
            handle_redirection(args);
            execvp(args[0], args);
            fprintf(stderr, "mysh: command not found: %s\n", args[0]);
            exit(1);
        } else {
            wait(NULL);
        }

        free_args(args);
    }

    return 0;
}
