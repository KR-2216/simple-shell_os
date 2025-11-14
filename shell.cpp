#include "shell.hpp"
#include <iostream>
#include <iomanip>

Shell::Shell() : background(false), append_output(false), last_exit_status(0) {
    builtins = {
        {"cd", &Shell::cmd_cd},
        {"echo", &Shell::cmd_echo},
        {"pwd", &Shell::cmd_pwd},
        {"exit", &Shell::cmd_exit},
        {"help", &Shell::cmd_help},
        {"history", &Shell::cmd_history},
        {"export", &Shell::cmd_export},
        {"type", &Shell::cmd_type},
        {"jobs", &Shell::cmd_jobs},
        
        {"getpid", &Shell::cmd_getpid},
        {"getppid", &Shell::cmd_getppid},
        {"getuid", &Shell::cmd_getuid},
        {"sleep", &Shell::cmd_sleep},
        {"kill", &Shell::cmd_kill},
        {"alarm", &Shell::cmd_alarm},
        {"mkdir", &Shell::cmd_mkdir},
        {"rmdir", &Shell::cmd_rmdir},
        {"chmod", &Shell::cmd_chmod},
        {"chown", &Shell::cmd_chown},
        {"umask", &Shell::cmd_umask},
        {"stat", &Shell::cmd_stat},
        {"pipe", &Shell::cmd_pipe},
        {"touch", &Shell::cmd_touch},
        {"rename", &Shell::cmd_rename}
    };
    
    
    setup_paths();
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer))) {
        cwd = buffer;
    } else {
        cwd = "/";
    }
    
    setup_signal_handlers();
}

void Shell::run() {
    std::cout << "Simple Shell - OS Project (C++ Version)" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    
    while (true) {
        display_prompt();
        read_input();
        
        if (input.empty()) {
            continue;
        }
        
        parse_input();
        
        if (!args.empty()) {
            if (!execute_builtin()) {
                execute_command();
            }
        }
    }
}

void Shell::display_prompt() {
    std::cout << "\033[1;32m" << cwd << "\033[0m$ " << std::flush;
}

void Shell::read_input() {
    std::getline(std::cin, input);
    
    if (std::cin.eof()) {
        std::cout << "\nExit" << std::endl;
        exit(0);
    }
    
    trim_string(input);
}

void Shell::parse_input() {
    
    args.clear();
    command.clear();
    background = false;
    input_file.clear();
    output_file.clear();
    append_output = false;
    
    
    std::string expanded_input = expand_variables(input);
    
    std::istringstream iss(expanded_input);
    std::string token;
    bool parsing_redirections = false;
    
    while (iss >> token) {
        if (token == "&") {
            background = true;
            break;
        } else if (token == ">") {
            parsing_redirections = true;
            if (iss >> token) {
                output_file = token;
            }
        } else if (token == ">>") {
            parsing_redirections = true;
            append_output = true;
            if (iss >> token) {
                output_file = token;
            }
        } else if (token == "<") {
            parsing_redirections = true;
            if (iss >> token) {
                input_file = token;
            }
        } else if (!parsing_redirections) {
            args.push_back(token);
        }
    }
    
    if (!args.empty()) {
        command = args[0];
        add_to_history(input);
    }
}

void Shell::execute_command() {
    if (command.empty()) {
        return;
    }
    
    std::string command_path = resolve_command_path(command);
    if (command_path.empty()) {
        std::cerr << command << ": command not found" << std::endl;
        last_exit_status = 127;
        return;
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {
        
        
        
        if (!input_file.empty()) {
            int fd = open(input_file.c_str(), O_RDONLY);
            if (fd < 0) {
                perror("open input file");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        
        
        if (!output_file.empty()) {
            int flags = O_WRONLY | O_CREAT;
            if (append_output) {
                flags |= O_APPEND;
            } else {
                flags |= O_TRUNC;
            }
            
            int fd = open(output_file.c_str(), flags, 0644);
            if (fd < 0) {
                perror("open output file");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        
        
        std::vector<char*> exec_args;
        for (auto& arg : args) {
            exec_args.push_back(const_cast<char*>(arg.c_str()));
        }
        exec_args.push_back(nullptr);
        
        execvp(command_path.c_str(), exec_args.data());
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
            last_exit_status = WEXITSTATUS(status);
        } else {
            std::cout << "[" << pid << "] " << command << std::endl;
        }
    } else {
        perror("fork");
        last_exit_status = 1;
    }
}

bool Shell::execute_builtin() {
    if (command.empty()) {
        return false;
    }
    
    auto it = builtins.find(command);
    if (it != builtins.end()) {
        return (this->*(it->second))() != 0;
    }
    
    return false;
}

void Shell::add_to_history(const std::string& cmd) {
    if (history.size() >= MAX_SHELL_HISTORY) {
        history.erase(history.begin());
    }
    history.push_back(cmd);
}