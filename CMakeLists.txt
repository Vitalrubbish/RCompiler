cmake_minimum_required(VERSION 3.22)
project(RCompiler)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.70.0 REQUIRED COMPONENTS regex)

# 如果成功找到了 Boost
if(Boost_FOUND)
    # 打印一条消息
    message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Libraries: ${Boost_LIBRARIES}")

    # 将 Boost 的头文件目录添加到项目的包含路径中
    include_directories(${Boost_INCLUDE_DIRS})
endif()

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

# 将找到的 Boost 库链接到你的可执行文件
# Boost::regex 是一个现代 CMake “目标”，它会自动处理头文件和库文件的链接
target_link_libraries(RCompiler PRIVATE Boost::regex)