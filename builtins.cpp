#include "shell.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>

int Shell::cmd_cd() {
    std::string path;
    
    if (args.size() < 2 || args[1] == "~") {
        const char* home = getenv("HOME");
        path = home ? home : "/";
    } else if (args[1] == "-") {
        const char* oldpwd = getenv("OLDPWD");
        if (!oldpwd) {
            std::cerr << "cd: OLDPWD not set" << std::endl;
            return 1;
        }
        path = oldpwd;
        std::cout << path << std::endl;
    } else {
        path = args[1];
    }
    
    char old_cwd[1024];
    if (!getcwd(old_cwd, sizeof(old_cwd))) {
        perror("getcwd");
        return 1;
    }
    
    if (chdir(path.c_str()) != 0) {
        perror("cd");
        return 1;
    }
    
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer))) {
        cwd = buffer;
    }
    
    setenv("OLDPWD", old_cwd, 1);
    setenv("PWD", cwd.c_str(), 1);
    
    return 1;
}

int Shell::cmd_echo() {
    for (size_t i = 1; i < args.size(); i++) {
        std::cout << args[i];
        if (i < args.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    return 1;
}

int Shell::cmd_pwd() {
    std::cout << cwd << std::endl;
    return 1;
}

int Shell::cmd_exit() {
    std::cout << "Goodbye!" << std::endl;
    exit(0);
}

int Shell::cmd_help() {
    std::cout << "Simple Shell - OS Project (C++ Version)\n\n";
    std::cout << "Built-in commands:\n";
    std::cout << "  cd [dir]        Change directory\n";
    std::cout << "  echo [text]     Display text\n";
    std::cout << "  pwd             Print working directory\n";
    std::cout << "  exit            Exit the shell\n";
    std::cout << "  help            Show this help\n";
    std::cout << "  history         Show command history\n";
    std::cout << "  export [var=value] Set environment variable\n";
    std::cout << "  type [command]  Show command type\n";
    std::cout << "  jobs            Show background jobs\n";
    std::cout << "  getpid          Show current process ID\n";
    std::cout << "  getppid         Show parent process ID\n";
    std::cout << "  getuid          Show user ID\n";
    std::cout << "  sleep [sec]     Sleep for seconds\n";
    std::cout << "  kill [pid]      Send signal to process\n";
    std::cout << "  alarm [sec]     Set alarm\n";
    std::cout << "  mkdir [dir]     Create directory\n";
    std::cout << "  rmdir [dir]     Remove directory\n";
    std::cout << "  chmod [mode file] Change file permissions\n";
    std::cout << "  chown [uid file] Change file owner\n";
    std::cout << "  umask [mask]    Set file creation mask\n";
    std::cout << "  stat [file]     Show file status\n";
    std::cout << "  pipe            Create a pipe\n";
    std::cout << "  touch [file]    Create empty file\n";
    std::cout << "  rename <old> <new>  Rename file\n";
    std::cout << "\nFeatures:\n";
    std::cout << "  - Background processes (&)\n";
    std::cout << "  - Environment variables\n";
    std::cout << "  - PATH resolution\n";
    std::cout << "  - Command history\n";
    return 1;
}

int Shell::cmd_history() {
    for (size_t i = 0; i < history.size(); i++) {
        std::cout << std::setw(3) << (i + 1) << "  " << history[i] << std::endl;
    }
    return 1;
}

int Shell::cmd_export() {
    if (args.size() < 2) {
        extern char** environ;
        for (char** env = environ; *env; env++) {
            std::cout << *env << std::endl;
        }
        return 1;
    }
    
    const std::string& arg = args[1];
    size_t equals_pos = arg.find('=');
    
    if (equals_pos == std::string::npos) {
        std::cerr << "export: usage: export VAR=value" << std::endl;
        return 1;
    }
    
    std::string var = arg.substr(0, equals_pos);
    std::string value = arg.substr(equals_pos + 1);
    
    if (setenv(var.c_str(), value.c_str(), 1) != 0) {
        perror("export");
        return 1;
    }

    if (var == "PATH") {
        setup_paths();
    }
    
    return 1;
}

int Shell::cmd_type() {
    if (args.size() < 2) {
        std::cerr << "type: missing argument" << std::endl;
        return 1;
    }
    
    const std::string& command = args[1];
    
    if (builtins.find(command) != builtins.end()) {
        std::cout << command << " is a shell builtin" << std::endl;
        return 1;
    }
    
    std::string path = resolve_command_path(command);
    if (!path.empty()) {
        std::cout << command << " is " << path << std::endl;
    } else {
        std::cout << command << ": not found" << std::endl;
    }
    
    return 1;
}

int Shell::cmd_jobs() {
    std::cout << "Background jobs feature not fully implemented" << std::endl;
    std::cout << "(Basic background process support is available using '&')" << std::endl;
    return 1;
}

int Shell::cmd_getpid() {
    std::cout << "Process ID: " << getpid() << std::endl;
    return 1;
}

int Shell::cmd_getppid() {
    std::cout << "Parent Process ID: " << getppid() << std::endl;
    return 1;
}

int Shell::cmd_getuid() {
    std::cout << "User ID: " << getuid() << std::endl;
    return 1;
}

int Shell::cmd_sleep() {
    if (args.size() < 2) {
        std::cerr << "sleep: missing operand" << std::endl;
        return 1;
    }
    int seconds = std::stoi(args[1]);
    sleep(seconds);
    return 1;
}

int Shell::cmd_kill() {
    if (args.size() < 2) {
        std::cerr << "kill: usage: kill <pid>" << std::endl;
        return 1;
    }
    int pid = std::stoi(args[1]);
    if (kill(pid, SIGTERM) != 0) {
        perror("kill");
    }
    return 1;
}

int Shell::cmd_alarm() {
    if (args.size() < 2) {
        std::cerr << "alarm: usage: alarm <seconds>" << std::endl;
        return 1;
    }
    int seconds = std::stoi(args[1]);
    alarm(seconds);
    std::cout << "Alarm set for " << seconds << " seconds" << std::endl;
    return 1;
}

int Shell::cmd_mkdir() {
    if (args.size() < 2) {
        std::cerr << "mkdir: missing operand" << std::endl;
        return 1;
    }
    if (mkdir(args[1].c_str(), 0755) != 0) {
        perror("mkdir");
    }
    return 1;
}

int Shell::cmd_rmdir() {
    if (args.size() < 2) {
        std::cerr << "rmdir: missing operand" << std::endl;
        return 1;
    }
    if (rmdir(args[1].c_str()) != 0) {
        perror("rmdir");
    }
    return 1;
}

int Shell::cmd_chmod() {
    if (args.size() < 3) {
        std::cerr << "chmod: usage: chmod <mode> <file>" << std::endl;
        return 1;
    }
    mode_t mode = std::stoi(args[1], 0, 8);
    if (chmod(args[2].c_str(), mode) != 0) {
        perror("chmod");
    }
    return 1;
}

int Shell::cmd_chown() {
    if (args.size() < 3) {
        std::cerr << "chown: usage: chown <uid> <file>" << std::endl;
        return 1;
    }
    uid_t uid = std::stoi(args[1]);
    if (chown(args[2].c_str(), uid, -1) != 0) {
        perror("chown");
    }
    return 1;
}

int Shell::cmd_umask() {
    mode_t mask = 0022;
    if (args.size() >= 2) {
        mask = std::stoi(args[1], 0, 8);
    }
    mode_t old_mask = umask(mask);
    std::cout << "Old umask: " << std::oct << old_mask << std::dec << std::endl;
    return 1;
}

int Shell::cmd_stat() {
    if (args.size() < 2) {
        std::cerr << "stat: missing operand" << std::endl;
        return 1;
    }
    struct stat file_stat;
    if (stat(args[1].c_str(), &file_stat) == 0) {
        std::cout << "File: " << args[1] << std::endl;
        std::cout << "Size: " << file_stat.st_size << " bytes" << std::endl;
        std::cout << "Permissions: " << std::oct << file_stat.st_mode << std::dec << std::endl;
        std::cout << "Owner UID: " << file_stat.st_uid << std::endl;
    } else {
        perror("stat");
    }
    return 1;
}

int Shell::cmd_pipe() {
    int pipefd[2];
    if (pipe(pipefd) == 0) {
        std::cout << "Pipe created: read=" << pipefd[0] << ", write=" << pipefd[1] << std::endl;
        close(pipefd[0]);
        close(pipefd[1]);
    } else {
        perror("pipe");
    }
    return 1;
}

int Shell::cmd_touch() {
    if (args.size() < 2) {
        std::cerr << "touch: missing file operand" << std::endl;
        return 1;
    }
    int fd = open(args[1].c_str(), O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        perror("touch");
        return 1;
    }
    close(fd);
    return 1;
}

int Shell::cmd_rename() {
    if (args.size() < 3) {
        std::cerr << "rename: usage: rename <old> <new>" << std::endl;
        return 1;
    }
    if (rename(args[1].c_str(), args[2].c_str()) != 0) {
        perror("rename");
    }
    return 1;
}