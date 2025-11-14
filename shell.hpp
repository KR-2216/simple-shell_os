#ifndef SHELL_HPP
#define SHELL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

class Shell {
private:
    
    std::string input;
    std::vector<std::string> args;
    std::string command;
    std::string cwd;
    std::vector<std::string> paths;
    std::vector<std::string> history;
    void execute_system_call();
    
    
    bool background;
    std::string input_file;
    std::string output_file;
    bool append_output;
    int last_exit_status;
    
    
    std::unordered_map<std::string, int (Shell::*)(void)> builtins;
    
    
    static const int MAX_SHELL_INPUT = 1024;
    static const int MAX_SHELL_HISTORY = 100;

public:

    Shell();
    ~Shell() = default;
    
    
    void run();
    
    
    void display_prompt();
    void read_input();
    void parse_input();
    void execute_command();
    bool execute_builtin();
    
    
    int cmd_cd();
    int cmd_echo();
    int cmd_pwd();
    int cmd_exit();
    int cmd_help();
    int cmd_history();
    int cmd_export();
    int cmd_type();
    int cmd_jobs();
    int cmd_getpid();
    int cmd_getppid();
    int cmd_getuid();
    int cmd_sleep();
    int cmd_kill();
    int cmd_alarm();
    int cmd_mkdir();
    int cmd_rmdir();
    int cmd_chmod();
    int cmd_chown();
    int cmd_umask();
    int cmd_stat();
    int cmd_pipe();
    int cmd_touch();
    int cmd_rename();
    
    
    void setup_paths();
    void setup_signal_handlers();
    std::string resolve_command_path(const std::string& command);
    std::string expand_variables(const std::string& str);
    void trim_string(std::string& str);
    void add_to_history(const std::string& command);
    
    
    friend void sigint_handler(int sig);
};


extern std::unique_ptr<Shell> global_shell;


void setup_signal_handlers();
void sigint_handler(int sig);

#endif