cmake_minimum_required(VERSION 3.17)
project(x86CompilerBackend)

add_compile_options(-march=native)

set(CMAKE_CXX_STANDARD 17)

add_library(Utilities utilities.cpp)

add_library(AbstractSyntaxTree AbstractSyntaxTree.cpp)

add_library(HashTable HashTable.cpp)

add_library(Vector Vector.cpp)

add_executable(x86CompilerBackend main.cpp)

target_link_libraries(x86CompilerBackend Utilities HashTable Vector AbstractSyntaxTree)