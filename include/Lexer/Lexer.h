#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include <boost/regex.hpp>
#include <utility>
#include "Token.h"

class Lexer {
    std::vector<std::pair<TokenType, boost::regex> > letter_rules;
    std::vector<std::pair<TokenType, boost::regex> > non_letter_rules;
    std::vector<std::pair<TokenType, boost::regex> > string_rules;

public:
    Lexer();

    [[nodiscard]] Token GetNextToken(std::string &str) const;
};
#endif //LEXER_H
