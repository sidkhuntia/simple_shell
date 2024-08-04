# SIDSH - Simple Interactive Shell

SIDSH is a basic interactive shell implemented in C++. It provides a command-line interface for executing both built-in and external commands.

## Features

- Basic command execution
- Support for built-in commands (exit, cd)
- Simple command parsing
- Error handling for unsupported commands

## Supported Commands

SIDSH currently supports the following commands:
- cd
- help (not implemented yet)
- exit
- ls
- cat
- who
- pwd
- whoami
- clear (planned implementation)

## Built-in Commands

The following commands are implemented as shell built-ins:
- exit: Exits the shell
- cd: Changes the current working directory

## Compilation

To compile SIDSH, use a C++ compiler supporting C++11 or later. For example:

```
g++ -std=c++11 sidsh.cpp -o sidsh
```

## Usage

After compilation, run the shell:

```
./sidsh
```

You will be presented with a prompt ('>') where you can enter commands.

## Future Improvements

- Implement the 'help' command
- Add support for input/output redirection
- Implement command history
- Add support for environment variables
- Implement the 'clear' command using the ncurses library

## Known Issues

- The 'clear' command is listed as supported but not yet implemented
- Error handling could be improved for edge cases
- No support for complex command structures (pipes, redirections, etc.)
