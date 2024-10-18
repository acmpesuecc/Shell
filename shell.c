#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "shell.h"
#include <dirent.h>

char current_dir[1024] = "";

builtin_command builtins[NUM_BUILTINS] = {
    {"cd", shell_cd, "Change the current directory. Usage: cd <directory>"},
    {"help", shell_help, "Display this help message. Usage: help"},
    {"pwd", shell_pwd, "Print the current working directory. Usage: pwd"},
    {"echo", shell_echo, "Print arguments to the console. Usage: echo <text>"},
    {"bye", shell_exit_command, "Exit the shell. Usage: bye"},
    {"exit", shell_exit_command, "Exit the shell. Usage: exit"},
    {"ls", shell_list, "List the contents in directory. Usage: ls <directory>"},
    {"roll", shell_roll_wrapper, "Rolls the terminal. Usage: roll"}
};

int main(int argc, char** argv)
{
    printf("\nHello, this is Ameya's basic shell (apksh).\n\n");
    shell_loop();
    return 0;
}

void shell_loop(void)
{
    char *line = NULL;
    char **args = NULL;
    int status = 1;

    do
    {
        getcwd(current_dir, sizeof(current_dir)); // Update current directory
        printf("%s :) ", current_dir);
        line = shell_read_line();
        args = shell_line_parse(line);
        status = shell_execute(args);

        free(line);
        free(args);  // Free the args array after execution.
    } while (status);
}

char *shell_read_line(void)
{
    int buffer_size = 1024;
    int i = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);

    if (!buffer)
    {
        shell_handle_error("allocation error");
    }

    while (1)
    {
        int c = getchar();
        if (c == EOF || c == '\n')
        {
            buffer[i] = '\0';
            return buffer;
        }
        else
        {
            buffer[i] = c;
        }
        i++;

        if (i >= buffer_size)
        {
            buffer_size += 1024;
            buffer = realloc(buffer, buffer_size);
            if (!buffer)
            {
                shell_handle_error("allocation error");
            }
        }
    }
}

char **shell_line_parse(char* line)
{
    int buffer_size = 64;
    int i = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;

    if (!tokens)
    {
        shell_handle_error("allocation error");
    }

    token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[i] = strdup(token);  // strdup allocates memory for each token
        if (!tokens[i])
        {
            shell_handle_error("allocation error");
        }
        i++;

        if (i >= buffer_size)
        {
            buffer_size += 64;
            tokens = realloc(tokens, buffer_size * sizeof(char *));
            if (!tokens)
            {
                shell_handle_error("allocation error");
            }
        }

        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
    return tokens;
}

void shell_handle_error(const char *message)
{
    fprintf(stderr, "apksh: %s\n", message);
    exit(EXIT_FAILURE);
}

int shell_cd(char** args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "apksh: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("apksh");
        }
    }
    return 1;
}

int shell_help(char** args)
{
    printf("Ameya's Shell (apksh)\n");
    printf("The following are built-in:\n");

    for (int i = 0; i < NUM_BUILTINS; i++)
    {
        printf("  %s - %s\n", builtins[i].name, builtins[i].description);
    }
    return 1;
}

int shell_list(char** args)
{
    char* directory = args[1] ? args[1] : ".";
    DIR *dir = opendir(directory);
    
    if (dir == NULL)
    {
        perror("apksh: ");
        return 1;
    }
    
    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
    return 1;
}

int shell_make_dir(char** args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "apksh: No arguement given. Usage: mkdir <directory>");
    return 1;
  }

  if (mkdir(args[1], 0755) != 0)
  {
    perror("apksh");
  }
  return 1;
}

int shell_pwd(char** args)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("apksh");
    }
    return 1;
}

