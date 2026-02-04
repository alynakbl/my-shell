#ifndef SHELL_H
#define SHELL_H

char* read_line_func();
char** split_line_func(char* line);
int execute_func(char** args);

#endif SHELL_H

