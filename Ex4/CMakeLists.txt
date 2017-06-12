cmake_minimum_required(VERSION 3.6)
project(Ex4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Test1.cpp CacheFS.h CacheAlg.cpp CacheAlg.h Lru.cpp Lru.h Lfu.cpp Lfu.h Fbr.cpp Fbr.h CacheFS.cpp Test1.cpp)
add_executable(Ex4 ${SOURCE_FILES})

set(SOURCE_FILES1 main.cpp)
add_executable(main ${SOURCE_FILES1})