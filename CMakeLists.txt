# cmake 最低版本需求
cmake_minimum_required(VERSION 3.13)

# 工程名称
project(minidb)

# 设置 C++17
set(CMAKE_CXX_STANDARD 17)

# 编译源码生成目标
add_executable(minidb main.cpp src/repl/InputBuffer.cpp src/repl/InputBuffer.h)