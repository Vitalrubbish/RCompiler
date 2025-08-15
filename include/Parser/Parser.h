#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include "Lexer/Lexer.h"
#include "Semantic/ASTNode.h"


class Parser {
    std::vector<Token> tokens;
    uint32_t parseIndex = 0;

    void ConsumeString(const std::string &);

public:
    Parser() = default;

    explicit Parser(const std::vector<Token> &tokens) {
        this->tokens = tokens;
    }

    /****************  Items  ****************/
    CrateNode *ParseCrate();

    VisItemNode *ParseVisItem();

    FunctionNode *ParseFunction();

    StructNode *ParseStruct();

    StructFieldNode *ParseStructFieldNode();

    EnumerationNode *ParseEnumeration();

    EnumVariantNode *ParseEnumVariant();

    EnumVariantStructNode *ParseEnumVariantStruct();

    EnumVariantDiscriminantNode *ParseEnumVariantDiscriminant();

    ConstantItemNode *ParseConstantItem();

    AssociatedItemNode *ParseAssociatedItem();

    // bool ParseTrait();

    ImplementationNode *ParseImplementation();

    InherentImplNode *ParseInherentImpl();

    TypeNode *ParseFunctionReturnType();

    FunctionParametersNode *ParseFunctionParameters();

    FunctionParamNode *ParseFunctionParam();

    FunctionParamPatternNode *ParseFunctionParamPattern();

    /****************  Expression  ****************/
    ExpressionNode *ParseExpression();

    ExpressionNode *ParseExpressionWithBlock();

    BlockExpressionNode *ParseBlockExpression();

    BlockExpressionNode *ParseConstBlockExpression();

    InfiniteLoopExpressionNode *ParseInfiniteLoopExpression();

    PredicateLoopExpressionNode *ParsePredicateLoopExpression();

    IfExpressionNode *ParseIfExpression();

    MatchExpressionNode *ParseMatchExpression();

    ExpressionNode *ParseExpressionWithoutBlock();

    ExpressionNode *ParseTupleExpression();

    ExpressionNode *ParseJumpExpression();

    ExpressionNode *ParseAssignmentExpression();

    ExpressionNode *ParseLogicalOrExpression();

    ExpressionNode *ParseLogicalAndExpression();

    ExpressionNode *ParseComparisonExpression();

    ExpressionNode *ParseBitwiseOrExpression();

    ExpressionNode *ParseBitwiseXorExpression();

    ExpressionNode *ParseBitwiseAndExpression();

    ExpressionNode *ParseShiftExpression();

    ExpressionNode *ParseAddMinusExpression();

    ExpressionNode *ParseMulDivModExpression();

    ExpressionNode *ParseTypeCastExpression();

    ExpressionNode *ParseUnaryExpression();

    ExpressionNode *ParseCallExpression();

    ExpressionNode *ParsePrimaryExpression();

    StructExpressionNode *ParseStructExpression();

    StructExprFieldsNode *ParseStructExprFields();

    StructExprFieldNode *ParseStructExprField();

    StructBaseNode *ParseStructBase();

    ExpressionNode *ParseLiteral();

    PathExpressionNode *ParsePathExpression();

    PathInExpressionNode *ParsePathInExpression();

    StatementsNode *ParseStatements();

    ConditionsNode *ParseConditions();

    MatchArmsNode *ParseMatchArms();

    MatchArmNode *ParseMatchArm();

    /****************  Statement  ****************/
    StatementNode *ParseStatement();

    LetStatementNode *ParseLetStatement();

    ExpressionStatementNode *ParseExpressionStatement();

    /****************  Patterns  ****************/
    PatternNode *ParsePattern();

    PatternNoTopAltNode *ParsePatternNoTopAlt();

    PatternWithoutRangeNode *ParsePatternWithoutRange();

    LiteralPatternNode *ParseLiteralPattern();

    IdentifierPatternNode *ParseIdentifierPattern();

    SlicePatternNode *ParseSlicePattern();

    PathPatternNode *ParsePathPattern();

    /****************  Types  ****************/
    TypeNode *ParseType();

    TypeNoBoundsNode *ParseTypeNoBounds();

    ParenthesizedTypeNode *ParseParenthesizedType();

    TypePathNode *ParseTypePath();

    TypePathSegmentNode *ParseTypePathSegment();

    TupleTypeNode *ParseTupleType();

    ArrayTypeNode *ParseArrayType();

    SliceTypeNode *ParseSliceType();

    InferredTypeNode *ParseInferredType();

    /****************  Paths  ****************/
    PathIndentSegmentNode *ParsePathIndentSegment();
};
#endif //PARSER_H
