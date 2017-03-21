cmake_minimum_required(VERSION 3.4)
project(Interpreter)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(./bricks)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)