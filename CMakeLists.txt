cmake_minimum_required(VERSION 3.28)
project(assignment_1_LMD)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

# flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -lm -fopenmp -O3") # -O3

add_executable(
        assignment_1_LMD
        main.cpp
        graph/graph.cpp
        graph/graph.h
        graph/graph_by_row.h
        graph/graph_by_row.cpp
        utility.cpp
        utility.h
)
