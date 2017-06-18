cmake_minimum_required(VERSION 3.6)
project(Ex5)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Client.cpp Client.h Server.cpp)
add_executable(Ex5 ${SOURCE_FILES})