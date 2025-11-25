#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define MAX_ARGS 64

char **parse_input(char *input) {
    char **args = malloc(sizeof(char) * MAX_ARGS);
    int arg_index = 0;

    char *token = strtok(input, " \t\n");

    while (token != NULL && arg_index < MAX_ARGS - 1){
        args[arg_index] = token;
        arg_index++;
        token = strtok(NULL, "\t\n");
    }

    args[arg_index] = NULL; 
    return args;
} 
