cmake_minimum_required(VERSION 3.6)
project(Ex4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Ex4/main.cpp Ex4/CacheFS.h Ex4/CacheAlg.cpp Ex4/CacheAlg.h Ex4/Lru.cpp Ex4/Lru.h Ex4/Lfu.cpp Ex4/Lfu.h Ex4/Fbr.cpp
        Ex4/Fbr.h Ex4/CacheFS.cpp)
add_executable(Ex4 ${SOURCE_FILES})

set(SOURCE_FILES1 Ex5/main.cpp Ex5/utilities.cpp Ex5/utilities.h)
add_executable(main ${SOURCE_FILES1})

set(SOURCE_FILES5 Ex5/Server.cpp)
add_executable(server ${SOURCE_FILES5})

set(SOURCE_FILES6 Ex5/Client.cpp)
add_executable(client ${SOURCE_FILES6})

set(SOURCE_FILES2 Ex4/TEST.cpp Ex4/CacheFS.h Ex4/CacheFS.cpp Ex4/CacheAlg.cpp Ex4/CacheAlg.h Ex4/Lru.cpp Ex4/Lru.h Ex4/Lfu.cpp
        Ex4/Lfu.h Ex4/Fbr.cpp Ex4/Fbr.h)

add_executable(test5 ${SOURCE_FILES2})