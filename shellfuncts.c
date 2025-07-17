/*************************************************************************************
 * shellfuncts.c - code definitions for your functions
 *
 *************************************************************************************/

#include <stdio.h>
#include "shellfuncts.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_ARGS 100
#define MAX_CMD_LEN 256

/*************************************************************************************
 * hello - sends hello world to the user! I'm doing proper function commenting so future
 *         coders might not find my code as painful.
 *
 *		Params:	param1 - I explain my parameters, like this is 1 for American, 2 for
 *                      Australian
 *
 *		Returns: always returns 1, no matter what. Fairly useless.
 *
 *************************************************************************************/

int hello(int param1) {
	// I'm commenting to explain that this checks param and changes the message
	if (param1 == 1)
		send_msg("Hello world!\n");
	else
		send_msg("G'day world!\n");

	// Return 1 because, why not.
	return 1;
}

/*************************************************************************************
 * hello - sends hello world to the user! I'm doing proper function commenting so future
 *         coders might not find my code as painful.
 *
 *    Params:  param2 - not a very good parameter name - something better might be
 *                      say, msgstr or sendtext
 *
 *************************************************************************************/

void send_msg(const char *param2) {
	printf("%s", param2);
}

int parse_command(char *cmd_line, char **args, int *background) {
    int i = 0;
    char *p = cmd_line;

    // Remove newline character
    p[strcspn(p, "\n")] = 0;

    while (*p && i < MAX_ARGS - 1) {
        // Skip leading whitespace
        while (*p && isspace((unsigned char)*p)) {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        if (*p == '"') {
            p++; // Skip the opening quote
            char *start = p;
            char *end = strchr(p, '"');
            if (end) {
                *end = '\0';
                args[i++] = strdup(start);
                p = end + 1;
            } else {
                // Unterminated quote, treat as a single token
                args[i++] = strdup(start);
                break; // No more tokens
            }
        } else {
            char *start = p;
            while (*p && !isspace((unsigned char)*p)) {
                p++;
            }
            if (isspace((unsigned char)*p)) {
                *p = '\0';
                p++;
            }
            args[i++] = strdup(start);
        }
    }
    args[i] = NULL;

    *background = 0;
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        *background = 1;
        free(args[i - 1]);
        args[i - 1] = NULL;
    }
    
    return i;
}

void create_file(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "create: missing file name\n");
        exit(1);
    }

    if (access(args[1], F_OK) == 0) {
        fprintf(stderr, "create: file '%s' already exists\n", args[1]);
        exit(1);
    }

    FILE *fp = fopen(args[1], "w");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    fclose(fp);
    exit(0);
}

void update_file(char **args) {
    if (args[1] == NULL || args[2] == NULL || args[3] == NULL) {
        fprintf(stderr, "update: missing arguments\n");
        exit(1);
    }

    char *filename = args[1];
    int num_lines = atoi(args[2]);
    char *text = args[3];

    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }

    for (int i = 0; i < num_lines; i++) {
        fprintf(fp, "%s\n", text);
        fflush(fp);
        sleep(strlen(text) / 5);
    }

    fclose(fp);
    printf("Update of '%s' completed by process %d\n", filename, getpid());
    exit(0);
}

void list_file(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "list: missing file name\n");
        exit(1);
    }

    if (access(args[1], F_OK) != 0) {
        fprintf(stderr, "list: cannot open file '%s'\n", args[1]);
        exit(1);
    }

    char command[MAX_CMD_LEN];
    snprintf(command, sizeof(command), "cat %s", args[1]);
    
    execlp("cat", "cat", args[1], NULL);
    perror("execlp"); // execlp only returns on error
    exit(1);
}

void list_dir() {
    execlp("ls", "ls", NULL);
    perror("execlp"); // execlp only returns on error
    exit(1);
}

