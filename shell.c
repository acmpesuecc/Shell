#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>


void shell_loop(void);
char *shell_read_line(void);
char **shell_line_parse(char *line);
int shell_function(char **tokens);

int main(int argc, char **argv)
{
    
    shell_loop();


}

void shell_loop(void)
{
    char *line = NULL;
    char **args = NULL;
    int status = 1;

    do
    {
        printf(":) ");
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
    if (strcmp(tokens[0], "bye") == 0)
    {
        return 0;
    }
    else if (strcmp(tokens[0], "cd") == 0)
    {
        if (tokens[1] == NULL)
        {
            printf("shell: expected an directory name after cd\n");
        }
        else
        {
            #ifdef _WIN32
            if (SetCurrentDirectory(tokens[1]) != 0)
            {
                printf("Directory changed to %s folder.\n\n\\%s",tokens[1], tokens[1]);
            }
            else
            {
                printf("Could not move to the given directory.\n");
                return 1;
            }
            #else
            #include <unistd.h>
            if (chdir(tokens[1]) != 0)
            {
                printf("Directory changed to %s folder.\n\n\\%s",tokens[1], tokens[1]);
            }
            else
            {
                printf("Could not move to the given directory.\n");
                return 1;
            }
            #endif
        }
    }
}