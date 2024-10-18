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
  
## Usage

To run the shell locally:
1) Clone the repo
  ```
   git clone <repository-url>
   cd Shell
  ```

2) Compile the source code
   ```
   gcc -o apksh shell.c
   ```
3) Run the shell
   ```
   ./apksh
   ```

## Prerequisites

To compile and run APKSH, you need:

- GCC compiler
- readline library

On Ubuntu or Debian-based systems, you can install these with:

```
sudo apt-get install gcc libreadline-dev
```

## Compilation

To compile the shell, run:

```
gcc -o shell shell.c -lreadline
```

This will create an executable named `shell`.

## Running the Shell

To start the shell, run:

```
./shell
```

## Usage

Once in the shell, you can use the following built-in commands:

- `cd [directory]`: Change the current directory
- `pwd`: Print the current working directory
- `ls [directory]`: List contents of a directory
- `echo [text]`: Print text to the console
- `help`: Display help message with available commands
- `exit` or `bye`: Exit the shell

You can also run any other commands available in your system PATH.

Use the up and down arrow keys to navigate through command history, and use tab for auto-completion of commands and file paths.

## History

Command history is saved in `~/.apksh_history` and persists between sessions.

# Note
If you are using a windows pc, you will have to first install WSL on your system and download an Ubuntu distro (or any other distro). Clone the repo in the WSL and run it in the linux environment

### Maintainer
[Ameya Khanzode](https://github.com/AmeyaKhanzode)
