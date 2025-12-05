#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pipeline.h"

#define MAX_ARGS 64
#define MAX_CMDS 32

// Trim leading/trailing whitespace
static char *trim(char *s) {
    while (*s == ' ' || *s == '\t') s++;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t')) {
        *end = '\0';
        end--;
    }
    return s;
}

// Parse one command segment (handles quoting)
static char **parse_segment(char *segment) {
    char **args = malloc(MAX_ARGS * sizeof(char *));
    int argc = 0;
    char *p = segment;

    while (*p != '\0' && argc < MAX_ARGS - 1) {

        // Skip spaces
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;

        char *start;

        // Double quotes
        if (*p == '"') {
            p++;
            start = p;
            while (*p != '"' && *p != '\0') p++;
        }

        // Single quotes
        else if (*p == '\'') {
            p++;
            start = p;
            while (*p != '\'' && *p != '\0') p++;
        }

        // Normal word
        else {
            start = p;
            while (*p != ' ' && *p != '\t' && *p != '\0') p++;
        }

        int length = p - start;
        args[argc] = malloc(length + 1);
        strncpy(args[argc], start, length);
        args[argc][length] = '\0';

        if (*p == '"' || *p == '\'')
            p++; // skip closing quote

        argc++;
    }

    args[argc] = NULL;
    return args;
}

// Parse full pipeline: cmd1 | cmd2 | cmd3 ...
char ***parse_pipeline(char *input, int *num_cmds) {
    char ***commands = malloc(sizeof(char **) * MAX_CMDS);
    *num_cmds = 0;

    char *segment = strtok(input, "|");

    while (segment != NULL && *num_cmds < MAX_CMDS) {
        segment = trim(segment);
        commands[*num_cmds] = parse_segment(segment);
        (*num_cmds)++;

        segment = strtok(NULL, "|");
    }

    return commands;
}

// Free commands
void free_commands(char ***commands, int num_cmds) {
    for (int i = 0; i < num_cmds; i++) {
        char **cmd = commands[i];
        for (int j = 0; cmd[j] != NULL; j++) {
            free(cmd[j]);
        }
        free(cmd);
    }
    free(commands);
}
