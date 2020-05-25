cmake_minimum_required(VERSION 3.17)
project(x86CompilerBackend)

add_compile_options(-march=native)

set(CMAKE_CXX_STANDARD 17)

add_library(Utilities utilities.cpp)

add_library(AssemblyTools AssemblyTools.cpp)

add_executable(x86CompilerBackend main.cpp)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

target_link_libraries(x86CompilerBackend Utilities AssemblyTools)