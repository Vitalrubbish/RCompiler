cmake_minimum_required(VERSION 3.22)
project(RCompiler)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 修改包含目录的方式
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/error
        ${PROJECT_SOURCE_DIR}/util
)

# 收集源文件
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "src/*/*.cpp"
        "main.cpp"
)

# 收集头文件
file(GLOB_RECURSE HEADERS
        "include/*.h"
        "error/*.h"
        "util/*.h"
)

# 添加可执行文件
add_executable(RCompiler ${SOURCES} ${HEADERS})

# 调试模式设置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g)
endif()