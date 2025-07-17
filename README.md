# Project Overview

In this project, you will be creating a simple Unix shell that executes a limited subset of commands. This will require leveraging a number of system and API functions, including fork(), wait(), getpid(), sleep(), system(), exit(), and several file I/O functions (fopen, fclose, fprintf, and fflush). You will also use some of the string manipulation functions in C. The function descriptions are available in man pages and widely-available on the internet. 

Project Description
You are going to implement a shell for a very simple operating system. In fact, the only commands that the shell can process are the following:

create <name>	Create a new file in the current directory with the given name. If the file already exists, print an error message on the workstation screen.
update <name> <number> <text>	Append lines of text to the named file. In particular, the text string <text> is appended to the end of the file, <number> times. To simulate a (very) slow device, the update command sleeps for several seconds after appending each line.
list <name>	Display the contents of the named file on the workstation screen. If the file does not exist, print an error message on the workstation screen and terminate the command.
dir	List the names of all files in the current directory. Note that since there is no command to change to a new directory, the current directory is the only directory.
halt	Terminate the shell
Each command is entered on one line with no preceding blanks and with one or more blanks between the parameters for the command, if any. Note that command names are lower case. A file name may be any valid UNIX file name (within the current directory). You should do necessary error checking of commands that are entered so that invalid commands are detected and ignored (after printing an appropriate message). Each command ends with a newline character (i.e., \n). The <text> in the update command is any string of characters in double quotes (i.e., "text").

Like UNIX, your shell executes commands in the foreground by default. This means that it waits for the command to finish before prompting the user for another command. However, also like UNIX, your shell should allow the user to specify that the command is to be executed in the background by placing an ampersand (i.e., &) at the end of the command line (preceded by one or more blanks). In this case the shell immediately prompts the user for another command without waiting for the current command to complete execution.

The implementation of your shell will mirror the implementation of shells in UNIX. That is, the shell will be your main process and you will spawn a new process to execute each command that the user enters. If the command is to be processed in the foreground, then your main process waits for the child process executing the command to complete before prompting for the next command. If the command is to be processed in the background, then your main process immediately prompts for a new command after spawning the new process to execute the current command. To help you debug your program, and to help us verify that your program works correctly, you should print out the process id of your main process when it begins, and then print out the process id of each new child process that you create to execute commands. Hence, the algorithm for your main process is the following:

use getpid() to get and print the process id for this process loop
prompt user for next command
parse command to determine what it is if command is halt, then terminate
fork() a child process to execute command
print the process id of the child process
if the command is to be executed in the foreground then wait for it to complete
end loop
The algorithms for processing each of the commands is shown below:

create <name>

use fopen() to create the file
use fclose() to close the file exit(0)
 

update <name> <number> <text>

use fopen() to open file <name> for append (i.e., mode "a")
loop <number> times
use fprintf() to append the line <text> to the end of the file
use fflush() to force the file write to disk
use sleep(strlen( <text>/5) to suspend the process for a few seconds
use fclose() to close the file
print a message on the workstation screen saying that the update has completed (include the process id of this child process)
exit(0)
 

list <name>

use fopen() to open file <name> for reading (i.e., mode "r")
if the file doesn't exist
then print an error message, and terminate the child process
use fclose() to close the file
construct cat command using snprintf(command, bufsize, "cat %s", <name>)
execute cat command using execl(command, "cat", <name>)
exit(0)
 

dir

use execl("/bin/ls", "ls", NULL) to list files in current directory
exit(0)
Minimum Standards
These are the minimum standards that must be met before the project can be submitted. 

Code compiles with no errors (warnings will cause point deduction)
create - creates a new file
update - writes to the file at least some of the data provided
list - lists the contents of the file
dir - lists the files in the current directory
halt - exits the shell
Things to Think About
One of the things that your program will have to do each time through the main loop in the main process is to decide which type of command has been entered. You might decide to do this before spawning a child process, or you might decide to do this in the child process. Either way will work.

After you run your program, make sure that all processes that it creates are deleted. This should only be a problem if your program terminates abnormally or if a background job was still active when the main process terminates (e.g., you forget to terminate it by using exit(0)). Use the UNIX ps command (command line, not in your code) to see if you have any leftover processes. If you do, use the kill command to get rid of them.

After you run your program there will be several files that it created in your current UNIX directory. You should delete these with the UNIX rm command before running your program again.

Sample Input

The sample input below is a sequence of correctly formatted commands. Your program should detect and ignore commands that are not correctly formatted.

dir
create file1
dir
update file1 3 “this is a line”
list file1
update file1 2 “this is another line”
list file1
create file2
update file2 10 “this is the first 10 lines” &
list file2
create file3
dir
list file2
update file3 10 “first update” &
update file3 12 “second update”
list file3
list file4
dir
halt
