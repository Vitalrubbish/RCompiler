#ifndef ERROR_H
#define ERROR_H
#include <string>
#include <exception>
#include <utility>
#include <iostream>
#include "Position.h"
class LexError : public std::exception {
public:
    explicit LexError(std::string msg) : message(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

class ParseError : public std::exception {
public:

    explicit ParseError(std::string msg, Position pos) : message(std::move(msg)), pos(pos) {}

    [[nodiscard]] const char* what() const noexcept override {
        std::cout << pos.GetRow() << ": ";
        return message.c_str();
    }

private:
    Position pos;
    std::string message;
};


class SemanticError : public std::exception {
public:
    explicit SemanticError(std::string msg, Position pos) : message(std::move(msg)), pos(pos) {}

    [[nodiscard]] const char* what() const noexcept override {
        std::cout << pos.GetRow() << ": ";
        return message.c_str();
    }

private:
    Position pos;
    std::string message;
};
#endif //ERROR_H
