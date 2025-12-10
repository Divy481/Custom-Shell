#ifndef REDIRECTION_HPP
#define REDIRECTION_HPP


#include<vector>
#include<string>
struct Redirection{
    std::string file_in;
    std::string file_out;
    bool append = false;
};
Redirection parse_redirection(std::vector<std::string>& tokens);
void apply_redirection(const Redirection& rd);

#endif 