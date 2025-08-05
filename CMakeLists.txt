cmake_minimum_required(VERSION 3.22)
project(RCompiler)

set(CMAKE_C_STANDARD 11)

include_directories(RCompiler)

add_executable(RCompiler
        main.cpp
        include/Lexer/Token.h
        src/Lexer/Token.cpp
        include/Lexer/Lexer.h
        src/Lexer/Lexer.cpp
        include/Lexer/TokenType.h
        include/Preprocessor/Preprocessor.h
        src/Preprocessor/Preprocessor.cpp
        error/Error.h
)
