#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include "../Lexer/Lexer.h"
#include "../AST/ASTNode.h"


class Parser {
    std::vector<Token> tokens;
    uint32_t parseIndex = 0;

    void ConsumeString(const std::string& );

public:
    Parser() = default;

    explicit Parser(const std::vector<Token> &tokens) {
        this -> tokens = tokens;
    }

    /****************  Items  ****************/
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

    /****************  Expression  ****************/
    ExpressionNode* ParseExpression();

    ExpressionNode* ParseExpressionWithBlock();

    BlockExpressionNode* ParseBlockExpression();

    BlockExpressionNode* ParseConstBlockExpression();

    InfiniteLoopExpressionNode* ParseInfiniteLoopExpression();

    PredicateLoopExpressionNode* ParsePredicateLoopExpression();

    IfExpressionNode* ParseIfExpression();

    MatchExpressionNode* ParseMatchExpression();

    ExpressionNode* ParseExpressionWithoutBlock();

    ExpressionNode* ParseTupleExpression();

    ExpressionNode* ParseJumpExpression();

    ExpressionNode* ParseAssignmentExpression();

    ExpressionNode* ParseLogicalOrExpression();

    ExpressionNode* ParseLogicalAndExpression();

    ExpressionNode* ParseComparisonExpression();

    ExpressionNode* ParseBitwiseOrExpression();

    ExpressionNode* ParseBitwiseXorExpression();

    ExpressionNode* ParseBitwiseAndExpression();

    ExpressionNode* ParseShiftExpression();

    ExpressionNode* ParseAddMinusExpression();

    ExpressionNode* ParseMulDivModExpression();

    ExpressionNode* ParseTypeCastExpression();

    ExpressionNode* ParseUnaryExpression();

    ExpressionNode* ParseCallExpression();

    ExpressionNode* ParsePrimaryExpression();

    ExpressionNode* ParseLiteral();

    PathExpressionNode* ParsePathExpression();

    StatementsNode* ParseStatements();

    ConditionsNode* ParseConditions();

    /****************  Statement  ****************/
    StatementNode* ParseStatement();

    LetStatementNode* ParseLetStatement();

    ExpressionStatementNode* ParseExpressionStatement();

    /****************  Patterns  ****************/
    PatternNoTopAltNode* ParsePatternNoTopAlt();

    PatternWithoutRangeNode* ParsePatternWithoutRange();

    LiteralPatternNode* ParseLiteralPattern();

    IdentifierPatternNode* ParseIdentifierPattern();

    SlicePatternNode* ParseSlicePattern();

    PathPatternNode* ParsePathPattern();
    /****************  Types  ****************/
    TypeNode* ParseType();

    TypeNoBoundsNode* ParseTypeNoBounds();

    ParenthesizedTypeNode* ParseParenthesizedType();

    TypePathNode* ParseTypePath();

    TypePathSegmentNode* ParseTypePathSegment();

    PathIndentSegmentNode* ParsePathIndentSegment();

    TupleType* ParseTupleType();

    ArrayType* ParseArrayType();

    SliceTypeNode* ParseSliceType();

    InferredType* ParseInferredType();

    /****************  Paths  ****************/
    SimplePathSegment* ParseSimplePathSegment();
};
#endif //PARSER_H
