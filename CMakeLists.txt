cmake_minimum_required(VERSION 4.3)
project(basiclang)

set(CMAKE_CXX_STANDARD 26)

# this part is vibecoded
include(FetchContent)
foreach(dep multiprecision config assert)
    FetchContent_Declare(boost_${dep}
            GIT_REPOSITORY https://github.com{dep}.git
            GIT_TAG        boost-1.85.0
            GIT_SHALLOW    ON
    )
endforeach()
FetchContent_MakeAvailable(boost_multiprecision boost_config boost_assert)


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

target_include_directories(basiclang PRIVATE
        ${boost_multiprecision_SOURCE_DIR}/include
        ${boost_config_SOURCE_DIR}/include
        ${boost_assert_SOURCE_DIR}/include
)

target_compile_definitions(basiclang PRIVATE BOOST_MP_STANDALONE)