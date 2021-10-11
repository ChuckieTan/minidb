cmake_minimum_required(VERSION 3.14)

project(minidb)

# Set C++17
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include(spdlog)

# Add sources
file(GLOB sources
    "${PROJECT_SOURCE_DIR}/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF()

add_executable(minidb "${sources}")
target_include_directories(minidb PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(minidb PRIVATE spdlog)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()