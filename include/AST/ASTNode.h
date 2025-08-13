#ifndef ASTNODE_H
#define ASTNODE_H
#include <vector>
#include <string>
#include "../../util/Position.h"
#include "../Lexer/TokenType.h"
#include "../Lexer/Token.h"

class ASTNode;
class CrateNode;
class VisItemNode; // In our R Compiler, Item just means VisItem
class ModuleNode;
class FunctionNode;
class StructNode;
class EnumerationNode;
class ConstantItemNode;
class TraitNode;
class ImplementationNode;
class IdentifierNode;
class GenericParamNode; // Use std::vector to perform GenericParamNode
class FunctionParametersNode;
class WhereClauseItemNode;
/* Use std::vector to perform WhereClauseNode, and since we do not have LifeTimeClauseItem,
 * it just represents TypeBoundClauseItem. */
class FunctionParamNode;
class TypeNode; // Also FunctionReturnTypeNode
class FunctionParamPatternNode;
class StructNode;
class StructFieldNode; // Use std::vector to perform StructFields
class EnumVariantNode; // Use std::vector to perform EnumVariants
class EnumVariantStructNode;
class EnumVariantDiscriminantNode;
class TypeParamBoundsNode;
class AssociatedItemNode;
class InherentImplNode;
class TraitImplNode;
class TypePathNode;
class TypeParamNode;
class ConstParamNode;
class StatementNode;
class EmptyStatementNode;
class LetStatementNode;
class ExpressionStatementNode;
class ExpressionNode; // Also StructBaseNode
class ExpressionWithoutBlockNode;
class ExpressionWithBlockNode;
class LiteralExpressionNode;
class CharLiteralNode;
class StringLiteralNode;
class CStringLiteralNode;
class IntLiteralNode;
class BoolLiteralNode;
class PathExpressionNode;
class OperatorExpressionNode;
class GroupedExpressionNode;
class TupleExpressionNode;
class TupleIndexingExpressionNode;
class StructExpressionNode;
class FieldExpressionNode;
class ContinueExpressionNode;
class UnderscoreExpressionNode;
class BlockExpressionNode; // use is_const_ to show whether this block expression is constant
class LoopExpressionNode;
class IfExpressionNode;
class MatchExpressionNode;
class MatchArmsNode;
class PathInExpressionNode;
class QualifiedPathInExpressionNode;
class StatementsNode;
class ComparisonExpressionNode;
class TypeCastExpressionNode;
class AssignmentExpressionNode;
class StructExprFieldNode;
class ConditionsNode;
class LetChainNode;

class PatternNode;
class PatternNoTopAltNode;

class ASTNode {
protected:
    Position pos_;
public:
    explicit ASTNode(const Position pos) {
        pos_ = pos;
    }

    virtual ~ASTNode() = default;

    // virtual void accept(Visitor visitor)
};

/****************  Items  ****************/
class CrateNode: public ASTNode {
    std::vector<VisItemNode*> items_;
public:
    CrateNode(Position pos, const std::vector<VisItemNode*>& items): ASTNode(pos) {
        items_ = items;
    }

    ~CrateNode() override;
};

class VisItemNode: public ASTNode {
    ASTNode* node_;
public:
    VisItemNode(Position pos, ASTNode* node): ASTNode(pos) {
        node_ = node;
    }

    ~VisItemNode() override;
};

class FunctionNode: public ASTNode {
    bool is_const_;
    std::string identifier_;
    FunctionParametersNode* function_parameters_ = nullptr;
    TypeNode* type_ = nullptr;
    std::vector<WhereClauseItemNode*> where_clause_items_;
    BlockExpressionNode* block_expression_ = nullptr;
public:
    FunctionNode(Position pos, bool is_const, const std::string& identifier,
        FunctionParametersNode* function_parameters, TypeNode* type,
        const std::vector<WhereClauseItemNode*>& where_clause_items,
        BlockExpressionNode* block_expression): ASTNode(pos) {
        is_const_ = is_const;
        identifier_ = identifier;
        function_parameters_ = function_parameters;
        type_ = type;
        where_clause_items_ = where_clause_items;
        block_expression_ = block_expression;
    }

