cmake_minimum_required(VERSION 3.31)
project(json_parser)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_executable(json_parser main.cpp)
