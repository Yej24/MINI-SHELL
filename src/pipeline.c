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

char ***parse_pipeline(char *input, int *num_cmds) {
    char ***commands = malloc(sizeof(char **) * MAX_CMDS);
    *num_cmds = 0;

    char *segment = strtok(input, "|");
    while (segment != NULL && *num_cmds < MAX_CMDS) {
        segment = trim(segment);

        char **args = malloc(sizeof(char *) * MAX_ARGS);
        int argc = 0;
        char *p = segment;

        while (*p != '\0' && argc < MAX_ARGS - 1) {
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '\0') break;

            char *start;
            if (*p == '"') {
                p++;
                start = p;
                while (*p != '"' && *p != '\0') p++;
            } else if (*p == '\'') {
                p++;
                start = p;
                while (*p != '\'' && *p != '\0') p++;
            } else {
                start = p;
                while (*p != ' ' && *p != '\t' && *p != '\0') p++;
            }

            int len = p - start;
            args[argc] = malloc(len + 1);
            strncpy(args[argc], start, len);
            args[argc][len] = '\0';

            if (*p == '"' || *p == '\'') p++;
            argc++;
        }

        args[argc] = NULL;
        commands[*num_cmds] = args;
        (*num_cmds)++;

        segment = strtok(NULL, "|");
    }

    return commands;
}

void free_commands(char ***commands, int num_cmds) {
    for (int i = 0; i < num_cmds; i++) {
        char **cmd = commands[i];
        for (int j = 0; cmd[j] != NULL; j++) free(cmd[j]);
        free(cmd);
    }
    free(commands);
}
