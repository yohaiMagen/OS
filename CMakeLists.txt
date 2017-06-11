cmake_minimum_required(VERSION 3.6)
project(Ex4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")


set(SOURCE_FILES Ex4/main.cpp Ex4/CacheFS.h Ex4/CacheAlg.cpp Ex4/CacheAlg.h Ex4/Lru.cpp Ex4/Lru.h Ex4/Lfu.cpp Ex4/Lfu.h Ex4/Fbr.cpp
        Ex4/Fbr.h Ex4/CacheFS.cpp Ex4/blc_data.cpp Ex4/blc_data.h)
add_executable(Ex4 ${SOURCE_FILES})

set(SOURCE_FILES1 Ex4/main.cpp)
add_executable(main ${SOURCE_FILES1})

set(SOURCE_FILES2 Ex4/TEST.cpp Ex4/CacheFS.h Ex4/CacheAlg.cpp Ex4/CacheAlg.h Ex4/Lfu.cpp Ex4/Lfu.h  Ex4/CacheFS.cpp)
add_executable(tests ${SOURCE_FILES2})