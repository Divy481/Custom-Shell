#include"pipe.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string>
#include<cstring>
#include <vector>
#include "tokenize.hpp"
#include<array>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include"redirection.hpp"

std::vector<std::string> tokenize(const std::string& cmd);

bool parse_pipeline(std::string& input, std::vector<std::string>& commands) {
    size_t start = 0, pos;

    while ((pos = input.find('|', start)) != std::string::npos) {
        std::string part = input.substr(start, pos - start);
        if (!part.empty()) commands.push_back(part);
        start = pos + 1;
    }

    std::string last = input.substr(start);
    if (!last.empty()) commands.push_back(last);

    return commands.size() > 1;
}

std::vector<char*> to_char(const std::string& cmd) {
    std::vector<char*> args;
    std::vector<std::string> tokens = tokenize(cmd);

    for (auto& t : tokens) {
        char* c = strdup(t.c_str());    // OK because child execvp replaces process
        args.push_back(c);
    }

    args.push_back(nullptr);
    return args;
}

void execute_pipeline(const std::vector<std::string>& commands) {

    int n = commands.size();
    std::vector<std::array<int,2>> pipeFd(n - 1);

    // Create N-1 pipes
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipeFd[i].data()) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    // Fork N processes
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {

            // If not first command → read from previous pipe
            if (i > 0) {
                dup2(pipeFd[i - 1][0], STDIN_FILENO);
            }

            // If not last command → write to next pipe
            if (i < n - 1) {
                dup2(pipeFd[i][1], STDOUT_FILENO);
            }

            // Close all pipes in the child
            for (int j = 0; j < n - 1; j++) {
                close(pipeFd[j][0]);
                close(pipeFd[j][1]);
            }

            // Run command
             std::vector<std::string> tokens = tokenize(commands[i]);
            Redirection rd = parse_redirection(tokens);
            apply_redirection(rd);      // does dup2()

            // Convert cleaned tokens → execvp argv
            std::vector<char*> argv;
            for (auto& t : tokens)
                argv.push_back(strdup(t.c_str()));
            argv.push_back(nullptr);

            execvp(argv[0], argv.data());
            perror("execvp");
            exit(1);
        }
    }

    // Parent closes all pipe ends
    for (int i = 0; i < n - 1; i++) {
        close(pipeFd[i][0]);
        close(pipeFd[i][1]);
    }

    // Parent waits for all children
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
}
