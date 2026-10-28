cmake_minimum_required(VERSION 3.25)
project(basiclang)

set(CMAKE_CXX_STANDARD 26)

# this part is vibecoded
include(FetchContent)

FetchContent_Declare(
        boost_config
        GIT_REPOSITORY https://github.com/boostorg/config.git
        GIT_TAG        boost-1.85.0
        GIT_SHALLOW    ON
)

FetchContent_Declare(
        boost_assert
        GIT_REPOSITORY https://github.com/boostorg/assert.git
        GIT_TAG        boost-1.85.0
        GIT_SHALLOW    ON
)

FetchContent_Declare(
        boost_multiprecision
        GIT_REPOSITORY https://github.com/boostorg/multiprecision.git
        GIT_TAG        boost-1.85.0
        GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(boost_config boost_assert boost_multiprecision)


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

# this part is vibecoded too
target_compile_definitions(basiclang PRIVATE BOOST_MP_STANDALONE)
target_link_libraries(basiclang PRIVATE boost_multiprecision boost_config boost_assert)