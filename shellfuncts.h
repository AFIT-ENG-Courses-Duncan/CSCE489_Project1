/*************************************************************************************
 * shellfuncts.h - header file for your shell functions
 *
 *************************************************************************************/

#ifndef SHELLFUNCTS_H
#define SHELLFUNCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 256
#define MAX_ARGS 10

// Function to parse the command line and check for background execution
int parse_command(char *cmd_line, char **args, int *background);

// Functions for each shell command
void create_file(char **args);
void update_file(char **args);
void list_file(char **args);
void list_dir();

#endif // SHELLFUNCTS_H