    ~FunctionNode() override;
};

class FunctionParametersNode: public ASTNode {
    std::vector<FunctionParamNode*> function_params_;
public:
    FunctionParametersNode(Position pos, const std::vector<FunctionParamNode*>& function_params):
        ASTNode(pos) {
        function_params_ = function_params;
    }

    ~FunctionParametersNode() override;
};

class FunctionParamNode: public ASTNode {
    FunctionParamPatternNode* function_param_pattern_ = nullptr;
    TypeNode* type_ = nullptr;
    bool is_DotDotDot_ = false;
public:
    FunctionParamNode(Position pos, FunctionParamPatternNode* function_param_pattern,
        TypeNode* type, bool is_DotDotDot): ASTNode(pos) {
        function_param_pattern_ = function_param_pattern;
        type_ = type;
        is_DotDotDot_ =  is_DotDotDot;
    }

    ~FunctionParamNode() override;
};

class FunctionParamPatternNode: public ASTNode {
    PatternNoTopAltNode* pattern_no_top_alt_;
    TypeNode* type_;
    bool is_DotDotDot_ = false;
public:
    FunctionParamPatternNode(Position pos, PatternNoTopAltNode* pattern_no_top_alt,
        TypeNode* type, bool is_DotDotDot): ASTNode(pos) {
        pattern_no_top_alt_ = pattern_no_top_alt;
        type_ = type;
        is_DotDotDot_ = is_DotDotDot;
    }

    ~FunctionParamPatternNode() override;
};

class StructNode: public ASTNode {
    std::string identifier_;
    std::vector<StructFieldNode*> struct_field_nodes_;
public:
    StructNode(Position pos, const std::string& identifier,
        const std::vector<StructFieldNode*>& struct_field_nodes): ASTNode(pos) {
        identifier_ = identifier;
        struct_field_nodes_ = struct_field_nodes;
    }

    ~StructNode() override;
};

class StructFieldNode: public ASTNode {
    std::string identifier_;
    TypeNode* type_node_;
public:
    StructFieldNode(Position pos, const std::string& identifier, TypeNode* type_node):
        ASTNode(pos) {
        identifier_ = identifier;
        type_node_ = type_node;
    }

    ~StructFieldNode() override;
};

class EnumerationNode: public ASTNode {
    std::string identifier_;
    std::vector<EnumVariantNode*> enum_variant_nodes_;
public:
    EnumerationNode(Position pos, const std::string& identifier,
        const std::vector<EnumVariantNode*>& enum_variant_nodes): ASTNode(pos) {
        identifier_ = identifier;
        enum_variant_nodes_ = enum_variant_nodes;
    }

    ~EnumerationNode() override;
};

class EnumVariantNode: public ASTNode {
    std::string identifier_;
    EnumVariantStructNode* enum_variant_struct_node_ = nullptr;
    EnumVariantDiscriminantNode* enum_variant_discriminant_node_ = nullptr;
public:
    EnumVariantNode(Position pos, const std::string& identifier,
        EnumVariantStructNode* enum_variant_struct_node,
        EnumVariantDiscriminantNode* enum_variant_discriminant_node): ASTNode(pos) {
        identifier_ = identifier;
        enum_variant_struct_node_ = enum_variant_struct_node;
        enum_variant_discriminant_node_ = enum_variant_discriminant_node;
    }

    ~EnumVariantNode() override;
};

class EnumVariantStructNode: public ASTNode {
    std::vector<StructFieldNode*> struct_field_nodes_;
public:
    EnumVariantStructNode(Position pos, const std::vector<StructFieldNode*>& struct_field_nodes):
        ASTNode(pos) {
        struct_field_nodes_ = struct_field_nodes;
    }

    ~EnumVariantStructNode() override;
};

class EnumVariantDiscriminantNode: public ASTNode {
    ExpressionNode* expression_node_;
public:
    EnumVariantDiscriminantNode(Position pos, ExpressionNode* expression_node): ASTNode(pos) {
        expression_node_ = expression_node;
    }

    ~EnumVariantDiscriminantNode() override;
};

