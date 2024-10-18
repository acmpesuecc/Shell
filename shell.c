#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <wordexp.h>
#include <signal.h>

char current_dir[1024] = "";
char history_file[1024];
void sigtstp_handler(int);

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

// ... (previous code remains the same)

int main(int argc, char** argv)
{
    printf("\nHello, this is Ameya's basic shell (apksh).\n\n");
    signal(SIGTSTP, sigtstp_handler);
    initialize_readline();
    shell_loop();
    cleanup_readline();
    return 0;
}

void initialize_readline(void)
{
    rl_bind_key('\t', rl_complete);
    
    // Set up history file path
    char* home = getenv("HOME");
    if (home != NULL) {
        snprintf(history_file, sizeof(history_file), "%s/.apksh_history", home);
    } else {
        strcpy(history_file, ".apksh_history");
    }
    
    using_history();
    read_history(history_file);
}

void cleanup_readline(void)
{
    write_history(history_file);
}

void shell_loop(void)
{
    char *line = NULL;
    char **args = NULL;
    int status = 1;

    do
    {
        getcwd(current_dir, sizeof(current_dir)); // Update current directory
        char prompt[1024];
        size_t prompt_max = sizeof(prompt) - 1; // Reserve space for null terminator
        size_t dir_length = strlen(current_dir);
        const char *suffix = " :) ";
        size_t suffix_length = strlen(suffix);
        
        if (dir_length + suffix_length > prompt_max) {
            // If the directory path is too long, truncate it
            size_t avail_space = prompt_max - suffix_length - 3; // 3 for "..."
            snprintf(prompt, avail_space + 1, "...%s", current_dir + dir_length - avail_space);
            strncat(prompt, suffix, suffix_length);
        } else {
            snprintf(prompt, dir_length + 1, "%s", current_dir);
            strncat(prompt, suffix, suffix_length);
        }
        
        line = readline(prompt);

        if (line == NULL) {
            printf("\n");
            break; // Exit on EOF (Ctrl+D)
        }

        if (strlen(line) > 0) {
            add_history(line);
        }

        args = shell_line_parse(line);
        status = shell_execute(args);

        free(line);
        free(args);
    } while (status);
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
        tokens[i] = strdup(token);
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
    
    // Handle tilde expansion
    wordexp_t exp_result;
    if (wordexp(directory, &exp_result, 0) != 0) {
        fprintf(stderr, "apksh: Error expanding path\n");
        return 1;
    }
    
    directory = exp_result.we_wordv[0];
    
    DIR *dir = opendir(directory);
    
    if (dir == NULL)
    {
        perror("apksh");
        wordfree(&exp_result);
        return 1;
    }
    
    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
    wordfree(&exp_result);
    return 1;
}


int shell_make_dir(char** args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "apksh: No argument given. Usage: mkdir <directory>");
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
            return builtins[i].func(args);
        }
    }
    
    printf("apksh: '%s' is not a recognized built-in command.\n", args[0]);
    return 1;
}

