cmake_minimum_required(VERSION 3.10)
project(problem_081)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(code solution.cpp)

target_compile_options(code PRIVATE -O2 -Wall -Wextra)