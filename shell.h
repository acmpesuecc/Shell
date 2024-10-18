// shell.h

#ifndef SHELL_H
#define SHELL_H

#define NUM_BUILTINS 8

void shell_loop(void);
char *shell_read_line(void);
char **shell_line_parse(char* line);
int shell_launch(char** args);
int shell_execute(char** args);
void shell_handle_error(const char* message);
int shell_man(char** args);
int shell_list(char** args);
int shell_make_dir(char** args);

// Global current directory variable
extern char current_dir[1024];

// Built-in shell commands
int shell_cd(char** args);
int shell_help(char** args);
int shell_pwd(char** args);
int shell_echo(char** args);
int shell_exit_command(char** args);
int shell_roll_wrapper(char** args);

// Command structure for built-in commands
typedef struct {
    char *name;
    int (*func)(char **args);
    char *description;
} builtin_command;

extern builtin_command builtins[NUM_BUILTINS];

#endif
