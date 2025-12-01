#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "redirection.h"

int handle_redirection(char **args){
    int fd;

    for (int i=0; args[i] != NULL; i++){

        //> (output redirection)
        if (strcmp(args[i], ">") == 0){
            if (args[i+1] == NULL) {
                fprintf(stderr, "mysh: syntax error near '>'\n");
                return - 1;

            }
            fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                return -1;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            return 1;

        }

        // >> (append output redirection)
         if (strcmp(args[i], ">>") == 0){
            if (args[i+1] == NULL) {
                fprintf(stderr, "mysh: syntax error near '>>'\n");
                return -1;
            }
            fd = open(args[i+1], O_CREAT | O_WRONLY | O_APPEND, 0644);
            if (fd < 0) {
                perror("open");
                return -1;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            return 1;

        }

        //< (input redirection)
        if (strcmp(args[i], "<") == 0){
            if (args[i+1] == NULL) {
                fprintf(stderr, "mysh: syntax error near '<'\n");
                return -1;

            }
            fd = open(args[i+1], O_RDONLY);
            if (fd < 0) {
                perror("open");
                return -1;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
            return 1;


        }

    }
    return 0;
    

}