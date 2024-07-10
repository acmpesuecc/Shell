#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void shell_loop(void);
char *shell_read_line(void);
char **shell_line_parse(char *line);
int shell_function(char **tokens);

char current_dir[1024] = "";

int main(int argc, char **argv)
{
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
        printf("\\%s:) ", current_dir);
        line = shell_read_line();
        args = shell_line_parse(line);
        status = shell_function(args);

        free(line);
        free(args);
    } while(status);
}

char *shell_read_line(void)
{
    int buffer_size = 1024;
    int i = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);

    if (!buffer)
    {
        fprintf(stderr, "shell: allocation error. Perhaps nothing is entered.\n");
        exit(EXIT_FAILURE);
    }

    while(1)
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
    char **tokens = malloc(sizeof(char*) * 64);
    char *token;
    int i = 0;

    if (!tokens)
    {
        printf("shell: allocation error. Perhaps nothing is entered.");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
    return tokens;
}

int shell_function(char **tokens)
{
    if (tokens[0] == NULL)
    {
        return 1;
    }

    if (strcmp(tokens[0], "bye") == 0)
    {
        return 0;
    }
    else if (strcmp(tokens[0], "cd") == 0)
    {
        if (tokens[1] == NULL)
        {
            printf("shell: expected a directory name after cd\n");
        }
        else if (strcmp(tokens[1], "..") == 0)
        {
            // char *updir = strtok(current_dir, "\\");
            chdir("..");
            char *last_slash = strrchr(current_dir, '\\');
            if (last_slash != NULL)
            {
                *last_slash = '\0';
            }
            else
            {
                strcpy(current_dir, "");
            }
        }
        else
        {
            if (chdir(tokens[1]) == 0)
            {
                printf("Changed directory to %s\n", tokens[1]);
                if (strlen(current_dir) == 0)
                {
                    strcpy(current_dir, tokens[1]);
                }
                else
                {
                    strcat(current_dir, "\\");
                    strcat(current_dir, tokens[1]);
                }
            }
            else
            {
                printf("Could not move to the given directory.\n");
            }
        }
        return 1;
    }
    else
    {
        printf("shell: command doesn't exist.\n");
        return 1;
    }
}