#ifndef TOKEN_H
#define TOKEN_H
#include <string>
#include "TokenType.h"
#include "Position.h"


struct Token {
    std::string token{};
    TokenType type{};
    Position pos{};

    Token() = default;

    explicit Token(const std::string &, const TokenType &);

    void putPosValue(const Position &pos) {
        this->pos = pos;
    }
};
#endif //TOKEN_H
