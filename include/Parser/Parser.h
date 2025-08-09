#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include "../Lexer/Lexer.h"
#include "../AST/ASTNode.h"


class Parser {
    std::vector<Token> tokens;
    uint32_t parseIndex = 0;
public:
    Parser() = default;

    explicit Parser(const std::vector<Token> &tokens) {
        this -> tokens = tokens;
    }

    void ConsumeString(const std::string& );

    CrateNode* ParseCrate();

    VisItemNode* ParseVisItem();

    // bool ParseModule();

    FunctionNode* ParseFunction();

    // bool ParseStruct();

    // bool ParseEnumeration();

    // bool ParseConstantItem();

    // bool ParseTrait();

    // bool ParseImplementation();

    // bool ParseFunctionQualifiers();

    // bool ParseGenericParams();

    // FunctionParametersNode* ParseFunctionParameters();

    // TypeNode* ParseFunctionReturnType();

    BlockExpressionNode* ParseBlockExpression();

    StatementsNode* ParseStatements();

    StatementNode* ParseStatement();

    ExpressionNode* ParseExpressionWithoutBlock();
};
#endif //PARSER_H
