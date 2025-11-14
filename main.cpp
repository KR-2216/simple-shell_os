#include "shell.hpp"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<Shell> global_shell;

void setup_signal_handlers() {
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, SIG_IGN);
}

void sigint_handler(int /* sig */) {
    std::cout << std::endl;
    if (global_shell) {
        global_shell->display_prompt();
    }
}

int main() {
    global_shell = std::make_unique<Shell>();
    
    try {
        global_shell->run();
    } catch (const std::exception& e) {
        std::cerr << "Shell error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}