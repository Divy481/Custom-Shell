cmake_minimum_required(VERSION 3.28.3)

project(
    myProject
    LANGUAGES C CXX
    VERSION 1.0.0
    
)

add_executable(run src/main.cpp src/cd.cpp src/history.cpp src/ls.cpp src/pipe.cpp src/tokenize.cpp src/redirection.cpp src/job_control.cpp src/cat.cpp) 