class ConstantItemNode: public ASTNode {
    std::string identifier_;
    bool is_underscore_ = false;
    TypeNode* type_node_;
    ExpressionNode* expression_node_;
public:
    ConstantItemNode(Position pos, const std::string& identifier, bool is_underscore,
        TypeNode* type_node, ExpressionNode* expression_node): ASTNode(pos) {
        identifier_ = identifier;
        is_underscore_ = is_underscore;
        type_node_ = type_node;
        expression_node_ = expression_node;
    }

    ~ConstantItemNode() override; // TODO
};

class AssociatedItemNode: public ASTNode {
    ConstantItemNode* constant_item_node_;
    FunctionNode* function_node_;
public:
    AssociatedItemNode(Position pos, ConstantItemNode* constant_item_node,
        FunctionNode* function_node): ASTNode(pos) {
        constant_item_node_ = constant_item_node;
        function_node_ = function_node;
    }

    ~AssociatedItemNode() override;
};

/****************  Expression With Block  ****************/
class ExpressionNode: public ASTNode {
protected:
    bool is_assignable_ = false;
public:
    ExpressionNode(Position pos, bool is_assignable): ASTNode(pos) {
        is_assignable_ = is_assignable;
    }

    ~ExpressionNode() override = default;
};

class ExpressionWithBlockNode: public ExpressionNode {
public:
    explicit ExpressionWithBlockNode(Position pos): ExpressionNode(pos, false) {}

    ~ExpressionWithBlockNode() override = default;
};

class BlockExpressionNode: public ExpressionWithBlockNode {
    bool is_const_;
    StatementsNode* statements_;
public:
    BlockExpressionNode(Position pos, bool is_const, StatementsNode* statements):
        ExpressionWithBlockNode(pos) {
        is_const_ = is_const;
        statements_ = statements;
    }

    ~BlockExpressionNode() override;
};


class LoopExpressionNode: public ExpressionWithBlockNode {
public:
    explicit LoopExpressionNode(Position pos): ExpressionWithBlockNode(pos) {};

    ~LoopExpressionNode() override = default;
};

class InfiniteLoopExpressionNode: public LoopExpressionNode {
    BlockExpressionNode* block_expression_ = nullptr;
public:
    InfiniteLoopExpressionNode(Position pos, BlockExpressionNode* block_expression):
        LoopExpressionNode(pos) {
        block_expression_ = block_expression;
    }

    ~InfiniteLoopExpressionNode() override;
};

class PredicateLoopExpressionNode: public LoopExpressionNode {
    ConditionsNode* conditions_ = nullptr;
    BlockExpressionNode* block_expression_ = nullptr;
public:
    PredicateLoopExpressionNode(Position pos, ConditionsNode* conditions,
        BlockExpressionNode* block_expression): LoopExpressionNode(pos) {
        conditions_ = conditions;
        block_expression_ = block_expression;
    }

    ~PredicateLoopExpressionNode() override;
};

class IfExpressionNode: public ExpressionWithBlockNode {
    ConditionsNode* conditions_ = nullptr;
    BlockExpressionNode* true_block_expression_ = nullptr;
    BlockExpressionNode* false_block_expression_ = nullptr;
    IfExpressionNode* if_expression_ = nullptr;
public:
    IfExpressionNode(Position pos, ConditionsNode* conditions,
        BlockExpressionNode* true_block_expression, BlockExpressionNode* false_block_expression,
        IfExpressionNode* if_expression): ExpressionWithBlockNode(pos) {
        conditions_ = conditions;
        true_block_expression_ = true_block_expression;
        false_block_expression_ = false_block_expression;
        if_expression_ = if_expression;
    }

    ~IfExpressionNode() override;
};

class MatchExpressionNode: public ExpressionWithBlockNode {
    ExpressionNode* expression_ = nullptr; // Except StructExpression
    MatchArmsNode* match_arms_ = nullptr;
public:
    MatchExpressionNode(Position pos, ExpressionNode* expression, MatchArmsNode* match_arms_node):
        ExpressionWithBlockNode(pos) {
        expression_ = expression;
        match_arms_ = match_arms_node;
    }

    ~MatchExpressionNode() override;
};

