#include"cat.hpp"
#include<string>
#include<fstream>
#include<cstring>
#include<iostream>

void cat_command(char** argv){
    if(argv[0] == nullptr){
        std::cerr<<"cat : missing file operands"<<std::endl;
        exit(1);
    }

    for(int i=1;argv[i]!=nullptr;i++){
        std::ifstream file(argv[i]);
        if(!file){
            std::cerr<<"cat: "<<argv[i]<<" :missing operands"<<std::endl;
            continue;
        }
        std::string line;
        while(std::getline(file,line)){
            std::cout<<line<<"\n";
        }
        file.close();
    }
}