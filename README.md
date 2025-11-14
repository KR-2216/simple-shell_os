# Simple Shell - C++ Implementation

A basic command line interface shell implemented in C++ for Operating Systems project. This shell provides similar functionality to UNIX shells and works well with WSL on Windows 11.

## Features

- **Built-in Commands**: cd, echo, pwd, exit, help, history, export, type, jobs
- **External Command Execution**: Execute system commands via fork/exec
- **I/O Redirection**: Input (`<`), output (`>`), and append (`>>`) redirection
- **Background Processes**: Run commands in background using `&`
- **Environment Variables**: Support for variable expansion (`$VAR`, `$?`, `$$`)
- **Command History**: Track command history
- **PATH Resolution**: Automatic command lookup in PATH
- **Signal Handling**: Proper handling of Ctrl+C

## Requirements

- Windows 11 with WSL enabled, OR Linux system
- GCC/G++ compiler with C++17 support
- Make build system

## Building

1. Clone or download the source files to a directory
2. Navigate to the project directory
3. Run `make` to compile:
```bash
make
```
4. Run `./simple_shell`
```bash
./simple_shell
```
### If you encounter permission issues:
1. `chmod +x Makefile`
```bash
chmod +x Makefile
```
2. `make clean`
```bash
make clean
```