int shell_roll(void)
{
    printf("⡀⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠉⠁⠀⣀⣀⠀⣀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⡀⠀⠀⠀\n"   "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣛⣿⣿⣷⣿⣿⣯⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣏⠉⣿⠉⢹⠟⢉⠉⢻⣿⠉⢻⠋⠙⡏⣿⠋⢻⡏⠉⣿⠉⣉⣻⠀\n" "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣻⡀⠈⢀⣿⠀⢸⠀⠀⣿⠀⢸⠀⠰⣿⣿⠀⢸⠁⢀⡟⠀⢹⣿⠀\n" "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⡿⠿⠿⢿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣧⠀⣼⣿⠀⢸⡀⠀⣏⠀⢸⠀⠀⣿⣿⡄⠘⠀⢸⡇⠀⢰⣾⠀\n"           "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⡿⠋⠀⠀⠀⠀⠀⠈⠉⠉⠁⠀⠀⠈⢻⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣀⣿⣿⣆⡈⢁⣰⣿⣄⠘⢀⣼⣿⣿⣇⣀⣀⣼⣧⣀⣈⣹⡇\n"           "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⣿⣿⣿⣿⣿⣿⠟⠿⢿⣿⠿⠛⠛⠻⠿⠿⠻⠛⠉⠉⠉⠀\n"           "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠀⠀⢀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣧⣄⠀⠀⠀⠀⠀⠀⣴⠶⡶⠿⣿⣿⠿⠿⢿⡿⠿⠿⣿⠿⢿⡿⢿⡿⠀⠀⠀⠀⠀⠀⠀\n"           "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⣿⣿⠀⠀⢨⣭⣽⣿⡇⠀⢠⣾⣿⣿⣷⣆⠀⠀⢸⣿⠀⠀⠀⠀⠀⠀⣿⠀⢱⡆⠈⣿⠀⢴⣾⡇⠀⣶⣿⠀⠘⡇⠀⡇⠀⠀⠀⠀⠀⠀⠀\n"           "⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⠉⠛⠀⠀⠀⠀⠉⠁⠀⠀⠘⡏⠉⠉⠛⠋⠀⣠⣼⣿⠀⠀⠀⠀⠀⠀⣿⠀⢨⡁⠺⣿⠀⣈⣹⡇⠀⣉⣿⠀⡀⠁⠀⡇⠀⠀⠀⠀⠀⠀⠀\n"           "⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⢹⡄⠀⠀⠀⠀⣿⣿⡿⠀⠀⠀⠀⠀⠀⣿⠀⠸⠇⠀⣿⠀⠹⢿⡇⠀⠿⢿⠀⢸⡀⠀⡇⠀⠀⠀⠀⠀⠀⠀\n"           "⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⢷⣄⡀⠀⢠⡾⠋⠀⠛⢶⣶⣾⡇⠀⣠⠄⢰⣿⠟⠀⠀⠀⠀⠀⠀⠀⠻⢶⣶⡶⠚⠓⠶⠶⠾⠷⠶⠶⠾⠶⠾⠳⠾⠟⠀⠀⠀⠀⠀⠀⠀\n"           "⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣹⡷⣠⠏⠙⢷⣶⠲⠶⠶⣷⣶⡿⠋⢀⣾⠃⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣹⣧⡀⢀⠀⠀⣀⣀⣀⡀⢀⣀⣀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠟⣫⣽⠃⠀⠀⠀⠉⠉⠙⠛⠋⠀⠀⢀⣾⡃⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠉⢉⡉⠻⡏⠉⣿⠟⢉⡉⠙⣿⠉⢹⡏⢉⡿⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠛⠁⠀⣼⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⡏⢳⡄⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠸⠇⣰⡇⠀⣿⠀⢸⣧⣀⣿⠀⠈⠀⣼⠁⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⢀⣴⠏⠀⠀⠀⢸⣿⣿⡀⠀⠀⠰⣦⣄⡀⣀⣤⡾⣿⣿⣧⠀⠻⢦⣄⡀⠀⠀⠀⠀⠀⣿⠀⢸⠀⠈⡇⠀⣿⠀⢸⡟⠛⣿⠀⢠⠀⢹⣆⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⠘⠁⠀⠀⠀⠀⣾⣿⣿⣷⣄⡀⠀⠙⠿⣿⣏⣽⣿⣿⣿⣿⠄⢸⣧⠈⠙⠶⣤⣀⠀⠀⣿⣀⣸⣄⣠⣷⣀⣿⣦⣀⣁⣠⣿⣀⣸⣧⣀⣿⠀⠀⠀⠀⠀⠀⠀\n"           "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣿⣿⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⠀⠀⠹⣆⠀⠀⠀⠉⠳⣦⡀⠉⠉⠙⠻⣿⠉⠁⠀⠉⠉⠀⠀⠈⠉⠀⠉⠹⠇⠀⠀⠀⠀⠀⠀\n"           "⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⢿⡆⠀⠀⠀⠀⠻⣿⠓⠒⠲⢦⣹⠷⠒⠲⣶⡖⠒⣶⣶⠒⣶⠒⠶⠒⠛⠀⠀⠀⠀⠀\n""⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⠀⢸⣧⠀⠀⠀⠀⠀⢻⡀⠀⠀⠈⠙⠀⠀⠀⠻⠇⠀⠻⠿⠐⠻⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    return 1;
}

int shell_roll_wrapper(char** args)
{
    return shell_roll();
}

void sigtstp_handler(int sig) {
    printf("\nExiting the shell. Goodbye!\n");
    exit(0); 
}
