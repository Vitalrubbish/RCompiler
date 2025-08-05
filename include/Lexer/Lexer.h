#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include <utility>
#include "Token.h"

class Lexer {
    std::vector<std::pair<TokenType, std::string>> rules;
public:

    Lexer();

    [[nodiscard]] Token GetNextToken(std::string& str) const;
};
#endif //LEXER_H
