#include"history.hpp"
#include<iostream>
#include <fstream>
#include<string>
#include<cstring>
#include<ctime>

#define BLUE    "\033[34m" 
#define RESET   "\033[0m"
#define GREEN    "\033[32m"
#define RED          "\033[31m"

void save_history(const std::string& cmd, std::time_t t) {

    std::string historyPath = std::string(getenv("HOME")) + "/.mintHistory";
    std::ofstream cmdFile(historyPath, std::ios::app);
    cmdFile << cmd << "\n";

    std::string timePath = std::string(getenv("HOME")) + "/.timeHistory";
    std::ofstream timeFile(timePath, std::ios::app);
    timeFile << t << "\n";
}
void load_history(std::vector<std::string>& cmds,
                  std::vector<std::time_t>& times)
{
    std::string hfile = std::string(getenv("HOME")) + "/.mintHistory";
    std::string tfile = std::string(getenv("HOME")) + "/.timeHistory";

    std::ifstream cmdFile(hfile);
    std::ifstream timeFile(tfile);

    std::string line;
    long long timestamp;

    while (std::getline(cmdFile, line)) {
        cmds.push_back(line);
    }

    while (timeFile >> timestamp) {
        times.push_back((std::time_t)timestamp);
    }
}
void history_command() {

    std::string hPath = std::string(getenv("HOME")) + "/.mintHistory";
    std::string tPath = std::string(getenv("HOME")) + "/.timeHistory";

    std::ifstream hFile(hPath);
    std::ifstream tFile(tPath);

    if (!hFile || !tFile) {
        std::cerr << "history: cannot read history files.\n";
        return;
    }

    std::string cmd;
    long long ts;
    int index = 1;

    while (std::getline(hFile, cmd) && (tFile >> ts)) {

        std::time_t t = (std::time_t)ts;
        char buf[40];
        std::strftime(buf, sizeof(buf), "%b %d %H:%M:%S %Y",
                      std::localtime(&t));

        std::cout <<RED <<index++ 
                  <<RESET<< "  " <<BLUE<< buf<<RESET 
                  << "   " << GREEN<<cmd <<RESET<< "\n";
    }
}

