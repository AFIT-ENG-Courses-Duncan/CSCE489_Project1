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
    char *token;
    char *cmd_copy = strdup(cmd_line);
    
    // Remove newline character
    cmd_copy[strcspn(cmd_copy, "\n")] = 0;

    token = strtok(cmd_copy, " \t\n");
    while (token != NULL && i < MAX_ARGS) {
        args[i++] = strdup(token);
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;

    *background = 0;
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        *background = 1;
        args[i - 1] = NULL;
    }
    
    free(cmd_copy);
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

    // Remove quotes from text
    if (text[0] == '"') {
        text++;
    }
    size_t len = strlen(text);
    if (len > 0 && text[len - 1] == '"') {
        text[len - 1] = '\0';
    }


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

    FILE *fp = fopen(args[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "list: cannot open file '%s'\n", args[1]);
        exit(1);
    }
    fclose(fp);

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

