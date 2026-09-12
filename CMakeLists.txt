cmake_minimum_required(VERSION 4.3)
project(basiclang)

set(CMAKE_CXX_STANDARD 26)

add_executable(basiclang main.cpp
        logger.h
        lexer.cpp
        lexer.h
)
