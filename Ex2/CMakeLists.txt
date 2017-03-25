cmake_minimum_required(VERSION 3.6)
project(Ex2)

set(MAKE_CXX_STANDARD 11)

set(SOURCE_FILES uthreads.h uthreads.cpp thread.cpp thread.h)
add_executable(Ex2 ${SOURCE_FILES})