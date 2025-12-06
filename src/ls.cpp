#include"ls.hpp"
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <iomanip>
#include<sys/stat.h>
#include <unistd.h>

// Colors
#define RESET   "\033[0m"
#define BLUE    "\033[34m"   // Directory
#define GREEN   "\033[32m"   // Executable
#define CYAN    "\033[36m"   // Symlink


void print_color(const char* path,const char* name){
    struct stat st;
    std::string fullPath = std::string(path)+"/"+name;

    if(lstat(fullPath.c_str(),&st)==-1){
        std::cout<<name<<" ";
        return;
    }

    if(S_ISDIR(st.st_mode)){
        std::cout<<BLUE<<name<<RESET<<std::setw(10)<<" ";
    }else if(S_ISLNK(st.st_mode)){
        std::cout<<CYAN<<name<<RESET<<std::setw(10)<<" ";
    }else if(st.st_mode & S_IXUSR){
        std::cout<<GREEN<<name<<RESET<<std::setw(10)<<" ";
    }else{
        std::cout<<name<<std::setw(10)<<" ";
    }
}
int ls_command(char** args){
    const char* path = ".";

    bool showHidden = false;
    int i = 1;
    while(args[i]!=NULL){
        if(strcmp(args[i],"-a")==0){
            showHidden = true;
        }else{
            path = args[i];
        }
        i++;
    }
    DIR* dir = opendir(path);
    if(!dir){
        perror("ls");
        return -1;
    }
    struct dirent* entry;
    while((entry = readdir(dir))!=NULL){
        if(!showHidden && entry->d_name[0] == '.')continue;
        print_color(path,entry->d_name);        
    }
    std::cout<<"\n";
    closedir(dir);
    return 0;
}