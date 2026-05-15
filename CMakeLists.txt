cmake_minimum_required(VERSION 3.28)
project(sim C)

set(CMAKE_C_STANDARD 11)

add_executable(sim src/main.c
        src/utils/logger.c
        src/utils/logger.h)
