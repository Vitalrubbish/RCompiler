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
