#include"history.hpp"
#include<iostream>
#include <fstream>
#include<string>
#include<cstring>

void load_history(std::vector<std::string>& historyList){
    std::string historyPath = std::string(getenv("HOME")) + "/.mintHistory";
    std::ifstream file(historyPath);
    std::string lines;
    while(std::getline(file,lines)){
        historyList.push_back(lines);
    }

}

void save_history(std::vector<std::string>& historyList){
    std::string historyPath = std::string(getenv("HOME")) + "/.mintHistory";
    std::ofstream file(historyPath,std::ios::app);
    if(!historyList.empty()){
        file<<historyList.back()<<"\n";
    }

}
int history_command(std::vector<std::string>& historyList, char** args){
    if(historyList.empty()){
        return -1;
    }
    
    if(args[1]!=NULL && strcmp(args[1],"-c")==0){
        historyList.clear();
        return 1;
    }
    int i=0;
    for(auto &history:historyList){
        std::cout<<++i<<" ";
        std::cout<<history<<"\n";
    }
    return 0;
}

