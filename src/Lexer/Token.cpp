#include "Lexer/Token.h"

Token::Token(const std::string &str, const TokenType &type) {
    this->token = str;
    this->type = type;
}
