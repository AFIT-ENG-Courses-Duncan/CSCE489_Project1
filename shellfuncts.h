/*************************************************************************************
 * shellfuncts.h - header file for your shell functions
 *
 *************************************************************************************/

#ifndef SHELLFUNCTS_H
#define SHELLFUNCTS_H

#include <unistd.h> // For ssize_t

// Function prototypes for shell commands
void create_cmd(char *filename);
void update_cmd(char *filename, int num, char *text);
void list_cmd(char *filename);
void dir_cmd();

#endif // SHELLFUNCTS_H

