#include <stdio.h>
#include <iostream>
#include <vector>

int no_of_supported_commands = 3;
std::string supported_commands[3] = {"cd", "help", "exit"};

// can use "strtok" for this
// TODO: taking into account quotes and escape characters
std::vector<std::string>sidsh_getargs(std::string line, std::string delimiter=" "){
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

std::string sidsh_getline(){
    std::string input;
    std::getline(std::cin, input);
    return input;
}

bool supported_command_check(std::string &command){
    for(int i = 0; i<no_of_supported_commands; i++){
        if(supported_commands[i] == command) return true;
    }
    return false;
}

void sidsh_execute(std::vector<std::string> &args){

}


void sidsh_loop(void){
    std::string input;
    std::vector<std::string> args;
    int status=1;
    while(status){
        printf("> ");
        input = sidsh_getline();
        args = sidsh_getargs(input);
        std::cout<<"length of args: "<<args.size()<<std::endl;
        if(args.size()==0){
            continue;
        }
        bool check = supported_command_check(args[0]);
        if(check == 0){
            std::cout<<args[0]<<": command not found"<<std::endl;
            continue;
        }
        sidsh_execute(args);
    }
}


int main(int argc, char **argv){
    
    sidsh_loop();

    return 0;
}