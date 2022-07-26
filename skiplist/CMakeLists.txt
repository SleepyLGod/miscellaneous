cmake_minimum_required(VERSION 3.20)

# project name
project(Skiplist VERSION 1.0)
# configure_file的作用将一份文件拷贝到另一个位置并修改它的内容，使得在代码中使用CMake中定义的变量 
# specify the C++ standard 
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

set(CMAKE_CXX_COMPILER "g++")
include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(./src SrcFiles)
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build)
include_directories(([AFTER|BEFORE] [SYSTEM] node.h skiplist.h utils.h))
add_executable(Skiplist ./src/main.cpp)