#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include <regex>
#include <utility>
#include "Token.h"

class Lexer {
    std::vector<std::pair<TokenType, std::regex>> letter_rules;
    std::vector<std::pair<TokenType, std::regex>> non_letter_rules;
public:

    Lexer();

    [[nodiscard]] Token GetNextToken(std::string& str) const;
};
#endif //LEXER_H
