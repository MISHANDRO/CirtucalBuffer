cmake_minimum_required(VERSION 3.24)

project(CirtucalBuffer)

include_directories("include")

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(tests)

add_executable(CirtucalBuffer main.cpp
               include/Iterator.h
               include/CirtucalBuffer.h
               include/Allocator.h
        )

