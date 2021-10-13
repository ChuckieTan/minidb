cmake_minimum_required(VERSION 3.14)

project(minidb)

# Set C++17
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILECOMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include(spdlog)

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF()

add_subdirectory(util)
add_subdirectory(parser)
add_subdirectory(ast)

add_executable(minidb main.cpp)

target_link_libraries(minidb PRIVATE
    spdlog::spdlog
    minidb::util
    minidb::parser
    # minidb::ast
)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()