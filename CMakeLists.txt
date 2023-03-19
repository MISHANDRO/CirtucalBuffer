cmake_minimum_required(VERSION 3.24)
project(CirtucalBuffer)

set(CMAKE_CXX_STANDARD 23)

include_directories("include")

add_executable(CirtucalBuffer main.cpp
        include/CirtucalBuffer.h
        include/Iterator.h
        )
