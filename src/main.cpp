#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include "cd.hpp"
#include "history.hpp"

std::vector<std::string> tokenize(const std::string& input) {
    std::string token;
    std::vector<std::string> tokens;
    bool in_quotes = false;

    for (char ch : input) {
        if (ch == '"') {
            in_quotes = !in_quotes;
            continue;
        }

        if (!in_quotes && isspace(ch)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += ch;
        }
    }

    if (!token.empty())
        tokens.push_back(token);

    return tokens;
}

void shell_loop(char** env) {
    std::string input;
    std::vector<std::string>history_list;
    load_history(history_list);

    while (true) {
        char curr[PATH_MAX];
        getcwd(curr,sizeof(curr));
        std::cout << "mintOS[SHELL]> "<<"["<<curr<<"] ~ ";
        
        if (!std::getline(std::cin, input)) {
            std::cerr << "EXIT\n";
            break;
        }
        if(!input.empty()){
            history_list.push_back(input);
            save_history(history_list);
        }

        // Skip empty command
        if (input.empty())
            continue;

        // Tokenize
        std::vector<std::string> tokens = tokenize(input);
        if (tokens.empty())
            continue;

        // Convert vector<string> → vector<char*>
        std::vector<char*> args;
        for (auto& t : tokens)
            args.push_back(const_cast<char*>(t.c_str()));
        args.push_back(nullptr);

        // Built-in commands
        if (strcmp(args[0], "cd") == 0) {
            cd_command(args.data());
            continue;
        }

        if(strcmp(args[0],"pwd")==0){
            char  pwd[PATH_MAX];
            getcwd(pwd,sizeof(pwd));
            std::cout<<pwd<<std::endl;
            continue;
        }
        if (strcmp(args[0], "exit") == 0) {
            std::cout << "Bye!\n";
            break;
        }

        if(strcmp(args[0],"history")==0){
            history_command(history_list,args.data());
            continue;
        }

        // External command →
        
    }
}

int main(int argc, char** argv, char** env) {
    (void)argc;
    (void)argv;
    shell_loop(env);
    return 0;
}
