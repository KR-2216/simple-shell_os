#include "shell.hpp"
#include <vector>
#include <cstring>

void Shell::setup_signal_handlers() {
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}