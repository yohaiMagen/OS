cmake_minimum_required(VERSION 3.6)
project(Ex3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -Wextra")

set(SOURCE_FILES Ex3/main.cpp Ex3/MapReduceFramework.cpp Ex3/MapReduceFramework.h Ex3/MapReduceClient.h Ex3/ThreadToContainer.cpp Ex3/ThreadToContainer.h)
add_executable(Ex3 ${SOURCE_FILES})

set(SOURCE_FILES1 Ex3/Search.cpp Ex3/MapReduceFramework.cpp Ex3/MapReduceFramework.h Ex3/MapReduceClient.h Ex3/ThreadToContainer.cpp Ex3/ThreadToContainer.h)
add_executable(Search ${SOURCE_FILES1})