#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory> // Required for std::shared_ptr
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
    std::shared_ptr<CrateNode> ParseCrate();
    std::shared_ptr<VisItemNode> ParseVisItem();
    std::shared_ptr<FunctionNode> ParseFunction();
    std::shared_ptr<StructNode> ParseStruct();
    std::shared_ptr<StructFieldNode> ParseStructFieldNode();
    std::shared_ptr<EnumerationNode> ParseEnumeration();
    std::shared_ptr<EnumVariantNode> ParseEnumVariant();
    std::shared_ptr<EnumVariantStructNode> ParseEnumVariantStruct();
    std::shared_ptr<EnumVariantDiscriminantNode> ParseEnumVariantDiscriminant();
    std::shared_ptr<ConstantItemNode> ParseConstantItem();
    std::shared_ptr<AssociatedItemNode> ParseAssociatedItem();
    std::shared_ptr<TraitNode> ParseTrait();
    std::shared_ptr<ImplementationNode> ParseImplementation();
    std::shared_ptr<InherentImplNode> ParseInherentImpl();
    std::shared_ptr<TraitImplNode> ParseTraitImpl();
    std::shared_ptr<TypeNode> ParseFunctionReturnType();
    std::shared_ptr<FunctionParametersNode> ParseFunctionParameters();
    std::shared_ptr<FunctionParamNode> ParseFunctionParam();
    std::shared_ptr<FunctionParamPatternNode> ParseFunctionParamPattern();

    /****************  Expression  ****************/
    std::shared_ptr<ExpressionNode> ParseExpression();
    std::shared_ptr<ExpressionNode> ParseExpressionWithBlock();
    std::shared_ptr<BlockExpressionNode> ParseBlockExpression();
    std::shared_ptr<BlockExpressionNode> ParseConstBlockExpression();
    std::shared_ptr<InfiniteLoopExpressionNode> ParseInfiniteLoopExpression();
    std::shared_ptr<PredicateLoopExpressionNode> ParsePredicateLoopExpression();
    std::shared_ptr<IfExpressionNode> ParseIfExpression();
    std::shared_ptr<MatchExpressionNode> ParseMatchExpression();
    std::shared_ptr<ExpressionNode> ParseExpressionWithoutBlock();
    std::shared_ptr<ExpressionNode> ParseTupleExpression();
    std::shared_ptr<ExpressionNode> ParseJumpExpression();
    std::shared_ptr<ExpressionNode> ParseAssignmentExpression();
    std::shared_ptr<ExpressionNode> ParseLogicalOrExpression();
    std::shared_ptr<ExpressionNode> ParseLogicalAndExpression();
    std::shared_ptr<ExpressionNode> ParseComparisonExpression();
    std::shared_ptr<ExpressionNode> ParseBitwiseOrExpression();
    std::shared_ptr<ExpressionNode> ParseBitwiseXorExpression();
    std::shared_ptr<ExpressionNode> ParseBitwiseAndExpression();
    std::shared_ptr<ExpressionNode> ParseShiftExpression();
    std::shared_ptr<ExpressionNode> ParseAddMinusExpression();
    std::shared_ptr<ExpressionNode> ParseMulDivModExpression();
    std::shared_ptr<ExpressionNode> ParseTypeCastExpression();
    std::shared_ptr<ExpressionNode> ParseUnaryExpression();
    std::shared_ptr<ExpressionNode> ParseCallExpression();
    std::shared_ptr<ExpressionNode> ParsePrimaryExpression();
    std::shared_ptr<StructExpressionNode> ParseStructExpression();
    std::shared_ptr<StructExprFieldsNode> ParseStructExprFields();
    std::shared_ptr<StructExprFieldNode> ParseStructExprField();
    std::shared_ptr<StructBaseNode> ParseStructBase();
    std::shared_ptr<ExpressionNode> ParseLiteral();
    std::shared_ptr<PathExpressionNode> ParsePathExpression();
    std::shared_ptr<PathInExpressionNode> ParsePathInExpression();
    std::shared_ptr<StatementsNode> ParseStatements();
    std::shared_ptr<ConditionsNode> ParseConditions();
    std::shared_ptr<MatchArmsNode> ParseMatchArms();
    std::shared_ptr<MatchArmNode> ParseMatchArm();
    std::shared_ptr<LetChainNode> ParseLetChain();
    std::shared_ptr<LetChainConditionNode> ParseLetChainCondition();

    /****************  Statement  ****************/
    std::shared_ptr<StatementNode> ParseStatement();
    std::shared_ptr<LetStatementNode> ParseLetStatement();
    std::shared_ptr<ExpressionStatementNode> ParseExpressionStatement();

    /****************  Patterns  ****************/
    std::shared_ptr<PatternNode> ParsePattern();
    std::shared_ptr<PatternNoTopAltNode> ParsePatternNoTopAlt();
    std::shared_ptr<PatternWithoutRangeNode> ParsePatternWithoutRange();
    std::shared_ptr<LiteralPatternNode> ParseLiteralPattern();
    std::shared_ptr<IdentifierPatternNode> ParseIdentifierPattern();
    std::shared_ptr<SlicePatternNode> ParseSlicePattern();
    std::shared_ptr<PathPatternNode> ParsePathPattern();

    /****************  Types  ****************/
    std::shared_ptr<TypeNode> ParseType();
    std::shared_ptr<TypeNoBoundsNode> ParseTypeNoBounds();
    std::shared_ptr<ParenthesizedTypeNode> ParseParenthesizedType();
    std::shared_ptr<TypePathNode> ParseTypePath();
    std::shared_ptr<TypePathSegmentNode> ParseTypePathSegment();
    std::shared_ptr<TupleTypeNode> ParseTupleType();
    std::shared_ptr<ArrayTypeNode> ParseArrayType();
    std::shared_ptr<SliceTypeNode> ParseSliceType();

    /****************  Paths  ****************/
    std::shared_ptr<PathIndentSegmentNode> ParsePathIndentSegment();
};

#endif //PARSER_H