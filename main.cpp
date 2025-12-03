#include"shell.hpp"

//shell loop
//input parsing 
//command excution
//handle built-in command cd,pwd,etc.
//excute external commands
//manage enivrnemt variable 
//manage path 
//error handling  



int shell_built(std::vector<char*>argc,char** env,char* intial_directory){
    if(argc.empty())return -1;
    std::string argu = argc[0];

    if(argu =="cd"){
        if(argu.size()<2){
            std::cerr<<"cd: missing operands\n";
            return -1;
        }
        if(chdir(argc[1])!=0){
            perror("cd");
        }
        return 1;
    }else if(argu == "exit"){
        exit(0);
    }

    return 0;
}
void shell_loop(char** env){
    std::string input;
    std::vector<char*> argc;
    char * intial_directory = getcwd(NULL,0);
    while(true){
        std::cout<<"[SHELL]> ";
        if(!std::getline(std::cin,input)){
            perror("EXIT");
            break;
        }
        std::vector<std::string> tokens = tokenize(input);

        for(auto& token:tokens){
            argc.push_back(const_cast<char*>(token.c_str()));
        }

        argc.push_back(nullptr);

        if(argc[0] !=NULL){
            shell_built(argc,env,intial_directory);
        }
        
    }
}
int main(int argc,char** argv,char** env){
    (void)argc;
    (void)argv;
    shell_loop(env);
    return 0;
}