#include <cstring>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

const int no_of_supported_commands = 8;
const int no_of_builtin_commands = 2;
std::string supported_commands[no_of_supported_commands] = {"cd", "help", "exit", "ls", "cat", "who", "pwd", "whoami"};
std::string builtin_commands[no_of_builtin_commands] = {"exit", "cd"};

// EXIT: this command is not an executable, it is a shell built-in command
// CD: doesnt exit as an executable, beacause a process can only change its own working directory and not of its parent process, we will need to implement cd as a builtin using chdir() system call

// can use "strtok" for this
// TODO: taking into account quotes and escape characters
std::vector<std::string>
sidsh_getargs(std::string line, std::string delimiter = " ") {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = line.find(delimiter)) != std::string::npos) {
        token = line.substr(0, pos);
        tokens.push_back(token);
        line.erase(0, pos + delimiter.length());
    }
    tokens.push_back(line);
    return tokens;
}

std::string sidsh_getline() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

bool supported_command_check(std::string &command) {
    for (int i = 0; i < no_of_supported_commands; i++) {
        if (supported_commands[i] == command)
            return true;
    }
    return false;
}

bool check_builtin_command(std::string &command) {
    for (int i = 0; i < no_of_builtin_commands; i++) {
        if (strcmp(builtin_commands[i].c_str(), command.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

int sidsh_cd(std::vector<std::string> &args) {
    std::string home = getenv("HOME");
    if (args.size() == 1) {
        if (chdir(home.c_str()) != 0) {
            std::cerr << "sidsh: cd: " << home << ": " << strerror(errno) << std::endl;
        }
    } else if (args.size() > 2) {
        std::cerr << "sidsh: cd: too many arguments" << std::endl;
    } else {
        if (chdir(args[1].c_str()) != 0) {
            std::cerr << "sidsh: cd: " << args[1] << ": " << strerror(errno) << std::endl;
        }
    }
    return 1;
}

void handle_builtin_command(std::vector<std::string> &args) {
    int status = 1;
    if (strncmp(args[0].c_str(), "exit", 2) == 0) {
        std::cout << "Exiting sidsh. Autobots roll out!" << std::endl;
        exit(0);
    } else if (strncmp(args[0].c_str(), "cd", 2) == 0) {
        status = sidsh_cd(args);
    }
}

int sidsh_execute(std::vector<std::string> &args) {
    // check if the command to be executed is builtin command
    bool is_builtin_command = check_builtin_command(args[0]);

    if (is_builtin_command) {
        handle_builtin_command(args);
        return 1; // TODO: different return value for builtin commands based on the execution status
    }

    // https://stackoverflow.com/questions/28733026/how-to-use-exec-to-execute-commands-in-c-and-linux
    // Convert std::vector<std::string> to char*[]
    std::vector<char *> c_args;
    for (auto arg : args) {
        c_args.push_back(strdup(arg.c_str())); // Copying strings to C-style strings
    }
    c_args.push_back(nullptr); // execvp expects a null-terminated array

    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0) { // CHILD PROCESS
        // man execvp
        // RETURN VALUES
        //     If any of the
        //     exec() functions returns,
        //     an error will have occurred.The return value is - 1, and the global variable errno will be set to indicate the error

        if (execvp(c_args[0], c_args.data()) == -1) {
            std::cerr << "sidsh: " << args[0] << ": " << strerror(errno) << std::endl;
        }
        // need to free the duplicated strings in child process
        for (char *c_arg : c_args) {
            free(c_arg);
        }
        exit(EXIT_FAILURE); // Exit child process if execvp fails
    } else if (pid < 0) {   // ERROR FORKING
        // FORK will also set global variable errno
        std::cerr << "sidsh: " << strerror(errno) << std::endl;

    } else {
        // PARENT PROCESS
        do {
            waitpid(pid, &status, WUNTRACED);
            // If the WUNTRACED option is set, children of the current process that are stopped due to a SIGTTIN, SIGTTOU,
            // SIGTSTP, or SIGSTOP signal also have their status reported.
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        //  WIFEXITED(status)
        //      True if the process terminated normally by a call to _exit(2) or exit(3).

        // WIFSIGNALED(status)
        //     True if the process terminated due to receipt of a signal.
    }

    // Free the duplicated strings in parent process
    for (char *c_arg : c_args) {
        free(c_arg);
    }

    return 1;
}

void sidsh_loop(void) {
    std::string input;
    std::vector<std::string> args;
    int status = 1;
    while (status) {
        printf("> ");
        input = sidsh_getline();
        args = sidsh_getargs(input);
        bool check = supported_command_check(args[0]);
        if (check == 0) {
            std::cout << args[0] << ": command not found" << std::endl;
            continue;
        }
        sidsh_execute(args);
    }
}

int main(int argc, char **argv) {

    sidsh_loop();

    return 0;
}