/****************  Expression Without Block  ****************/
class ExpressionWithoutBlockNode: public ExpressionNode {
public:
    ExpressionWithoutBlockNode(Position pos, bool is_assignable):
        ExpressionNode(pos, is_assignable) {}

    ~ExpressionWithoutBlockNode() override = default;
};

class ContinueExpressionNode: public ExpressionWithoutBlockNode {
public:
    explicit ContinueExpressionNode(Position pos):
        ExpressionWithoutBlockNode(pos, false) {}

    ~ContinueExpressionNode() override = default;
};

class TupleExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<ExpressionNode*> expressions_;
public:
    TupleExpressionNode(Position pos, const std::vector<ExpressionNode*>& expressions):
        ExpressionWithoutBlockNode(pos, false) {
        expressions_ = expressions;
    }

    ~TupleExpressionNode() override;
};



class UnderscoreExpressionNode: public ExpressionWithoutBlockNode {
public:
    explicit UnderscoreExpressionNode(Position pos): ExpressionWithoutBlockNode(pos, true) {}

    ~UnderscoreExpressionNode() override = default;
};

class JumpExpressionNode: public ExpressionWithoutBlockNode {
    TokenType type_;
    ExpressionNode* expression_;
public:
    JumpExpressionNode(Position pos, TokenType type,
        ExpressionNode* assignment_expression):
        ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        expression_ = assignment_expression;
    }

    ~JumpExpressionNode() override;
};

class LogicOrExpressionNode;
class AssignmentExpressionNode: public ExpressionWithoutBlockNode {
    TokenType type_;
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
public:
    AssignmentExpressionNode(Position pos, TokenType type,
        ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~AssignmentExpressionNode() override;
};

class LogicAndExpressionNode;
class LogicOrExpressionNode: public ExpressionWithoutBlockNode {
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
public:
    LogicOrExpressionNode(Position pos, ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~LogicOrExpressionNode() override;
};

class LogicAndExpressionNode: public ExpressionWithoutBlockNode {
    ExpressionNode* lhs_ = nullptr;
    ExpressionNode* rhs_ = nullptr;
public:
    LogicAndExpressionNode(Position pos, ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~LogicAndExpressionNode() override;
};

class BitwiseOrExpressionNode;
class ComparisonExpressionNode: public ExpressionWithoutBlockNode {
    TokenType type_;
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    ComparisonExpressionNode(Position pos, TokenType type, ExpressionNode* lhs,
        ExpressionNode* rhs): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~ComparisonExpressionNode() override;
};

class BitwiseXorExpressionNode;
class BitwiseOrExpressionNode: public ExpressionWithoutBlockNode {
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    BitwiseOrExpressionNode(Position pos, ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~BitwiseOrExpressionNode() override;
};

class BitwiseAndExpressionNode;
class BitwiseXorExpressionNode: public ExpressionWithoutBlockNode {
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    BitwiseXorExpressionNode(Position pos, ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~BitwiseXorExpressionNode() override;
};

class ShiftExpressionNode;
class BitwiseAndExpressionNode: public ExpressionWithoutBlockNode {
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    BitwiseAndExpressionNode(Position pos, ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~BitwiseAndExpressionNode() override;
};

class AddMinusExpressionNode;
class ShiftExpressionNode: public ExpressionWithoutBlockNode {
    TokenType type_;
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    ShiftExpressionNode(Position pos, TokenType type,
        ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~ShiftExpressionNode() override;
};

class MulDivModExpressionNode;
class AddMinusExpressionNode: public ExpressionWithoutBlockNode {
    TokenType type_;
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    AddMinusExpressionNode(Position pos, TokenType type,
        ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~AddMinusExpressionNode() override;
};

class MulDivModExpressionNode: public ExpressionWithoutBlockNode {
    TokenType type_;
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    MulDivModExpressionNode(Position pos, TokenType type,
        ExpressionNode* lhs, ExpressionNode* rhs):
        ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~MulDivModExpressionNode() override;
};

class UnaryExpressionNode;
class TypeCastExpressionNode: public ExpressionWithoutBlockNode {
    TypeNode* type_;
    ExpressionNode* expression_;
public:
    TypeCastExpressionNode(Position pos, TypeNode* type, ExpressionNode* expression):
        ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        expression_ = expression;
    }

