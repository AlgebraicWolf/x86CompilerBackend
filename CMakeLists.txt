cmake_minimum_required(VERSION 3.17)
project(x86CompilerBackend)

set(CMAKE_CXX_STANDARD 17)

add_library(Utilities utilities.cpp AbstractSyntaxTree.cpp AbstractSyntaxTree.hpp)

add_executable(x86CompilerBackend main.cpp)

target_link_libraries(x86CompilerBackend Utilities)