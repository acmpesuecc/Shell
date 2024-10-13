# Ameya's Basic Shell (apksh)

Welcome to **Ameya's Basic Shell (apksh)**, a simple command-line shell written in C. This shell provides a basic command interface that supports various built-in commands for navigating and managing the file system.

## Features

- **Built-in Commands**: Supports the following commands:
  - `cd <directory>`: Change the current directory.
  - `ls [directory]`: List the contents of the specified directory (current directory if not specified).
  - `mkdir <directory>`: Create a new directory.
  - `pwd`: Print the current working directory.
  - `echo <text>`: Print the specified text to the console.
  - `help`: Display information about built-in commands.
  - `exit` or `bye`: Exit the shell.
  - `man <command>`: Display usage and description for the specified command.
  
- **Interactive Shell**: Provides a user-friendly command-line interface for executing commands.
- **Dynamic Memory Management**: Allocates memory dynamically to handle user input and command execution.

## Installation

To compile and run Ameya's Basic Shell, follow these steps:

1. **Clone the Repository** (if applicable):
   ```bash
   git clone <repository-url>
   cd apksh
