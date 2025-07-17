#!/bin/bash

#
# CSCE489_Project1 Test Script
#
# This script tests the `myshell` executable by running a predefined set of
# commands and comparing the resulting output against a known-good pattern.
# It handles dynamic PIDs and variable 'dir' output to provide a reliable
# test result.
#

# --- Configuration ---
# Colors for PASS/FAIL messages to make output easier to read.
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "===== CSCE489 Project 1 Test Script ====="

# --- 1. Cleanup and Compilation ---
echo
echo "--- Cleaning up previous runs and compiling project ---"

# Clean up old files to ensure a clean test environment.
make -f Makefile.mk clean > /dev/null 2>&1
rm -f testfile*.txt actual_output.txt expected_output.txt normalized_output.txt diff_report.txt

# Compile the project using the provided Makefile.mk.
make -f Makefile.mk > /dev/null 2>&1
if [ ! -f myshell ]; then
    echo -e "${RED}FAIL: Compilation failed. 'myshell' executable not found.${NC}"
    # Rerun make without hiding output to show the user the compilation error.
    make -f Makefile.mk
    exit 1
fi
echo "Compilation successful."
echo

# --- 2. Define Commands and Expected Output Pattern ---

# A 'here document' defines the sequence of commands to pipe into the shell.
# This sequence is deterministic (no background '&' tasks).
COMMANDS=$(cat <<'EOF'
dir
create testfile1.txt
create testfile1.txt
update testfile1.txt 3 "Hello World!"
list testfile1.txt
dir
create testfile2.txt
dir
list non_existent_file.txt
halt
EOF
)

# A 'here document' defines the known-good output pattern.
# - PIDs are replaced with a static placeholder '[PID]'.
# - The 'dir' command output is shown sorted and contains only test files.
# cat > expected_output.txt <<'EOF'
# Main process started with PID: [PID]
# myshell> Child process [PID] created to execute command.
# actual_output.txt  diff_report.txt  expected_output.txt  Makefile  myshell  myshell.c  myshell.o  normalized_output.txt  Notes  Prompt.txt  README.md  shellfuncts.c  shellfuncts.h  shellfuncts.o  testfile1.txt  testfile2.txt  test_project.sh
# myshell> Child process [PID] created to execute command.
# myshell> Child process [PID] created to execute command.
# Error: File 'testfile1.txt' already exists.
# myshell> Child process [PID] created to execute command.
# Update of 'testfile1.txt' completed by process [PID].
# myshell> Child process [PID] created to execute command.
# Hello World!
# Hello World!
# Hello World!
# myshell> Child process [PID] created to execute command.
# actual_output.txt  diff_report.txt  expected_output.txt  Makefile  myshell  myshell.c  myshell.o  normalized_output.txt  Notes  Prompt.txt  README.md  shellfuncts.c  shellfuncts.h  shellfuncts.o  testfile1.txt  testfile2.txt  test_project.sh
# myshell> Child process [PID] created to execute command.
# myshell> Child process [PID] created to execute command.
# actual_output.txt  diff_report.txt  expected_output.txt  Makefile  myshell  myshell.c  myshell.o  normalized_output.txt  Notes  Prompt.txt  README.md  shellfuncts.c  shellfuncts.h  shellfuncts.o  testfile1.txt  testfile2.txt  test_project.sh
# myshell> Child process [PID] created to execute command.
# Error: File 'non_existent_file.txt' does not exist.
# myshell> Shell terminated.
# EOF
cat > expected_output.txt <<'EOF'
Main process started with process [PID]
myshell> actual_output.txt
expected_output.txt
Makefile
myshell
myshell.c
myshell.o
Notes
Prompt.txt
README.md
shellfuncts.c
shellfuncts.h
shellfuncts.o
test_project.sh
textfile1.txt
myshell> Child process [PID] created to execute command.
myshell> Error: File 'testfile1.txt' already exists.
Child process [PID] created to execute command.
myshell> Child process [PID] created to execute command.
Update of 'testfile1.txt' completed by process [PID].
myshell> Hello World!
Hello World!
Hello World!
myshell> actual_output.txt
expected_output.txt
Makefile
myshell
myshell.c
myshell.o
Notes
Prompt.txt
README.md
shellfuncts.c
shellfuncts.h
shellfuncts.o
testfile1.txt
test_project.sh
textfile1.txt
myshell> Child process [PID] created to execute command.
myshell> actual_output.txt
expected_output.txt
Makefile
myshell
myshell.c
myshell.o
Notes
Prompt.txt
README.md
shellfuncts.c
shellfuncts.h
shellfuncts.o
testfile1.txt
testfile2.txt
test_project.sh
textfile1.txt
myshell> Error: File 'non_existent_file.txt' does not exist.
Child process [PID] created to execute command.
myshell> Shell terminated.
EOF

# --- 3. Run the Test and Compare Output ---
echo "--- Running test commands and capturing output ---"

# Pipe the commands to myshell and capture all output (stdout and stderr).
echo "$COMMANDS" | ./myshell > actual_output.txt 2>&1

echo "--- Comparing actual output against expected pattern ---"

# Normalize the actual output for a stable comparison. This is the key step.
# 1. Use awk to handle the 'dir' output. When 'dir' is seen, it enters a state
#    where it only prints lines matching our test files until the next prompt.
# 2. Use sed to replace all dynamic PIDs with the static placeholder '[PID]'.
awk '/myshell> dir/{p=1;print;next} /myshell>/{p=0} p{if(/testfile1\.txt|testfile2\.txt/)print;next} 1' actual_output.txt | \
    sed -E 's/(PID: |[Pp]rocess )[0-9]+/process [PID]/g' | \
    sed -E 's/completed by process \[PID\]/completed by process [PID]/' > normalized_output.txt

# --- 4. Report Results ---
# Use 'diff' to compare the normalized output with the expected pattern.
diff -wB normalized_output.txt expected_output.txt > diff_report.txt

if [ $? -eq 0 ]; then
    echo -e "${GREEN}PASS: The output perfectly matches the expected pattern.${NC}"
    # (Optional) Display a summary of the test:
    # echo
    # echo "Test Summary:"
    # echo "  Commands executed: "
    # echo "$COMMANDS" | sed 's/^/    /' 
    # echo

    # Clean up temporary files on success
    rm -f testfile*.txt actual_output.txt expected_output.txt normalized_output.txt diff_report.txt
else
    echo -e "${RED}FAIL: The output does not match the expected pattern.${NC}"
    echo "--------------------------- DIFF REPORT (Expected vs. Yours) ---------------------------"
    # Filter diff_report to exclude lines related to 'dir' command output
    grep -v "^\(< \|> \)testfile[12]\.txt" diff_report.txt
    echo "------------------------------------------------------------------------------------"

    # Display relevant sections of actual output (not implemented in this example, but as a suggestion):
    # To implement this, you would need to parse diff_report.txt, identify the line numbers
    # with differences, and extract those lines (plus some context) from normalized_output.txt using
    # tools like 'sed' or 'awk'.  This is left as an exercise for the reader to keep the example concise.
    # echo
    # echo "Relevant Section(s) of Your Output (from normalized_output.txt):"
    # # ... (code to extract and display relevant lines based on diff_report.txt)
    # echo "..."
    # echo

    echo "NOTE: Your full, raw output was saved to 'actual_output.txt' for debugging."
fi

echo "===== Test Complete ====="