    ~TypeCastExpressionNode() override;
};

class UnaryExpressionNode: public ExpressionWithoutBlockNode {
    TokenType type_;
    ExpressionNode* expression_;
public:
    UnaryExpressionNode(Position pos, TokenType type,
        ExpressionNode* expression): ExpressionWithoutBlockNode(pos, false) {
        type_ = type;
        expression_ = expression;
    }

    ~UnaryExpressionNode() override;
};

class FunctionCallExpressionNode : public ExpressionWithoutBlockNode {
    ExpressionNode* callee_;
    std::vector<ExpressionNode*> params_;
public:
    FunctionCallExpressionNode(Position pos, ExpressionNode* callee,
        const std::vector<ExpressionNode*>& params): ExpressionWithoutBlockNode(pos, false) {
        callee_ = callee;
        params_ = params;
    }

    ~FunctionCallExpressionNode() override;
};

class ArrayIndexExpressionNode : public ExpressionWithoutBlockNode {
    ExpressionNode* base_;
    ExpressionNode* index_;
public:
    ArrayIndexExpressionNode(Position pos, ExpressionNode* base,
        ExpressionNode* index): ExpressionWithoutBlockNode(pos, true) {
        base_ = base;
        index_ = index;
    }

    ~ArrayIndexExpressionNode() override;
};

class MemberAccessExpressionNode : public ExpressionWithoutBlockNode {
    ExpressionNode* base_;
    Token member_;
public:
    MemberAccessExpressionNode(Position pos, ExpressionNode* base, const Token& member):
        ExpressionWithoutBlockNode(pos, false) {
        base_ = base;
        member_ = member;
    }

    ~MemberAccessExpressionNode() override;
};

class GroupedExpressionNode: public ExpressionWithoutBlockNode {
    ExpressionNode* expression_;
public:
    GroupedExpressionNode(Position pos, ExpressionNode* expression):
        ExpressionWithoutBlockNode(pos, false) {
        expression_ = expression;
    }

    ~GroupedExpressionNode() override;
};

class StructExprFieldsNode;
class StructBaseNode;

class StructExpressionNode: public ExpressionWithoutBlockNode {
    PathInExpressionNode* path_in_expression_node_ = nullptr;
    StructExprFieldsNode* struct_expr_fields_node_ = nullptr;
    StructBaseNode* struct_base_node_ = nullptr;
public:
    StructExpressionNode(Position pos, PathInExpressionNode* path_in_expression_node,
        StructExprFieldsNode* struct_expr_fields_node, StructBaseNode* struct_base_node):
        ExpressionWithoutBlockNode(pos, false) {
        path_in_expression_node_ = path_in_expression_node;
        struct_expr_fields_node_ = struct_expr_fields_node;
        struct_base_node_ = struct_base_node;
    }

    ~StructExpressionNode() override;
};

class StructExprFieldsNode: public ASTNode {
    std::vector<StructExprFieldNode*> struct_expr_field_nodes_;
    StructBaseNode* struct_base_node_;
public:
    StructExprFieldsNode(Position pos, const std::vector<StructExprFieldNode*>& struct_expr_field_nodes,
        StructBaseNode* struct_base): ASTNode(pos) {
        struct_expr_field_nodes_ = struct_expr_field_nodes;
        struct_base_node_ = struct_base;
    }

    ~StructExprFieldsNode() override;
};

class StructExprFieldNode: public ASTNode {
    std::string identifier_;
    ExpressionNode* expression_node_;
public:
    StructExprFieldNode(Position pos, const std::string& identifier,
        ExpressionNode* expression_node): ASTNode(pos) {
        identifier_ = identifier;
        expression_node_ = expression_node;
    }

    ~StructExprFieldNode() override;
};

class StructBaseNode: public ASTNode {
    ExpressionNode* expression_node_;
public:
    StructBaseNode(Position pos, ExpressionNode* expression_node): ASTNode(pos) {
        expression_node_ = expression_node;
    }

