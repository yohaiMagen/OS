cmake_minimum_required(VERSION 3.6)
project(Ex5)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp utilities.cpp )
add_executable(main ${SOURCE_FILES})

set(SOURCE_FILES1 Server.cpp utilities.cpp )
add_executable(server ${SOURCE_FILES1})

set(SOURCE_FILES2 Client.cpp utilities.cpp whatsapp_exception.h)
add_executable(client ${SOURCE_FILES2})