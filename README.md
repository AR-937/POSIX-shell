# POSIX Shell

This project is a custom-built POSIX-compliant shell, written in C. The shell implements essential features such as command parsing, execution of external programs, and handling built-in commands (e.g., `cd`, `pwd`, `echo`). It also supports advanced features, including quoted strings, input/output redirection, and more, simulating a fully functional shell environment.

## Features

- **POSIX Compliance**: Fully adheres to POSIX standards, ensuring compatibility with a wide range of shell commands and external programs.
- **Built-in Commands**: Implements common shell built-ins, such as:
  - `exit`: Exits the shell with a custom exit status.
  - `pwd`: Prints the current working directory.
  - `echo`: Outputs text or variables to the terminal.
  - `cd`: Changes the current directory, supporting absolute, relative paths, and home directory navigation.
- **Command Parsing**: Efficiently parses shell commands and arguments, supporting both built-in commands and external executables.
- **Redirection**: Supports input/output redirection, allowing you to:
  - Redirect `stdout` to files.
  - Redirect `stderr` to files.
  - Append to `stdout` or `stderr`.
  - Redirect input from files to programs.
- **Quoting**: Properly handles quoted strings and escape sequences, including:
  - Single (`'`) and double (`"`) quotes.
  - Backslashes (`\`) for escaping special characters.
- **REPL (Read-Eval-Print Loop)**: The shell continuously reads input, processes commands, and prints results, creating a smooth, interactive experience for the user.

## Installation

### Clone the Repository

Start by cloning the repository to your local machine:

```bash
git clone https://github.com/yourusername/shell.git
cd shell
```

### Compile the Project

To compile the shell, use `gcc`(or any other C compiler):

```bash
gcc main.c -o shell
```

### Run the Shell

After compiling, you can run the shell by executing the following command:

```bash
./shell
```

## Usage

Once the shell is running, you can interact with it by typing commands. It will behave similarly to a standard Unix shell, executing both built-in and external commands.

### Example Commands

- `pwd` - Prints the current working directory.
- `echo Hello, World!` - Outputs `Hello, World!` to the terminal.
- `cd /path/to/directory` - Changes the current directory to the specified path.
- `ls > output.txt` - Redirects the output of the `ls` command to a file named `output.txt`.
- `ls 2> error.txt` - Redirects the error output of the `ls` command to a file named `error.txt`.
- `cat < input.txt` - Redirects the contents of `input.txt` to the `cat` command as input.

## Contributing

Contributions are welcome! If you have suggestions for improvements, bug fixes, or new features, feel free to fork the repository and submit a pull request. All contributions will be reviewed and considered for inclusion.
