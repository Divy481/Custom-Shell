#ifndef PIPE_HPP
#define PIPE_HPP

#include <string>
#include <vector>

bool parse_pipeline( std::string &input, std::vector<std::string> &commands);
void execute_pipeline(const std::vector<std::string> &commands);

#endif
