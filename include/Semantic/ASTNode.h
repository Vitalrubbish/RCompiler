#ifndef ASTNODE_H
#define ASTNODE_H
#include <vector>
#include <string>
#include "Position.h"
#include "Lexer/TokenType.h"
#include "Lexer/Token.h"
#include "Semantic/ASTVisitor.h"

// Forward declarations
class ASTVisitor;
class ASTNode;
class CrateNode;
class VisItemNode;
class FunctionNode;
class StructNode;
class EnumerationNode;
class ConstantItemNode;
class TraitNode;
class ImplementationNode;
class FunctionParametersNode;
class FunctionParamNode;
class TypeNode;
class FunctionParamPatternNode;
class StructFieldNode;
class EnumVariantNode;
class EnumVariantStructNode;
class EnumVariantDiscriminantNode;
class AssociatedItemNode;
class InherentImplNode;
class TraitImplNode;
class TypePathNode;
class StatementNode;
class EmptyStatementNode;
class LetStatementNode;
class ExpressionStatementNode;
class ExpressionNode;
class ExpressionWithoutBlockNode;
class ExpressionWithBlockNode;
class LiteralExpressionNode;
class CharLiteralNode;
class StringLiteralNode;
class CStringLiteralNode;
class IntLiteralNode;
class BoolLiteralNode;
class PathExpressionNode;
class GroupedExpressionNode;
class TupleExpressionNode;
class StructExpressionNode;
class ContinueExpressionNode;
class UnderscoreExpressionNode;
class BlockExpressionNode;
class LoopExpressionNode;
class IfExpressionNode;
class MatchExpressionNode;
class MatchArmsNode;
class PathInExpressionNode;
class StatementsNode;
class ComparisonExpressionNode;
class TypeCastExpressionNode;
class AssignmentExpressionNode;
class StructExprFieldNode;
class ConditionsNode;
class LetChainNode;
class PatternNode;
class PatternNoTopAltNode;
class InfiniteLoopExpressionNode;
class PredicateLoopExpressionNode;
class JumpExpressionNode;
class LogicOrExpressionNode;
class LogicAndExpressionNode;
class BitwiseOrExpressionNode;
class BitwiseXorExpressionNode;
class BitwiseAndExpressionNode;
class ShiftExpressionNode;
class AddMinusExpressionNode;
class MulDivModExpressionNode;
class UnaryExpressionNode;
class FunctionCallExpressionNode;
class ArrayIndexExpressionNode;
class MemberAccessExpressionNode;
class StructExprFieldsNode;
class StructBaseNode;
class PathIndentSegmentNode;
class ArrayLiteralNode;
class MatchArmNode;
class MatchArmGuardNode;
class PatternWithoutRangeNode;
class LiteralPatternNode;
class IdentifierPatternNode;
class WildcardPatternNode;
class RestPatternNode;
class GroupedPatternNode;
class SlicePatternNode;
class PathPatternNode;
class TypeNoBoundsNode;
class ParenthesizedTypeNode;
class TypePathSegmentNode;
class TupleTypeNode;
class ArrayTypeNode;
class SliceTypeNode;
class InferredTypeNode;

class ASTNode {
public:
    Position pos_;

    explicit ASTNode(const Position pos) {
        pos_ = pos;
    }

    virtual ~ASTNode() = default;

    virtual void accept(ASTVisitor *visitor) = 0;
};

/****************  Path (Naive Version) ****************/
class PathIndentSegmentNode : public ASTNode {
public:
    TokenType type_;
    std::string identifier_;

    PathIndentSegmentNode(Position pos, TokenType type, const std::string &identifier): ASTNode(pos) {
        type_ = type;
        identifier_ = identifier;
    }

    ~PathIndentSegmentNode() override = default;

    [[nodiscard]] std::string toString() const {
        return identifier_;
    }

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

/****************  Items  ****************/
class CrateNode : public ASTNode {
public:
    std::vector<VisItemNode *> items_;

    CrateNode(Position pos, const std::vector<VisItemNode *> &items): ASTNode(pos) {
        items_ = items;
    }

