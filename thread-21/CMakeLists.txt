cmake_minimum_required(VERSION 3.5)
project(thread_21)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp main.h)
add_executable(thread_21 ${SOURCE_FILES})