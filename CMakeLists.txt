cmake_minimum_required(VERSION 3.22)
project(RCompiler)

set(CMAKE_C_STANDARD 11)

# 添加所有需要的头文件目录
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/error
        ${CMAKE_SOURCE_DIR}/util
)

add_executable(RCompiler
        main.cpp
        include/Lexer/Token.h
        src/Lexer/Token.cpp
        include/Lexer/Lexer.h
        src/Lexer/Lexer.cpp
        include/Lexer/TokenType.h
        error/Error.h
        include/Parser/Parser.h
        src/Parser/Parser.cpp
        include/Semantic/ASTNode.h
        src/Semantic/ASTNode.cpp
        util/Position.h
        include/Semantic/ASTVisitor.h
        util/Util.h
        src/Semantic/ASTVisitor.cpp
        include/Semantic/Type.h
        include/Semantic/Symbol.h
        include/Semantic/Scope.h
        include/Semantic/ScopeManager.h
        include/Semantic/TypeManager.h
)

# 添加编译选项
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 如果是Debug模式，添加调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g)
endif()