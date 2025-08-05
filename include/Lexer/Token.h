#ifndef TOKEN_H
#define TOKEN_H
#include <string>
#include "TokenType.h"


struct Token {
    std::string token{};
    TokenType type{};

    Token() = default;

    explicit Token(const std::string& , const TokenType& );
};
#endif //TOKEN_H
