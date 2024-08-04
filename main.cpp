#include <cstring>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

const int no_of_supported_commands = 8;
std::string supported_commands[no_of_supported_commands] = {"cd", "help", "exit", "ls", "cat", "who", "pwd", "whoami"};
std::string builtin_commands[1] = {"exit"};

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
    for (int i = 0; i < 1; i++) {
        if (builtin_commands[i] == command) {
            return true;
        }
    }
    return false;
}

void handle_builtin_command(std::vector<char *> &c_args) {
    if (strncmp(c_args[0], "exit", 2) == 0) {
        for (char *c_arg : c_args) {
            free(c_arg);
        }
        std::cout << "Exiting sidsh. Autobots roll out!" << std::endl;
        exit(0);
    }
}

int sidsh_execute(std::vector<std::string> &args) {
    // https://stackoverflow.com/questions/28733026/how-to-use-exec-to-execute-commands-in-c-and-linux
    // Convert std::vector<std::string> to char*[]
    std::vector<char *> c_args;
    for (auto arg : args) {
        c_args.push_back(strdup(arg.c_str())); // Copying strings to C-style strings
    }
    c_args.push_back(nullptr); // execvp expects a null-terminated array

    pid_t pid;
    int status;

    // check if the command to be executed is builtin command
    bool is_builtin_command = check_builtin_command(args[0]);

    if (is_builtin_command) {
        handle_builtin_command(c_args);
        return 1; //TODO: different return value for builtin commands based on the execution status
    }

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