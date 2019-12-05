cmake_minimum_required(VERSION 3.15)
project(semaphor C)

set(CMAKE_C_STANDARD 11)

add_executable(semaphor main.c list.h utils.h)
target_link_libraries(semaphor pthread rt)