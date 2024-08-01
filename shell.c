#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Function declarations
void shell_loop(void);
char *shell_read_line(void);
char **shell_line_parse(char *line);
int shell_launch(char **args);
int shell_execute(char **args);

// Global current directory variable
char current_dir[1024] = "";

// Built-in shell commands
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit_command(char **args);

int main(int argc, char **argv)
{
    printf("\nHello, this is Ameya's basic shell.\n\n");
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
        free(args);
    } while (status);
}

char *shell_read_line(void)
{
    int buffer_size = 1024;
    int i = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);

    if (!buffer)
    {
        fprintf(stderr, "shell: allocation error.\n");
        exit(EXIT_FAILURE);
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
    }
}

char **shell_line_parse(char *line)
{
    int buffer_size = 64;
    int i = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "shell: allocation error.\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[i] = token;
        i++;

        if (i >= buffer_size)
        {
            buffer_size += 64;
            tokens = realloc(tokens, buffer_size * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "shell: allocation error.\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
    return tokens;
}

int shell_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("shell");
        }
    }
    return 1;
}

int shell_help(char **args)
{
    printf("Ameya's Shell\n");
    printf("The following are built-in:\n");

    printf("  cd\n");
    printf("  help\n");
    printf("  bye\n");

    return 1;
}

int shell_exit_command(char **args)
{
    return 0;
}

int shell_execute(char **args)
{
    if (args[0] == NULL)
    {
        return 1; // An empty command was entered.
    }

    if (strcmp(args[0], "cd") == 0)
    {
        return shell_cd(args);
    }
    else if (strcmp(args[0], "help") == 0)
    {
        return shell_help(args);
    }
    else if (strcmp(args[0], "bye") == 0)
    {
        return shell_exit_command(args);
    }

    return shell_launch(args);
}

int shell_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("shell");
    }
    else
    {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
