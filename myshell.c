/*************************************************************************************
 * myshell - student code for Project 1 of CSCE 489 
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shellfuncts.h"

#define MAX_CMD_LEN 256
#define MAX_ARGS 10

void parse_command(char *cmd, char **args, int *background) {
    int i = 0;
    char *token;
    *background = 0;

    // Remove trailing newline
    cmd[strcspn(cmd, "\n")] = 0;

    // Check for background execution
    char *ampersand = strchr(cmd, '&');
    if (ampersand != NULL) {
        *background = 1;
        *ampersand = '\0'; // Remove '&' from command string
    }

    token = strtok(cmd, " \t");
    while (token != NULL && i < MAX_ARGS - 1) {
        if (token[0] == '"') { // Handle quoted text for update
            memmove(token, token + 1, strlen(token)); // Remove leading quote
            char *full_text = token;
            char *next_token = strtok(NULL, "\"");
            if(next_token) {
                strcat(full_text, " ");
                strcat(full_text, next_token);
            }
            
            char *end_quote = strrchr(full_text, '"');
            if (end_quote) {
                *end_quote = '\0';
            }
            args[i++] = full_text;
            token = strtok(NULL, " \t");
        } else {
            args[i++] = token;
            token = strtok(NULL, " \t");
        }
    }
    args[i] = NULL;
}


int main(void) {
    char cmd_line[MAX_CMD_LEN];
    char *args[MAX_ARGS];
    int background;

    printf("Main process started with PID: %d\n", getpid());

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (fgets(cmd_line, sizeof(cmd_line), stdin) == NULL) {
            break; // End of input
        }

        if (strlen(cmd_line) <= 1) { // Empty command
            continue;
        }

        char cmd_line_copy[MAX_CMD_LEN];
        strcpy(cmd_line_copy, cmd_line);
        parse_command(cmd_line_copy, args, &background);

        if (args[0] == NULL) {
            continue;
        }

        if (strcmp(args[0], "halt") == 0) {
            break;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) { // Child process
            printf("Child process %d created to execute command.\n", getpid());

			volatile int wait_for_debugger = 1;
            while(wait_for_debugger) {
                sleep(1); // wait for debugger to attach
            }

            if (strcmp(args[0], "create") == 0) {
                if (args[1]) create_cmd(args[1]);
                else fprintf(stderr, "Usage: create <name>\n");
            } else if (strcmp(args[0], "update") == 0) {
                if (args[1] && args[2] && args[3]) {
                    update_cmd(args[1], atoi(args[2]), args[3]);
                } else {
                    fprintf(stderr, "Usage: update <name> <number> \"<text>\"\n");
                }
            } else if (strcmp(args[0], "list") == 0) {
                if (args[1]) list_cmd(args[1]);
                else fprintf(stderr, "Usage: list <name>\n");
            } else if (strcmp(args[0], "dir") == 0) {
                dir_cmd();
            } else {
                fprintf(stderr, "Error: Unknown command '%s'\n", args[0]);
            }
            exit(1); // Exit child if command fails validation or completes
        } else { // Parent process
            if (!background) {
                waitpid(pid, NULL, 0);
            }
            // For background processes, reap zombies periodically or use signals
            while (waitpid(-1, NULL, WNOHANG) > 0);
        }
    }

    printf("Shell terminated.\n");
    return 0;
}

// Here is a new change that I want to add to the branch and then push to the cloud (remote).