int shell_echo(char** args)
{
    for (int i = 1; args[i] != NULL; i++)
    {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 1;
}

int shell_exit_command(char** args)
{
    return 0;
}

int shell_man(char** args)
{
    if (args[1] == NULL)
    {
        printf("Usage: man <command>\n");
        printf("Available commands:\n");

        for (int i = 0; i < NUM_BUILTINS; i++)
        {
            printf("  %s\n", builtins[i].name);
        }

        return 1;
    }

    for (int i = 0; i < NUM_BUILTINS; i++)
    {
        if (strcmp(args[1], builtins[i].name) == 0)
        {
            printf("%s - %s\n", builtins[i].name, builtins[i].description);
            return 1;
        }
    }

    printf("apksh: no manual entry for %s\n", args[1]);
    return 1;
}

int shell_execute(char** args)
{
    if (args[0] == NULL)
    {
        return 1; // An empty command was entered.
    }

    if (strcmp(args[0], "man") == 0)
    {
        return shell_man(args);
    }

    for (int i = 0; i < NUM_BUILTINS; i++)
    {
        if (strcmp(args[0], builtins[i].name) == 0)
        {
            int result = builtins[i].func(args);
            
            // Freeing args in shell_loop after execution
            for (int j = 0; args[j] != NULL; j++)
            {
                free(args[j]);  // Free each token inside the args array.
            }
            return result;
        }
    }

    printf("apksh: '%s' is not a recognized built-in command.\n", args[0]);
    return 1;
}

// int shell_launch(char** args)
// {
//     pid_t pid, wpid;
//     int status;

//     pid = fork();
//     if (pid == 0)
//     {
//         if (execvp(args[0], args) == -1)                                     //to make it such that external commands work if they dont exist in the builtin commands of apksh
//         {
//             fprintf(stderr, "apksh: Such a command doesn't exist.\n");
//         }
//         exit(EXIT_FAILURE);
//     }
//     else if (pid < 0)
//     {
//         perror("apksh");
//     }
//     else
//     {
//         do
//         {
//             wpid = waitpid(pid, &status, WUNTRACED);
//         } while (!WIFEXITED(status) && !WIFSIGNALED(status));
//     }

//     return 1;
// }

int shell_roll(void)
{
    printf("⡀⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠉⠁⠀⣀⣀⠀⣀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⡀⠀⠀⠀\n"   "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣛⣿⣿⣷⣿⣿⣯⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣏⠉⣿⠉⢹⠟⢉⠉⢻⣿⠉⢻⠋⠙⡏⣿⠋⢻⡏⠉⣿⠉⣉⣻⠀\n" "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣻⡀⠈⢀⣿⠀⢸⠀⠀⣿⠀⢸⠀⠰⣿⣿⠀⢸⠁⢀⡟⠀⢹⣿⠀\n" "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⡿⠿⠿⢿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣧⠀⣼⣿⠀⢸⡀⠀⣏⠀⢸⠀⠀⣿⣿⡄⠘⠀⢸⡇⠀⢰⣾⠀\n"           "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⡿⠋⠀⠀⠀⠀⠀⠈⠉⠉⠁⠀⠀⠈⢻⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣀⣿⣿⣆⡈⢁⣰⣿⣄⠘⢀⣼⣿⣿⣇⣀⣀⣼⣧⣀⣈⣹⡇\n"           "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⣿⣿⣿⣿⣿⣿⠟⠿⢿⣿⠿⠛⠛⠻⠿⠿⠻⠛⠉⠉⠉⠀\n"           "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠀⠀⢀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣧⣄⠀⠀⠀⠀⠀⠀⣴⠶⡶⠿⣿⣿⠿⠿⢿⡿⠿⠿⣿⠿⢿⡿⢿⡿⠀⠀⠀⠀⠀⠀⠀\n"           "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⣿⣿⠀⠀⢨⣭⣽⣿⡇⠀⢠⣾⣿⣿⣷⣆⠀⠀⢸⣿⠀⠀⠀⠀⠀⠀⣿⠀⢱⡆⠈⣿⠀⢴⣾⡇⠀⣶⣿⠀⠘⡇⠀⡇⠀⠀⠀⠀⠀⠀⠀\n"           "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⠉⠛⠀⠀⠀⠀⠉⠁⠀⠀⠘⡏⠉⠉⠛⠋⠀⣠⣼⣿⠀⠀⠀⠀⠀⠀⣿⠀⢨⡁⠺⣿⠀⣈⣹⡇⠀⣉⣿⠀⡀⠁⠀⡇⠀⠀⠀⠀⠀⠀⠀\n"           "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⢹⡄⠀⠀⠀⠀⣿⣿⡿⠀⠀⠀⠀⠀⠀⣿⠀⠸⠇⠀⣿⠀⠹⢿⡇⠀⠿⢿⠀⢸⡀⠀⡇⠀⠀⠀⠀⠀⠀⠀\n"           "⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⢷⣄⡀⠀⢠⡾⠋⠀⠛⢶⣶⣾⡇⠀⣠⠄⢰⣿⠟⠀⠀⠀⠀⠀⠀⠀⠻⢶⣶⡶⠚⠓⠶⠶⠾⠷⠶⠶⠾⠶⠾⠳⠾⠟⠀⠀⠀⠀⠀⠀⠀\n"           "⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣹⡷⣠⠏⠙⢷⣶⠲⠶⠶⣷⣶⡿⠋⢀⣾⠃⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣹⣧⡀⢀⠀⠀⣀⣀⣀⡀⢀⣀⣀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠟⣫⣽⠃⠀⠀⠀⠉⠉⠙⠛⠋⠀⠀⢀⣾⡃⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠉⢉⡉⠻⡏⠉⣿⠟⢉⡉⠙⣿⠉⢹⡏⢉⡿⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠛⠁⠀⣼⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⡏⢳⡄⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠸⠇⣰⡇⠀⣿⠀⢸⣧⣀⣿⠀⠈⠀⣼⠁⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⢀⣴⠏⠀⠀⠀⢸⣿⣿⡀⠀⠀⠰⣦⣄⡀⣀⣤⡾⣿⣿⣧⠀⠻⢦⣄⡀⠀⠀⠀⠀⠀⣿⠀⢸⠀⠈⡇⠀⣿⠀⢸⡟⠛⣿⠀⢠⠀⢹⣆⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⠘⠁⠀⠀⠀⠀⣾⣿⣿⣷⣄⡀⠀⠙⠿⣿⣏⣽⣿⣿⣿⣿⠄⢸⣧⠈⠙⠶⣤⣀⠀⠀⣿⣀⣸⣄⣠⣷⣀⣿⣦⣀⣁⣠⣿⣀⣸⣧⣀⣿⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣿⣿⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⠀⠀⠹⣆⠀⠀⠀⠉⠳⣦⡀⠉⠉⠙⠻⣿⠉⠁⠀⠉⠉⠀⠀⠈⠉⠀⠉⠹⠇⠀⠀⠀⠀⠀⠀\n"           "⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⢿⡆⠀⠀⠀⠀⠻⣿⠓⠒⠲⢦⣹⠷⠒⠲⣶⡖⠒⣶⣶⠒⣶⠒⠶⠒⠛⠀⠀⠀⠀⠀\n""⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⠀⢸⣧⠀⠀⠀⠀⠀⢻⡀⠀⠀⠈⠙⠀⠀⠀⠻⠇⠀⠻⠿⠐⠻⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    return 1;
}

int shell_roll_wrapper(char** args)
{
    return shell_roll();
}