    ~StructBaseNode() override;
};

class PathExpressionNode: public ExpressionWithoutBlockNode {
public:
    explicit PathExpressionNode(Position pos): ExpressionWithoutBlockNode(pos, true) {}

    ~PathExpressionNode() override = default;
};

class PathIndentSegmentNode;
class PathInExpressionNode: public PathExpressionNode {
    std::vector<PathIndentSegmentNode*> path_indent_segments_;
public:
    explicit PathInExpressionNode(Position pos,
        const std::vector<PathIndentSegmentNode*>& simple_path_segments):
        PathExpressionNode(pos) {
        path_indent_segments_ = simple_path_segments;
    }

    ~PathInExpressionNode() override;
};

/****************  Literal Node  ****************/
class LiteralExpressionNode: public ExpressionWithoutBlockNode {
public:
    explicit LiteralExpressionNode(Position pos):
        ExpressionWithoutBlockNode(pos, false) {}

    ~LiteralExpressionNode() override = default;
};

class CharLiteralNode: public LiteralExpressionNode {
    char char_literal_;
public:
    CharLiteralNode(Position pos, const char& char_literal):
        LiteralExpressionNode(pos) {
        char_literal_ = char_literal;
    }

    ~CharLiteralNode() override = default;
};

class StringLiteralNode: public LiteralExpressionNode {
    std::string string_literal_;
public:
    StringLiteralNode(Position pos, const std::string& string_literal):
        LiteralExpressionNode(pos) {
        string_literal_ = string_literal;
    }

    ~StringLiteralNode() override = default;
};

class IntLiteralNode: public LiteralExpressionNode {
    int64_t int_literal_;
public:
    IntLiteralNode(Position pos, const int64_t& int_literal):
        LiteralExpressionNode(pos) {
        int_literal_ = int_literal;
    }

    ~IntLiteralNode() override = default;
};

class BoolLiteralNode: public LiteralExpressionNode {
    bool bool_literal_;
public:
    BoolLiteralNode(Position pos, const bool& bool_literal):
        LiteralExpressionNode(pos) {
        bool_literal_ = bool_literal;
    }

    ~BoolLiteralNode() override = default;
};

class CStringLiteralNode: public LiteralExpressionNode {
    std::string c_string_literal_;
public:
    CStringLiteralNode(Position pos, const std::string& c_string_literal):
        LiteralExpressionNode(pos) {
        c_string_literal_ = c_string_literal;
    }

    ~CStringLiteralNode() override = default;
};

class ArrayLiteralNode: public LiteralExpressionNode {
    std::vector<ExpressionNode*> expressions_;
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    ArrayLiteralNode(Position pos, const std::vector<ExpressionNode*>& expression_nodes,
        ExpressionNode* lhs, ExpressionNode* rhs):
        LiteralExpressionNode(pos) {
        expressions_ = expression_nodes;
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~ArrayLiteralNode() override;
};

/****************  Support Node for Expression  ****************/

class ConditionsNode: public ASTNode {
    ExpressionNode* expression_ = nullptr;
public:
    ConditionsNode(Position pos, ExpressionNode* expression): ASTNode(pos) {
        expression_ = expression;
    }

    ~ConditionsNode() override;
};

class StatementsNode: public ASTNode {
    std::vector<StatementNode*> statements_;
    ExpressionNode* expression_ = nullptr;
public:
    StatementsNode(Position pos, const std::vector<StatementNode*>& statements,
        ExpressionNode* expression): ASTNode(pos) {
        statements_ = statements;
        expression_ = expression;
    }

    ~StatementsNode() override;
};

class MatchArmNode;
class MatchArmsNode: public ASTNode {
    std::vector<MatchArmNode*> match_arm_nodes_;
    std::vector<ExpressionNode*> expression_nodes_;
public:
    MatchArmsNode(Position pos, const std::vector<MatchArmNode*>& match_arm_nodes,
        const std::vector<ExpressionNode*>& expression_nodes): ASTNode(pos) {
        match_arm_nodes_ = match_arm_nodes;
        expression_nodes_ = expression_nodes;
    }

