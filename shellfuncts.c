/*************************************************************************************
 * shellfuncts.c - code definitions for your functions
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shellfuncts.h"

/*************************************************************************************
 * create_cmd - creates a new file.
 *
 *		Params:	filename - The name of the file to create.
 *************************************************************************************/
void create_cmd(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        fclose(fp);
        fprintf(stderr, "Error: File '%s' already exists.\n", filename);
        exit(1);
    }

    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    fclose(fp);
    exit(0);
}

/*************************************************************************************
 * update_cmd - appends lines of text to a file.
 *
 *		Params:	filename - The name of the file to update.
 *              num - The number of times to append the text.
 *              text - The text to append.
 *************************************************************************************/
void update_cmd(char *filename, int num, char *text) {
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }

    for (int i = 0; i < num; i++) {
        fprintf(fp, "%s\n", text);
        fflush(fp);
        sleep(strlen(text) / 5);
    }

    fclose(fp);
    printf("Update of '%s' completed by process %d.\n", filename, getpid());
    exit(0);
}

/*************************************************************************************
 * list_cmd - displays the contents of a file.
 *
 *		Params:	filename - The name of the file to list.
 *************************************************************************************/
void list_cmd(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: File '%s' does not exist.\n", filename);
        exit(1);
    }
    fclose(fp);

    execlp("cat", "cat", filename, NULL);
    // If execlp returns, it must have failed.
    perror("execlp");
    exit(1);
}

/*************************************************************************************
 * dir_cmd - lists the files in the current directory.
 *************************************************************************************/
void dir_cmd() {
    execlp("ls", "ls", NULL);
    // If execlp returns, it must have failed.
    perror("execlp");
    exit(1);
}

