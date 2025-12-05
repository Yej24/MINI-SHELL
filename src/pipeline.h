#ifndef PIPELINE_H
#define PIPELINE_H

// Parse input line into array of commands (char ***)
// num_cmds is set to number of commands found
char ***parse_pipeline(char *input, int *num_cmds);

// Free the array of commands
void free_commands(char ***commands, int num_cmds);

#endif