    ~MatchArmsNode() override;
};

class MatchArmGuardNode;
class MatchArmNode: public ASTNode {
    PatternNode* pattern_node_;
    ExpressionNode* match_arm_guard_;
public:
    MatchArmNode(Position pos, PatternNode* pattern_node,
        ExpressionNode* match_arm_guard): ASTNode(pos) {
        pattern_node_ = pattern_node;
        match_arm_guard_ = match_arm_guard;
    }

    ~MatchArmNode() override;
};

// TODO Add LetChain

/****************  Statement  ****************/
class StatementNode: public ASTNode {
public:
    explicit StatementNode(Position pos): ASTNode(pos) {}

    ~StatementNode() override = default;
};

class EmptyStatementNode: public StatementNode {
public:
    explicit EmptyStatementNode(Position pos): StatementNode(pos) {}

    ~EmptyStatementNode() override = default;
};

class LetStatementNode: public StatementNode {
    PatternNoTopAltNode* pattern_no_top_alt_ = nullptr;
    TypeNode* type_ = nullptr;
    ExpressionNode* expression_ = nullptr;
    BlockExpressionNode* block_expression_ = nullptr;
public:
    LetStatementNode(Position pos, PatternNoTopAltNode* pattern_no_top_alt, TypeNode* type,
        ExpressionNode* expression, BlockExpressionNode* block_expression): StatementNode(pos) {
        pattern_no_top_alt_ = pattern_no_top_alt;
        type_ = type;
        expression_ = expression;
        block_expression_ = block_expression;
    }

    ~LetStatementNode() override;
};

class ExpressionStatementNode: public StatementNode {
    ExpressionNode* expression_ = nullptr; // Except StructExpression
    // TODO: Add LetChain
public:
    ExpressionStatementNode(Position pos, ExpressionNode* expression): StatementNode(pos) {
        expression_ = expression;
    }

    ~ExpressionStatementNode() override;
};

/****************  Patterns  ****************/
class PatternNode: public ASTNode {
    std::vector<PatternNoTopAltNode*> pattern_no_top_alts_;
public:
    PatternNode(Position pos, const std::vector<PatternNoTopAltNode*>& pattern_no_top_alts):
        ASTNode(pos) {
        pattern_no_top_alts_ = pattern_no_top_alts;
    }

    ~PatternNode() override;
};

class PatternNoTopAltNode: public ASTNode {
public:
    explicit PatternNoTopAltNode(Position pos): ASTNode(pos) {}

    ~PatternNoTopAltNode() override = default;
};

class PatternWithoutRangeNode: public PatternNoTopAltNode {
public:
    explicit PatternWithoutRangeNode(Position pos): PatternNoTopAltNode(pos) {}

    ~PatternWithoutRangeNode() override = default;
};

class LiteralPatternNode: public PatternWithoutRangeNode {
    bool have_minus_ = false;
    ExpressionNode* expression_;
public:
    LiteralPatternNode(Position pos, bool have_minus, ExpressionNode* expression):
        PatternWithoutRangeNode(pos) {
        have_minus_ = have_minus;
        expression_ = expression;
    }

    ~LiteralPatternNode() override;
};

class IdentifierPatternNode: public PatternWithoutRangeNode {
    bool is_ref_ = false;
    bool is_mut_ = false;
    std::string identifier_;
    PatternNoTopAltNode* node_ = nullptr;
public:
    IdentifierPatternNode(Position pos, bool is_ref, bool is_mut, const std::string& identifier,
        PatternNoTopAltNode* node): PatternWithoutRangeNode(pos) {
        is_ref_ = is_ref;
        is_mut_ = is_mut;
        identifier_ = identifier;
        node_ = node;
    }

    ~IdentifierPatternNode() override;
};

class WildcardPatternNode: public PatternWithoutRangeNode {
public:
    explicit WildcardPatternNode(Position pos): PatternWithoutRangeNode(pos) {}

    ~WildcardPatternNode() override = default;
};

class RestPatternNode: public PatternWithoutRangeNode {
public:
    explicit RestPatternNode(Position pos): PatternWithoutRangeNode(pos) {}

