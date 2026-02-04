# Simple Shell Implementation in C

This project is a minimalist Unix shell implemented in C, designed to explore the core mechanics of how operating systems manage processes and execute commands.

## Overview

The goal of this project is to understand the fundamental lifetime cycle of a shell:
1. **Initialize:** Setting up the environment.
2. **Interpret:** A loop that reads input from `stdin`, parses it into commands/arguments, and executes them.
3. **Terminate:** Freeing memory and exiting.

This is a learning-focused implementation. While it mimics the behavior of standard shells like Bash, it focuses on essential system calls and logic required for process management.

## Features

- **Command Loop:** A standard REPL (Read-Eval-Print Loop) that provides a prompt and waits for user input.
- **Input Parsing:** Handles dynamic memory allocation to read lines and tokenize them into individual arguments.
- **Process Creation:** Utilizes `fork()` and `exec()` system calls to launch programs.
- **Process Synchronization:** Uses `waitpid()` to ensure the shell waits for child processes to finish.
- **Built-in Commands:** Includes internal implementations for `cd`, `help`, and `exit`.

## Technical Details

- **Memory Management:** Manual handling using `malloc`, `realloc`, and `free` to manage command buffers.
- **System Calls:** Direct usage of POSIX APIs, including `unistd.h` for process control and `sys/wait.h` for state management.
- **Tokenization:** Uses `strtok` to split input strings based on whitespace delimiters.

## Project Status

This project was developed as a hands-on learning exercise to build a foundation for more complex systems. It is currently a work in progress, and I plan to add more advanced features in the future.

Potential enhancements include:
- Implementing support for quotes and backslash escaping in arguments.
- Adding piping (`|`) and I/O redirection (`<`, `>`).
- Implementing signal handling and background process execution.

## Getting Started

To compile and run the shell on your local machine, use the following commands in your terminal:

```bash
# Compile the source code
gcc main.c shell.c -o myshell

# Run the executable
./myshell
