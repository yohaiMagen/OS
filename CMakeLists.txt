cmake_minimum_required(VERSION 3.6)
project(Ex2)

set(MAKE_CXX_STANDARD 11)

set(SOURCE_FILES Ex2/uthreads.h Ex2/uthreads.cpp Ex2/Thread.h Ex2/Thread.cpp Ex2/test1430.cpp)

add_executable(Ex2 ${SOURCE_FILES})