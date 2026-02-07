#ifndef SHELL_H
#define SHELL_H

// Temel fonksiyonlar.
char* read_line_func();
char** split_line_func(char* line);
int launch_func(char** args);
int execute_func(char** args);

// Built-in fonksiyonlar.
int cd_func(char** args);
int help_func(char** args);
int exit_func(char** args);
int num_builtins_func();

#endif // SHELL_H

