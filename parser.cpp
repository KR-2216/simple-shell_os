#include "shell.hpp"
#include <sstream>
#include <cstdlib>

void Shell::setup_paths() {
    paths.clear();
    
    const char* path_env = getenv("PATH");
    if (!path_env) {
        paths.push_back("/usr/local/bin");
        paths.push_back("/usr/bin");
        paths.push_back("/bin");
        return;
    }
    
    std::string path_str = path_env;
    std::istringstream iss(path_str);
    std::string path;
    
    while (std::getline(iss, path, ':')) {
        if (!path.empty()) {
            paths.push_back(path);
        }
    }
}

std::string Shell::resolve_command_path(const std::string& command) {
    if (command.find('/') != std::string::npos) {
        if (access(command.c_str(), X_OK) == 0) {
            return command;
        }
        return "";
    }

    for (const auto& path_dir : paths) {
        std::string full_path = path_dir + "/" + command;
        if (access(full_path.c_str(), X_OK) == 0) {
            return full_path;
        }
    }
    
    return "";
}

std::string Shell::expand_variables(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '$' && i + 1 < str.length()) {
            if (str[i + 1] == '?') {
                result += std::to_string(last_exit_status);
                i++;
            } else if (str[i + 1] == '$') {
                result += std::to_string(getpid());
                i++;
            } else {
                std::string var_name;
                i++;
                
                while (i < str.length() && (isalnum(str[i]) || str[i] == '_')) {
                    var_name += str[i];
                    i++;
                }
                i--;
                
                const char* value = getenv(var_name.c_str());
                if (value != nullptr) {
                    result += value;
                }
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

void Shell::trim_string(std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) {
        str = str.substr(start);
    } else {
        str.clear();
        return;
    }

    size_t end = str.find_last_not_of(" \t\n\r");
    if (end != std::string::npos) {
        str = str.substr(0, end + 1);
    }
}