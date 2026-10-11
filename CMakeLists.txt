cmake_minimum_required(VERSION 4.3)
project(basiclang)

set(CMAKE_CXX_STANDARD 26)

find_package(Boost REQUIRED)


add_executable(basiclang main.cpp
        logger.h
        lexer.cpp
        lexer.h
        parser.cpp
        parser.h
        ast.h
        bytecode.cpp
        bytecode.h
)

target_link_libraries(basiclang PRIVATE Boost::headers)

# 4. Keep Standalone Mode active so it stays decoupled from compiled binaries
target_compile_definitions(basiclang PRIVATE BOOST_MP_STANDALONE)