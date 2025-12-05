#ifndef HISTORY_HPP
#define HISTORY_HPP

#include<vector>
#include<string>
int history_command(std::vector<std::string>& historyList, char** args);
void load_history(std::vector<std::string>& historyList);
void save_history(std::vector<std::string>& historyList);
#endif   //HISTROY_HPP