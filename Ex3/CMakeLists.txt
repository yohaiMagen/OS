cmake_minimum_required(VERSION 3.6)
project(Ex3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -Wextra")

set(SOURCE_FILES main.cpp MapReduceFramework.cpp MapReduceFramework.h MapReduceClient.h ThreadToContainer.cpp ThreadToContainer.h)
add_executable(Ex3 ${SOURCE_FILES})

set(SOURCE_FILES1 Search.cpp MapReduceFramework.cpp MapReduceFramework.h MapReduceClient.h ThreadToContainer.cpp ThreadToContainer.h)
add_executable(Search ${SOURCE_FILES1})

set(SOURCE_FILES2 test.cpp MapReduceFramework.cpp MapReduceFramework.h MapReduceClient.h ThreadToContainer.cpp ThreadToContainer.h)
add_executable(test1 ${SOURCE_FILES2})

