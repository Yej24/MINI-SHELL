#ifndef PARSER_H
#define PARSER_H

char ***parse_pipeline(char *input, int *num_cmds);
void free_commands(char ***commands, int num_cmds);

#endif
