cmake_minimum_required(VERSION 3.21)
project(Matrices_multiplication C)

set(CMAKE_C_STANDARD 99)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
add_executable(Matrices_multiplication main.c)
