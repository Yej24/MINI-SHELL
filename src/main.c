#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64
#define MAX_INPUT 1024

//Parse input into arguments
void parse_input(char *input, char **args) {
    int arg_index = 0;
    char *token = strtok(input, " ");

    while (token != NULL && arg_index < MAX_ARGS - 1){
        args[arg_index] = token;
        arg_index++;
        token = strtok(NULL, " ");
    }

    args[arg_index] = NULL; 
}

int main() {
    char input [MAX_INPUT];

    while (1) {
        printf("mysh> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;

        }
        //remove newline character from input
        input[strcspn(input, "\n")] = 0;

        // Exit command
        if (strcmp(input, "exit") == 0) {
            break;
        }

        //parse into args

        parse_input(input, args);

        //fork a child process
        pid_t pid = fork();
        if(pid < 0){
            perror("fork");
            continue;

        }

       if (pid == 0){
        //child process

        //parsing the input into args[]
        char *args[64];
        parse_input(input, args);

        execvp(args[0], args);

        //if execvp fails, run perror and exit
        fprintf(stderr, "mysh: command not found: %s\n", args[0]);
        exit(1);

       }else{
        //parent process waits
        wait(NULL);
       }
        }


    
    return 0;
}
