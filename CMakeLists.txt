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
        error/Error.h
        include/Parser/Parser.h
        src/Parser/Parser.cpp
        include/AST/ASTNode.h
        src/AST/ASTNode.cpp
        util/Position.h
        include/AST/ASTVisitor.h
        util/Util.h
        src/AST/ASTVisitor.cpp
        include/AST/Type.h
)
