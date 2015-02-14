cmake_minimum_required(VERSION 2.8.4)
project(TGA)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp tga.cpp)
add_executable(TGA ${SOURCE_FILES})