    ~RestPatternNode() override = default;
};

// TODO StructPattern

// TODO TuplePattern

class GroupedPatternNode: public PatternWithoutRangeNode {
    PatternNode* pattern_ = nullptr;
public:
    GroupedPatternNode(Position pos, PatternNode* pattern):
        PatternWithoutRangeNode(pos) {
        pattern_ = pattern;
    }

    ~GroupedPatternNode() override;
};

class SlicePatternNode: public PatternWithoutRangeNode {
    std::vector<PatternNode*> patterns_;
public:
    SlicePatternNode(Position pos, const std::vector<PatternNode*> &patterns):
        PatternWithoutRangeNode(pos) {
        patterns_ = patterns;
    }

    ~SlicePatternNode() override;
};

class PathPatternNode: public PatternWithoutRangeNode {
    ExpressionNode* expression_;
public:
    PathPatternNode(Position pos, ExpressionNode* expression): PatternWithoutRangeNode(pos) {
        expression_ = expression;
    }

    ~PathPatternNode() override;
};

/****************  Types  ****************/
class TypeNode: public ASTNode {
public:
    explicit TypeNode(Position pos): ASTNode(pos) {}

    ~TypeNode() override = default;
};

class TypeNoBoundsNode: public TypeNode {
public:
    explicit TypeNoBoundsNode(Position pos): TypeNode(pos) {}

    ~TypeNoBoundsNode() override = default;
};

class ParenthesizedTypeNode: public TypeNoBoundsNode {
    TypeNode* type_ = nullptr;
public:
    ParenthesizedTypeNode(Position pos, TypeNode* type): TypeNoBoundsNode(pos) {
        type_ = type;
    }

    ~ParenthesizedTypeNode() override;
};

class TypePathSegmentNode;
class TypePathNode: public TypeNoBoundsNode {
    std::vector<TypePathSegmentNode*> type_path_segment_nodes_;
public:
    TypePathNode(Position pos, const std::vector<TypePathSegmentNode*>& type_path_segment_nodes):
        TypeNoBoundsNode(pos) {
        type_path_segment_nodes_ = type_path_segment_nodes;
    }

    ~TypePathNode() override;
};

class PathIndentSegmentNode;
class TypePathSegmentNode: public ASTNode {
    PathIndentSegmentNode* path_indent_segment_node_ = nullptr;
public:
    TypePathSegmentNode(Position pos, PathIndentSegmentNode* path_indent_segment_node): ASTNode(pos) {
        path_indent_segment_node_ = path_indent_segment_node;
    }

    ~TypePathSegmentNode() override;
};

class TupleTypeNode: public TypeNoBoundsNode {
    std::vector<TypeNode*> type_nodes_;
public:
    TupleTypeNode(Position pos, const std::vector<TypeNode*>& type_nodes): TypeNoBoundsNode(pos) {
        type_nodes_ = type_nodes;
    }

    ~TupleTypeNode() override;
};

class ArrayTypeNode: public TypeNoBoundsNode {
    TypeNode* type_;
    ExpressionNode* expression_node_ = nullptr;
public:
    ArrayTypeNode(Position pos, TypeNode* type, ExpressionNode* expression_node): TypeNoBoundsNode(pos) {
        type_ = type;
        expression_node_ = expression_node;
    }

    ~ArrayTypeNode() override;
};

class SliceTypeNode: public TypeNoBoundsNode {
    TypeNode* type_ = nullptr;
public:
    SliceTypeNode(Position pos, TypeNode* type): TypeNoBoundsNode(pos) {
        type_ = type;
    }

    ~SliceTypeNode() override;
};

class InferredTypeNode: public TypeNoBoundsNode {
public:
    explicit InferredTypeNode(Position pos): TypeNoBoundsNode(pos) {}

    ~InferredTypeNode() override = default;
};

/****************  Path (Naive Version) ****************/
class PathIndentSegmentNode: public ASTNode {
    TokenType type_;
    std::string identifier_;
public:
    PathIndentSegmentNode(Position pos, TokenType type, const std::string& identifier): ASTNode(pos) {
        type_ = type;
        identifier_ = identifier;
    }

    ~PathIndentSegmentNode() override = default;
};

#endif //ASTNODE_H