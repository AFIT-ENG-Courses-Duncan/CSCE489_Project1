/*************************************************************************************
 * myshell - Code that implements a simple shell-like program.
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shellfuncts.h"

int main() {
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

        if (strcmp(cmd_line, "\n") == 0) {
            continue; // Ignore empty lines
        }

        int arg_count = parse_command(cmd_line, args, &background);

        if (arg_count == 0) {
            continue; // No command entered
        }

        if (strcmp(args[0], "halt") == 0) {
            printf("Halting shell.\n");
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) { // Child process
            printf("Child process %d created to execute command: %s\n", getpid(), args[0]);

            if (strcmp(args[0], "create") == 0) {
                create_file(args);
            } else if (strcmp(args[0], "update") == 0) {
                update_file(args);
            } else if (strcmp(args[0], "list") == 0) {
                list_file(args);
            } else if (strcmp(args[0], "dir") == 0) {
                list_dir();
            } else {
                fprintf(stderr, "Unknown command: %s\n", args[0]);
                exit(1);
            }
            exit(0); // Should not be reached if exec works
        } else { // Parent process
            if (!background) {
                int status;
                waitpid(pid, &status, 0);
            }
            // For background processes, the parent continues immediately
        }
        
        // Free allocated memory for arguments
        for (int i = 0; i < arg_count; i++) {
            free(args[i]);
        }
    }

    return 0;
}
