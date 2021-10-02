cmake_minimum_required(VERSION 3.13)

project(minidb)

# Set C++17
set(CMAKE_CXX_STANDARD 17)

# Specify include Directory
include_directories("${PROJECT_SOURCE_DIR}/include")

# Add sources
file(GLOB sources
    "${PROJECT_SOURCE_DIR}/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Set debug or release
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0")
    message("Debug mode:${CMAKE_CXX_FLAGS_DEBUG}")
    add_executable(minidb_debug ${sources})

elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
    message("Release mode:${CMAKE_CXX_FLAGS_RELEASE}")
    add_executable(minidb_release ${sources})

else()
    message("else:${CMAKE_BUILD_TYPE}")
    message("else:${CMAKE_CXX_FLAGS_RELEASE}")
    add_executable(minidb_release ${sources})
endif()