    ~CrateNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class VisItemNode : public ASTNode {
public:
    explicit VisItemNode(Position pos): ASTNode(pos) {
    }

    ~VisItemNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class FunctionNode : public VisItemNode {
public:
    bool is_const_;
    std::string identifier_;
    FunctionParametersNode *function_parameters_ = nullptr;
    TypeNode *type_ = nullptr;
    BlockExpressionNode *block_expression_ = nullptr;

    FunctionNode(Position pos, bool is_const, const std::string &identifier,
                 FunctionParametersNode *function_parameters, TypeNode *type,
                 BlockExpressionNode *block_expression): VisItemNode(pos) {
        is_const_ = is_const;
        identifier_ = identifier;
        function_parameters_ = function_parameters;
        type_ = type;
        block_expression_ = block_expression;
    }

    ~FunctionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class FunctionParametersNode : public ASTNode {
public:
    std::vector<FunctionParamNode *> function_params_;

    FunctionParametersNode(Position pos, const std::vector<FunctionParamNode *> &function_params): ASTNode(pos) {
        function_params_ = function_params;
    }

    ~FunctionParametersNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class FunctionParamNode : public ASTNode {
public:
    PatternNoTopAltNode *pattern_no_top_alt_node_ = nullptr;
    TypeNode *type_ = nullptr;
    bool is_DotDotDot_ = false;

    FunctionParamNode(Position pos, PatternNoTopAltNode *function_param_pattern,
                      TypeNode *type, bool is_DotDotDot): ASTNode(pos) {
        pattern_no_top_alt_node_ = function_param_pattern;
        type_ = type;
        is_DotDotDot_ = is_DotDotDot;
    }

    ~FunctionParamNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class FunctionParamPatternNode : public ASTNode {
public:
    PatternNoTopAltNode *pattern_no_top_alt_;
    TypeNode *type_;
    bool is_DotDotDot_ = false;

    FunctionParamPatternNode(Position pos, PatternNoTopAltNode *pattern_no_top_alt,
                             TypeNode *type, bool is_DotDotDot): ASTNode(pos) {
        pattern_no_top_alt_ = pattern_no_top_alt;
        type_ = type;
        is_DotDotDot_ = is_DotDotDot;
    }

    ~FunctionParamPatternNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StructNode : public VisItemNode {
public:
    std::string identifier_;
    std::vector<StructFieldNode *> struct_field_nodes_;

    StructNode(Position pos, const std::string &identifier,
               const std::vector<StructFieldNode *> &struct_field_nodes): VisItemNode(pos) {
        identifier_ = identifier;
        struct_field_nodes_ = struct_field_nodes;
    }

    ~StructNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StructFieldNode : public ASTNode {
public:
    std::string identifier_;
    TypeNode *type_node_;

    StructFieldNode(Position pos, const std::string &identifier, TypeNode *type_node): ASTNode(pos) {
        identifier_ = identifier;
        type_node_ = type_node;
    }

    ~StructFieldNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class EnumerationNode : public VisItemNode {
public:
    std::string identifier_;
    std::vector<EnumVariantNode *> enum_variant_nodes_;

    EnumerationNode(Position pos, const std::string &identifier,
                    const std::vector<EnumVariantNode *> &enum_variant_nodes): VisItemNode(pos) {
        identifier_ = identifier;
        enum_variant_nodes_ = enum_variant_nodes;
    }

    ~EnumerationNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class EnumVariantNode : public ASTNode {
public:
    std::string identifier_;
    EnumVariantStructNode *enum_variant_struct_node_ = nullptr;
    EnumVariantDiscriminantNode *enum_variant_discriminant_node_ = nullptr;

    EnumVariantNode(Position pos, const std::string &identifier,
                    EnumVariantStructNode *enum_variant_struct_node,
                    EnumVariantDiscriminantNode *enum_variant_discriminant_node): ASTNode(pos) {
        identifier_ = identifier;
        enum_variant_struct_node_ = enum_variant_struct_node;
        enum_variant_discriminant_node_ = enum_variant_discriminant_node;
    }

    ~EnumVariantNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class EnumVariantStructNode : public ASTNode {
public:
    std::vector<StructFieldNode *> struct_field_nodes_;

    EnumVariantStructNode(Position pos, const std::vector<StructFieldNode *> &struct_field_nodes): ASTNode(pos) {
        struct_field_nodes_ = struct_field_nodes;
    }

    ~EnumVariantStructNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class EnumVariantDiscriminantNode : public ASTNode {
public:
    ExpressionNode *expression_node_;

    EnumVariantDiscriminantNode(Position pos, ExpressionNode *expression_node): ASTNode(pos) {
        expression_node_ = expression_node;
    }

    ~EnumVariantDiscriminantNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ConstantItemNode : public VisItemNode {
public:
    std::string identifier_;
    bool is_underscore_ = false;
    TypeNode *type_node_;
    ExpressionNode *expression_node_;

    ConstantItemNode(Position pos, const std::string &identifier, bool is_underscore,
                     TypeNode *type_node, ExpressionNode *expression_node): VisItemNode(pos) {
        identifier_ = identifier;
        is_underscore_ = is_underscore;
        type_node_ = type_node;
        expression_node_ = expression_node;
    }

    ~ConstantItemNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class AssociatedItemNode : public VisItemNode {
public:
    ConstantItemNode *constant_item_node_;
    FunctionNode *function_node_;

    AssociatedItemNode(Position pos, ConstantItemNode *constant_item_node,
                       FunctionNode *function_node): VisItemNode(pos) {
        constant_item_node_ = constant_item_node;
        function_node_ = function_node;
    }

    ~AssociatedItemNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class TraitNode : public VisItemNode {
public:
    std::string identifier_;
    std::vector<AssociatedItemNode *> associated_item_nodes_;

    TraitNode(Position pos, const std::string &identifier,
              const std::vector<AssociatedItemNode *> &associated_item_nodes): VisItemNode(pos) {
        identifier_ = identifier;
        associated_item_nodes_ = associated_item_nodes;
    }

    ~TraitNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ImplementationNode : public VisItemNode {
public:
    explicit ImplementationNode(Position pos): VisItemNode(pos) {
    }

    ~ImplementationNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class InherentImplNode : public ImplementationNode {
public:
    TypeNode *type_node_;
    std::vector<AssociatedItemNode *> associated_item_nodes_;

    InherentImplNode(Position pos, TypeNode *type_node,
                     const std::vector<AssociatedItemNode *> &associated_item_nodes): ImplementationNode(pos) {
        type_node_ = type_node;
        associated_item_nodes_ = associated_item_nodes;
    }

    ~InherentImplNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class TraitImplNode : public ImplementationNode {
public:
    std::string identifier_;
    TypeNode *type_node_;
    std::vector<AssociatedItemNode *> associated_item_nodes_;

    TraitImplNode(Position pos, const std::string &identifier, TypeNode *type_node,
                  const std::vector<AssociatedItemNode *> &associated_item_nodes): ImplementationNode(pos) {
        identifier_ = identifier;
        type_node_ = type_node;
        associated_item_nodes_ = associated_item_nodes;
    }

    ~TraitImplNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

/****************  Expression With Block  ****************/
class ExpressionNode : public ASTNode {
public:
    std::vector<std::shared_ptr<Type>> types; // We cannot know this when building the AST
    bool is_assignable_ = false;

    ExpressionNode(Position pos, bool is_assignable): ASTNode(pos) {
        is_assignable_ = is_assignable;
    }

    ~ExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ExpressionWithBlockNode : public ExpressionNode {
public:
    explicit ExpressionWithBlockNode(Position pos): ExpressionNode(pos, false) {
    }

    ~ExpressionWithBlockNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class BlockExpressionNode : public ExpressionWithBlockNode {
public:
    bool is_const_;
    StatementsNode *statements_;

    BlockExpressionNode(Position pos, bool is_const, StatementsNode *statements): ExpressionWithBlockNode(pos) {
        is_const_ = is_const;
        statements_ = statements;
    }

    ~BlockExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class LoopExpressionNode : public ExpressionWithBlockNode {
public:
    explicit LoopExpressionNode(Position pos): ExpressionWithBlockNode(pos) {
    };

    ~LoopExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class InfiniteLoopExpressionNode : public LoopExpressionNode {
public:
    BlockExpressionNode *block_expression_ = nullptr;

    InfiniteLoopExpressionNode(Position pos, BlockExpressionNode *block_expression): LoopExpressionNode(pos) {
        block_expression_ = block_expression;
    }

    ~InfiniteLoopExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class PredicateLoopExpressionNode : public LoopExpressionNode {
public:
    ConditionsNode *conditions_ = nullptr;
    BlockExpressionNode *block_expression_ = nullptr;

    PredicateLoopExpressionNode(Position pos, ConditionsNode *conditions,
                                BlockExpressionNode *block_expression): LoopExpressionNode(pos) {
        conditions_ = conditions;
        block_expression_ = block_expression;
    }

    ~PredicateLoopExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class IfExpressionNode : public ExpressionWithBlockNode {
public:
    ConditionsNode *conditions_ = nullptr;
    BlockExpressionNode *true_block_expression_ = nullptr;
    BlockExpressionNode *false_block_expression_ = nullptr;
    IfExpressionNode *if_expression_ = nullptr;

    IfExpressionNode(Position pos, ConditionsNode *conditions,
                     BlockExpressionNode *true_block_expression, BlockExpressionNode *false_block_expression,
                     IfExpressionNode *if_expression): ExpressionWithBlockNode(pos) {
        conditions_ = conditions;
        true_block_expression_ = true_block_expression;
        false_block_expression_ = false_block_expression;
        if_expression_ = if_expression;
    }

    ~IfExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class MatchExpressionNode : public ExpressionWithBlockNode {
public:
    ExpressionNode *expression_ = nullptr; // Except StructExpression
    MatchArmsNode *match_arms_ = nullptr;

    MatchExpressionNode(Position pos, ExpressionNode *expression,
                        MatchArmsNode *match_arms_node): ExpressionWithBlockNode(pos) {
        expression_ = expression;
        match_arms_ = match_arms_node;
    }

    ~MatchExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

/****************  Expression Without Block  ****************/
class ExpressionWithoutBlockNode : public ExpressionNode {
public:
    ExpressionWithoutBlockNode(Position pos, bool is_assignable): ExpressionNode(pos, is_assignable) {
    }

    ~ExpressionWithoutBlockNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ContinueExpressionNode : public ExpressionWithoutBlockNode {
public:
    explicit ContinueExpressionNode(Position pos): ExpressionWithoutBlockNode(pos, false) {
    }

    ~ContinueExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class TupleExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::vector<ExpressionNode *> expressions_;

    TupleExpressionNode(Position pos, const std::vector<ExpressionNode *> &expressions): ExpressionWithoutBlockNode(
        pos, false) {
        expressions_ = expressions;
    }

    ~TupleExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class UnderscoreExpressionNode : public ExpressionWithoutBlockNode {
public:
    explicit UnderscoreExpressionNode(Position pos): ExpressionWithoutBlockNode(pos, true) {
    }

    ~UnderscoreExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class JumpExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    ExpressionNode *expression_;

    JumpExpressionNode(Position pos, TokenType type,
                       ExpressionNode *assignment_expression): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        expression_ = assignment_expression;
    }

    ~JumpExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class AssignmentExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    ExpressionNode *lhs_ = nullptr;
    ExpressionNode *rhs_ = nullptr;

    AssignmentExpressionNode(Position pos, TokenType type,
                             ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~AssignmentExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class LogicOrExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *lhs_ = nullptr;
    ExpressionNode *rhs_ = nullptr;

    LogicOrExpressionNode(Position pos, ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(
        pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~LogicOrExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class LogicAndExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *lhs_ = nullptr;
    ExpressionNode *rhs_ = nullptr;

    LogicAndExpressionNode(Position pos, ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(
        pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~LogicAndExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ComparisonExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    ExpressionNode *lhs_;
    ExpressionNode *rhs_;

    ComparisonExpressionNode(Position pos, TokenType type, ExpressionNode *lhs,
                             ExpressionNode *rhs): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~ComparisonExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class BitwiseOrExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *lhs_;
    ExpressionNode *rhs_;

    BitwiseOrExpressionNode(Position pos, ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(
        pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~BitwiseOrExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class BitwiseXorExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *lhs_;
    ExpressionNode *rhs_;

    BitwiseXorExpressionNode(Position pos, ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(
        pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~BitwiseXorExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class BitwiseAndExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *lhs_;
    ExpressionNode *rhs_;

    BitwiseAndExpressionNode(Position pos, ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(
        pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~BitwiseAndExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ShiftExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    ExpressionNode *lhs_;
    ExpressionNode *rhs_;

    ShiftExpressionNode(Position pos, TokenType type,
                        ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~ShiftExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class AddMinusExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    ExpressionNode *lhs_;
    ExpressionNode *rhs_;

    AddMinusExpressionNode(Position pos, TokenType type,
                           ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~AddMinusExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class MulDivModExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    ExpressionNode *lhs_;
    ExpressionNode *rhs_;

    MulDivModExpressionNode(Position pos, TokenType type,
                            ExpressionNode *lhs, ExpressionNode *rhs): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~MulDivModExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class TypeCastExpressionNode : public ExpressionWithoutBlockNode {
public:
    TypeNode *type_;
    ExpressionNode *expression_;

    TypeCastExpressionNode(Position pos, TypeNode *type, ExpressionNode *expression): ExpressionWithoutBlockNode(
        pos, false) {
        type_ = type;
        expression_ = expression;
    }

    ~TypeCastExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};


class UnaryExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    ExpressionNode *expression_;

    UnaryExpressionNode(Position pos, TokenType type,
                        ExpressionNode *expression): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        expression_ = expression;
    }

    ~UnaryExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class FunctionCallExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *callee_;
    std::vector<ExpressionNode *> params_;

    FunctionCallExpressionNode(Position pos, ExpressionNode *callee,
                               const std::vector<ExpressionNode *> &params): ExpressionWithoutBlockNode(pos, false) {
        callee_ = callee;
        params_ = params;
    }

    ~FunctionCallExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ArrayIndexExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *base_;
    ExpressionNode *index_;

    ArrayIndexExpressionNode(Position pos, ExpressionNode *base,
                             ExpressionNode *index): ExpressionWithoutBlockNode(pos, true) {
        base_ = base;
        index_ = index;
    }

    ~ArrayIndexExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class MemberAccessExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *base_;
    Token member_;

    MemberAccessExpressionNode(Position pos, ExpressionNode *base, const Token &member): ExpressionWithoutBlockNode(
        pos, false) {
        base_ = base;
        member_ = member;
    }

    ~MemberAccessExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class GroupedExpressionNode : public ExpressionWithoutBlockNode {
public:
    ExpressionNode *expression_;

    GroupedExpressionNode(Position pos, ExpressionNode *expression): ExpressionWithoutBlockNode(pos, false) {
        expression_ = expression;
    }

    ~GroupedExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StructExpressionNode : public ExpressionWithoutBlockNode {
public:
    PathInExpressionNode *path_in_expression_node_ = nullptr;
    StructExprFieldsNode *struct_expr_fields_node_ = nullptr;
    StructBaseNode *struct_base_node_ = nullptr;

    StructExpressionNode(Position pos, PathInExpressionNode *path_in_expression_node,
                         StructExprFieldsNode *struct_expr_fields_node,
                         StructBaseNode *struct_base_node): ExpressionWithoutBlockNode(pos, false) {
        path_in_expression_node_ = path_in_expression_node;
        struct_expr_fields_node_ = struct_expr_fields_node;
        struct_base_node_ = struct_base_node;
    }

    ~StructExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StructExprFieldsNode : public ASTNode {
public:
    std::vector<StructExprFieldNode *> struct_expr_field_nodes_;
    StructBaseNode *struct_base_node_;

    StructExprFieldsNode(Position pos, const std::vector<StructExprFieldNode *> &struct_expr_field_nodes,
                         StructBaseNode *struct_base): ASTNode(pos) {
        struct_expr_field_nodes_ = struct_expr_field_nodes;
        struct_base_node_ = struct_base;
    }

    ~StructExprFieldsNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StructExprFieldNode : public ASTNode {
public:
    std::string identifier_;
    ExpressionNode *expression_node_;

    StructExprFieldNode(Position pos, const std::string &identifier,
                        ExpressionNode *expression_node): ASTNode(pos) {
        identifier_ = identifier;
        expression_node_ = expression_node;
    }

    ~StructExprFieldNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StructBaseNode : public ASTNode {
public:
    ExpressionNode *expression_node_;

    StructBaseNode(Position pos, ExpressionNode *expression_node): ASTNode(pos) {
        expression_node_ = expression_node;
    }

    ~StructBaseNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class PathExpressionNode : public ExpressionWithoutBlockNode {
public:
    explicit PathExpressionNode(Position pos): ExpressionWithoutBlockNode(pos, true) {
    }

    ~PathExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class PathInExpressionNode : public PathExpressionNode {
public:
    std::vector<PathIndentSegmentNode *> path_indent_segments_;

    explicit PathInExpressionNode(Position pos,
                                  const std::vector<PathIndentSegmentNode *> &simple_path_segments): PathExpressionNode(
        pos) {
        path_indent_segments_ = simple_path_segments;
    }

    ~PathInExpressionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

/****************  Literal Node  ****************/
class LiteralExpressionNode : public ExpressionWithoutBlockNode {
public:
    explicit LiteralExpressionNode(Position pos): ExpressionWithoutBlockNode(pos, false) {
    }

    ~LiteralExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class CharLiteralNode : public LiteralExpressionNode {
public:
    char char_literal_;

    CharLiteralNode(Position pos, const char &char_literal): LiteralExpressionNode(pos) {
        char_literal_ = char_literal;
    }

    ~CharLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StringLiteralNode : public LiteralExpressionNode {
public:
    std::string string_literal_;

    StringLiteralNode(Position pos, const std::string &string_literal): LiteralExpressionNode(pos) {
        string_literal_ = string_literal;
    }

    ~StringLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class IntLiteralNode : public LiteralExpressionNode {
public:
    int64_t int_literal_;
    bool is_u32_ = false;
    bool is_i32_ = false;
    bool is_isize_ = false;
    bool is_usize_ = false;

    IntLiteralNode(Position pos, const int64_t &int_literal,
        bool is_u32, bool is_i32, bool is_usize, bool is_isize): LiteralExpressionNode(pos) {
        int_literal_ = int_literal;
        is_u32_ = is_u32;
        is_i32_ = is_i32;
        is_isize_ = is_isize;
        is_usize_ = is_usize;
    }

    ~IntLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class BoolLiteralNode : public LiteralExpressionNode {
public:
    bool bool_literal_;

    BoolLiteralNode(Position pos, const bool &bool_literal): LiteralExpressionNode(pos) {
        bool_literal_ = bool_literal;
    }

    ~BoolLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class CStringLiteralNode : public LiteralExpressionNode {
public:
    std::string c_string_literal_;

    CStringLiteralNode(Position pos, const std::string &c_string_literal): LiteralExpressionNode(pos) {
        c_string_literal_ = c_string_literal;
    }

    ~CStringLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ArrayLiteralNode : public LiteralExpressionNode {
public:
    std::vector<ExpressionNode *> expressions_;
    ExpressionNode *lhs_;
    ExpressionNode *rhs_;

    ArrayLiteralNode(Position pos, const std::vector<ExpressionNode *> &expression_nodes,
                     ExpressionNode *lhs, ExpressionNode *rhs): LiteralExpressionNode(pos) {
        expressions_ = expression_nodes;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~ArrayLiteralNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

/****************  Support Node for Expression  ****************/
class ConditionsNode : public ASTNode {
public:
    ExpressionNode *expression_ = nullptr;

    LetChainNode *let_chain_node_ = nullptr;

    ConditionsNode(Position pos, ExpressionNode *expression, LetChainNode *let_chain_node): ASTNode(pos) {
        expression_ = expression;
        let_chain_node_ = let_chain_node;
    }

    ~ConditionsNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class LetChainConditionNode;

class LetChainNode : public ASTNode {
public:
    std::vector<LetChainConditionNode *> let_chain_condition_nodes_;

    LetChainNode(Position pos, const std::vector<LetChainConditionNode *> &let_chain_condition_nodes): ASTNode(pos) {
        let_chain_condition_nodes_ = let_chain_condition_nodes;
    }

    ~LetChainNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class LetChainConditionNode : public ASTNode {
public:
    PatternNode *pattern_node_;
    ExpressionNode *expression_node_;

    LetChainConditionNode(Position pos, PatternNode *pattern_node, ExpressionNode *expression_node): ASTNode(pos) {
        pattern_node_ = pattern_node;
        expression_node_ = expression_node;
    }

    ~LetChainConditionNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StatementsNode : public ASTNode {
public:
    std::vector<StatementNode *> statements_;
    ExpressionNode *expression_ = nullptr;

    StatementsNode(Position pos, const std::vector<StatementNode *> &statements,
                   ExpressionNode *expression): ASTNode(pos) {
        statements_ = statements;
        expression_ = expression;
    }

    ~StatementsNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class MatchArmsNode : public ASTNode {
public:
    std::vector<MatchArmNode *> match_arm_nodes_;
    std::vector<ExpressionNode *> expression_nodes_;

    MatchArmsNode(Position pos, const std::vector<MatchArmNode *> &match_arm_nodes,
                  const std::vector<ExpressionNode *> &expression_nodes): ASTNode(pos) {
        match_arm_nodes_ = match_arm_nodes;
        expression_nodes_ = expression_nodes;
    }

    ~MatchArmsNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class MatchArmNode : public ASTNode {
public:
    PatternNode *pattern_node_;
    ExpressionNode *match_arm_guard_;

    MatchArmNode(Position pos, PatternNode *pattern_node,
                 ExpressionNode *match_arm_guard): ASTNode(pos) {
        pattern_node_ = pattern_node;
        match_arm_guard_ = match_arm_guard;
    }

    ~MatchArmNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};


/****************  Statement  ****************/
class StatementNode : public ASTNode {
public:
    explicit StatementNode(Position pos): ASTNode(pos) {
    }

    ~StatementNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class EmptyStatementNode : public StatementNode {
public:
    explicit EmptyStatementNode(Position pos): StatementNode(pos) {
    }

    ~EmptyStatementNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class LetStatementNode : public StatementNode {
public:
    PatternNoTopAltNode *pattern_no_top_alt_ = nullptr;
    TypeNode *type_ = nullptr;
    ExpressionNode *expression_ = nullptr;
    BlockExpressionNode *block_expression_ = nullptr;

    LetStatementNode(Position pos, PatternNoTopAltNode *pattern_no_top_alt, TypeNode *type,
                     ExpressionNode *expression, BlockExpressionNode *block_expression): StatementNode(pos) {
        pattern_no_top_alt_ = pattern_no_top_alt;
        type_ = type;
        expression_ = expression;
        block_expression_ = block_expression;
    }

    ~LetStatementNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ExpressionStatementNode : public StatementNode {
public:
    ExpressionNode *expression_ = nullptr; // Except StructExpression

    ExpressionStatementNode(Position pos, ExpressionNode *expression): StatementNode(pos) {
        expression_ = expression;
    }

    ~ExpressionStatementNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class VisItemStatementNode : public StatementNode {
public:
    VisItemNode *vis_item_node_ = nullptr;

    VisItemStatementNode(Position pos, VisItemNode *vis_item_node): StatementNode(pos) {
        vis_item_node_ = vis_item_node;
    }

    ~VisItemStatementNode() override;
};

/****************  Patterns  ****************/
class PatternNode : public ASTNode {
public:
    std::vector<PatternNoTopAltNode *> pattern_no_top_alts_;

    PatternNode(Position pos, const std::vector<PatternNoTopAltNode *> &pattern_no_top_alts): ASTNode(pos) {
        pattern_no_top_alts_ = pattern_no_top_alts;
    }

    ~PatternNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class PatternNoTopAltNode : public ASTNode {
public:
    explicit PatternNoTopAltNode(Position pos): ASTNode(pos) {
    }

    ~PatternNoTopAltNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class PatternWithoutRangeNode : public PatternNoTopAltNode {
public:
    explicit PatternWithoutRangeNode(Position pos): PatternNoTopAltNode(pos) {
    }

    ~PatternWithoutRangeNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class LiteralPatternNode : public PatternWithoutRangeNode {
public:
    bool have_minus_ = false;
    ExpressionNode *expression_;

    LiteralPatternNode(Position pos, bool have_minus, ExpressionNode *expression): PatternWithoutRangeNode(pos) {
        have_minus_ = have_minus;
        expression_ = expression;
    }

    ~LiteralPatternNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class IdentifierPatternNode : public PatternWithoutRangeNode {
public:
    bool is_ref_ = false;
    bool is_mut_ = false;
    std::string identifier_;
    PatternNoTopAltNode *node_ = nullptr;

    IdentifierPatternNode(Position pos, bool is_ref, bool is_mut, const std::string &identifier,
                          PatternNoTopAltNode *node): PatternWithoutRangeNode(pos) {
        is_ref_ = is_ref;
        is_mut_ = is_mut;
        identifier_ = identifier;
        node_ = node;
    }

    ~IdentifierPatternNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class WildcardPatternNode : public PatternWithoutRangeNode {
public:
    explicit WildcardPatternNode(Position pos): PatternWithoutRangeNode(pos) {
    }

    ~WildcardPatternNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class RestPatternNode : public PatternWithoutRangeNode {
public:
    explicit RestPatternNode(Position pos): PatternWithoutRangeNode(pos) {
    }

    ~RestPatternNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class GroupedPatternNode : public PatternWithoutRangeNode {
public:
    PatternNode *pattern_ = nullptr;

    GroupedPatternNode(Position pos, PatternNode *pattern): PatternWithoutRangeNode(pos) {
        pattern_ = pattern;
    }

    ~GroupedPatternNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class SlicePatternNode : public PatternWithoutRangeNode {
public:
    std::vector<PatternNode *> patterns_;

    SlicePatternNode(Position pos, const std::vector<PatternNode *> &patterns): PatternWithoutRangeNode(pos) {
        patterns_ = patterns;
    }

    ~SlicePatternNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class PathPatternNode : public PatternWithoutRangeNode {
public:
    ExpressionNode *expression_;

    PathPatternNode(Position pos, ExpressionNode *expression): PatternWithoutRangeNode(pos) {
        expression_ = expression;
    }

    ~PathPatternNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

/****************  Types  ****************/
class TypeNode : public ASTNode {
public:
    std::shared_ptr<Type> type;

    explicit TypeNode(Position pos): ASTNode(pos) {
    }

    ~TypeNode() override = default;

    virtual std::string toString() = 0;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class TypeNoBoundsNode : public TypeNode {
public:
    explicit TypeNoBoundsNode(Position pos): TypeNode(pos) {
    }

    ~TypeNoBoundsNode() override = default;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ParenthesizedTypeNode : public TypeNoBoundsNode {
public:
    TypeNode *type_ = nullptr;

    ParenthesizedTypeNode(Position pos, TypeNode *type): TypeNoBoundsNode(pos) {
        type_ = type;
    }

    ~ParenthesizedTypeNode() override;

    std::string toString() override {
        return "( " + type_->toString() + " )";
    }

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};


class TypePathSegmentNode : public ASTNode {
public:
    PathIndentSegmentNode *path_indent_segment_node_ = nullptr;

    TypePathSegmentNode(Position pos, PathIndentSegmentNode *path_indent_segment_node): ASTNode(pos) {
        path_indent_segment_node_ = path_indent_segment_node;
    }

    ~TypePathSegmentNode() override;

    [[nodiscard]] std::string toString() const {
        return path_indent_segment_node_->toString();
    }

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};


class TypePathNode : public TypeNoBoundsNode {
public:
    TypePathSegmentNode * type_path_segment_node_;

    TypePathNode(Position pos, TypePathSegmentNode * type_path_segment_node):
    TypeNoBoundsNode(pos) {
        type_path_segment_node_ = type_path_segment_node;
    }

    ~TypePathNode() override;

    std::string toString() override {
        return type_path_segment_node_->toString();
    }

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class TupleTypeNode : public TypeNoBoundsNode {
public:
    std::vector<TypeNode *> type_nodes_;

    TupleTypeNode(Position pos, const std::vector<TypeNode *> &type_nodes): TypeNoBoundsNode(pos) {
        type_nodes_ = type_nodes;
    }

    ~TupleTypeNode() override;

    [[nodiscard]] std::string toString() override {
        std::string str = "( ";
        for (auto &it: type_nodes_) {
            str += it->toString();
            str += ",";
        }
        str += " )";
        return str;
    }

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ArrayTypeNode : public TypeNoBoundsNode {
public:
    TypeNode *type_;
    ExpressionNode *expression_node_ = nullptr;

    ArrayTypeNode(Position pos, TypeNode *type, ExpressionNode *expression_node): TypeNoBoundsNode(pos) {
        type_ = type;
        expression_node_ = expression_node;
    }

    ~ArrayTypeNode() override;

    [[nodiscard]] std::string toString() override {
        std::string str = "[";
        str += type_->toString();
        str += ";";

        auto *tmp = dynamic_cast<IntLiteralNode *>(expression_node_);
        // TODO Now just let it just be an IntLiteral.
        if (!tmp) {
            throw SemanticError(
                "Semantic Error: The size of an array must be a constant in compilation", pos_);
        }
        str += std::to_string((tmp->int_literal_));
        str += "]";
        return str;
    }

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class SliceTypeNode : public TypeNoBoundsNode {
public:
    TypeNode *type_ = nullptr;

    SliceTypeNode(Position pos, TypeNode *type): TypeNoBoundsNode(pos) {
        type_ = type;
    }

    ~SliceTypeNode() override;

    [[nodiscard]] std::string toString() override {
        return "[" + type_->toString() + "]";
    }

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class ReferenceTypeNode: public TypeNoBoundsNode {
public:
    bool is_mut_;
    TypeNode* type_node_;

    ReferenceTypeNode(Position pos, bool is_mut, TypeNode* type_node):
        TypeNoBoundsNode(pos) {
        is_mut_ = is_mut;
        type_node_ = type_node;
    }

    ~ReferenceTypeNode() override;

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};

class InferredTypeNode : public TypeNoBoundsNode {
public:
    explicit InferredTypeNode(Position pos): TypeNoBoundsNode(pos) {
    }

    ~InferredTypeNode() override = default;

    [[nodiscard]] std::string toString() override {
        return "_";
    }

    void accept(ASTVisitor *visitor) override {
        visitor->visit(this);
    }
};
#endif //ASTNODE_H
