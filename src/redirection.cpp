#include"redirection.hpp"
#include<fcntl.h>
#include<unistd.h>



Redirection parse_redirection(std::vector<std::string>& tokens){
    Redirection rd;
    std::vector<std::string> clean;
    for(int i=0;i<tokens.size();i++){
        if(tokens[i] == "<"){
            rd.file_in = tokens[i+1];
            i++;
        }else if(tokens[i] == ">"){
            rd.file_out = tokens[i+1];
            i++;
        }else if(tokens[i] == ">>"){
            rd.file_out = tokens[i+1];
            rd.append = true;
            i++;
        }else{
            clean.push_back(tokens[i]);
        }
    }
    tokens = clean;  // remove redirection tokens
    return rd;
}


void apply_redirection(const Redirection& rd){
    if(!rd.file_in.empty()){
        int fd = open(rd.file_in.c_str(),O_RDONLY);
        if(fd<0){
            perror("input redirection eror");
            exit(1);
        }
        dup2(fd,STDIN_FILENO);
        close(fd);
    }

    if(!rd.file_out.empty()){
        int flags = rd.append ? (O_WRONLY | O_APPEND | O_CREAT)
                           : (O_WRONLY | O_CREAT | O_TRUNC);
            
        int fd = open(rd.file_out.c_str(),flags,0666);
        if(fd<0){
            perror("output redirection error");
            exit(1);
        }

        dup2(fd,STDOUT_FILENO);
        close(fd);
    }
}