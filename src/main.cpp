#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <climits>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>
#include <ctime>

#include "cd.hpp"
#include "history.hpp"
#include "ls.hpp"
#include "tokenize.hpp"
#include "pipe.hpp"
#include "redirection.hpp"
#include "cat.hpp"
#include "job_control.hpp"

#define BLUE    "\033[34m" 
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define CYAN    "\033[36m"


void start_gui(){
    std::cout<<RED<<"----------------------------------------------------"<<RESET<<"\n";
    
    std::cout<<RED<<"|"<<std::setw(52)<<"|"<<"\n";
    std::cout<<RED<<"|"<<std::setw(52)<<"|"<<"\n";
    std::cout<<RED<<"|"<<std::setw(25)<<"MINTOS"<<std::setw(27)<<"|"<<"\n";
    std::cout<<RED<<"|"<<std::setw(52)<<"|"<<"\n";
    std::cout<<RED<<"|"<<std::setw(52)<<"|"<<"\n";
    
    std::cout<<RED<<"----------------------------------------------------"<<RESET<<"\n";

}

void shell_loop(char** env) {
    std::string input;

    while (true) {
        process_sigchld_events();
        reap_done_jobs();

        char curr[PATH_MAX];
        getcwd(curr, sizeof(curr));
        std::cout << GREEN << "mintOS[SHELL]> "
                  << RESET << BLUE << "[" << curr << "] ~ "
                  << RESET;

        if (!std::getline(std::cin, input)) break;
        if (input.empty()) continue;

        /* ---------------- PIPELINE ---------------- */
        std::vector<std::string> pipeline_cmds;
        if (parse_pipeline(input, pipeline_cmds)) {
            execute_pipeline(pipeline_cmds);
            continue;
        }

        /* ---------------- TOKENIZE ---------------- */
        std::vector<std::string> tokens = tokenize(input);
        if (tokens.empty()) continue;

        /* ---------------- BACKGROUND ---------------- */
        bool background = false;
        if (tokens.back() == "&") {
            background = true;
            tokens.pop_back();
        }

        /* ---------------- BUILTINS ---------------- */
        if (tokens[0] == "exit") break;

        if (tokens[0] == "cd") {
            std::vector<char*> args;
            for (auto& s : tokens) args.push_back((char*)s.c_str());
            args.push_back(nullptr);
            cd_command(args.data());
            continue;
        }

        if (tokens[0] == "jobs") {
            list_jobs();
            continue;
        }

        if (tokens[0] == "fg") {
            put_job_in_foreground(std::stoi(tokens[1]), true);
            continue;
        }

        if (tokens[0] == "bg") {
            put_job_in_background(std::stoi(tokens[1]), true);
            continue;
        }

        if (tokens[0] == "ls") {
            std::vector<char*> args;
            for (auto& s : tokens) args.push_back((char*)s.c_str());
            args.push_back(nullptr);
            ls_command(args.data());
            continue;
        }

        if (tokens[0] == "cat") {
            std::vector<char*> args;
            for (auto& s : tokens) args.push_back((char*)s.c_str());
            args.push_back(nullptr);
            cat_command(args.data());
            continue;
        }

        /* ---------------- REDIRECTION ---------------- */
        if (input.find('<') != std::string::npos ||
            input.find('>') != std::string::npos) {

            pid_t pid = fork();
            if (pid == 0) {
                auto rtokens = tokenize(input);
                Redirection rd = parse_redirection(rtokens);
                apply_redirection(rd);

                std::vector<char*> argv;
                for (auto& s : rtokens)
                    argv.push_back(strdup(s.c_str()));
                argv.push_back(nullptr);

                execvp(argv[0], argv.data());
                perror("exec");
                _exit(1);
            }
            waitpid(pid, nullptr, 0);
            continue;
        }

        /* ---------------- EXTERNAL COMMAND ---------------- */
        launch_job(tokens, background, env);
    }
}


int main(int argc, char** argv, char** env) {
    (void)argc;
    (void)argv;
    init_shell();
    start_gui();
    
    shell_loop(env);
    return 0;
}
