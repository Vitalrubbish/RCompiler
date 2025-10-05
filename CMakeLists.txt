cmake_minimum_required(VERSION 3.22)
project(RCompiler)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.70.0 REQUIRED COMPONENTS regex)

if(Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Libraries: ${Boost_LIBRARIES}")
endif()

# 收集源文件
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "src/*/*.cpp"
        "main.cpp"
)

# 收集头文件（通常不需要将头文件添加到可执行文件中）
file(GLOB_RECURSE HEADERS
        "include/*.h"
        "error/*.h"
        "util/*.h"
)

# 添加可执行文件
add_executable(RCompiler ${SOURCES})

# 使用现代 CMake 方式设置包含目录
target_include_directories(RCompiler PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}      # 项目根目录
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/error
        ${CMAKE_CURRENT_SOURCE_DIR}/util
)

# 如果有 Boost 头文件，也使用目标属性方式
if(Boost_FOUND)
    target_link_libraries(RCompiler PRIVATE Boost::regex)
endif()

# 调试模式设置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(RCompiler PRIVATE -g)
endif()