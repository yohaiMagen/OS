cmake_minimum_required(VERSION 3.6)
project(Ex2)

set(MAKE_CXX_STANDARD 11)

set(SOURCE_FILES uthreads.h uthreads.cpp Thread.h Thread.cpp sanity.cpp)

set(ITIMER_FILES demo_itimer.c )
add_executable(itimer ${ITIMER_FILES})

add_executable(Ex2 ${SOURCE_FILES})

set(JMP_FILES demo_jmp.c )
add_executable(jmp ${JMP_FILES})