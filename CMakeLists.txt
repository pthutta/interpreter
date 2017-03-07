cmake_minimum_required(VERSION 3.4)
project(Interpreter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(./bricks)

set(SOURCE_FILES
     main.cpp)

add_executable(Interpreter ${SOURCE_FILES})