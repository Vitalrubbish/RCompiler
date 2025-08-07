#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include "../Lexer/Lexer.h"

class Parser {
    std::vector<Token> tokens;
    uint32_t parseIndex = 0;
public:
    Parser() = default;

    explicit Parser(const std::vector<Token> &tokens) {
        this -> tokens = tokens;
    }

    bool ParseCrate();

    bool ParseVisItem();

    bool ParseModule();

    bool ParseFunction();

    bool ParseStruct();

    bool ParseEnumeration();

    bool ParseConstantItem();

    bool ParseTrait();

    bool ParseImplementation();

    bool ParseFunctionQualifiers();

    bool ParseGenericParams();

    bool ParseFunctionParameter();

    bool ParseFunctionReturnType();


};
#endif //PARSER_H
