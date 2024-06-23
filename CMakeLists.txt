cmake_minimum_required(VERSION 3.5)                 # 定义cmake的最低版本要求

project(skiplist)                                   # 定义项目的名称

set(CMAKE_BUILD_TYPE "Debug")                       # 构建Debug模式
# set(CMAKE_BUILD_TYPE "Release")                     # 构建Release模式

include_directories(include)                        # 项目所用到的需要包含的头文件目录，#include时会以这些目录为根路径查找头文件

add_executable(main src/main.cpp)        # 添加要编译的可执行文件

message(main)

add_executable(stress_test test/stress_test.cpp)

message(stress_test)

add_executable(create_node_test test/create_node_test.cpp)

message(create_node_test)