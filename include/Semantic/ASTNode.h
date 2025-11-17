#ifndef ASTNODE_H
#define ASTNODE_H

#include <utility>
#include <vector>
#include <string>
#include <memory> // Required for std::shared_ptr
#include "Position.h"
#include "Lexer/TokenType.h"
#include "Lexer/Token.h"
#include "Semantic/ASTVisitor.h"
#include "IR/IRVar.h"

using ConstValue = std::variant<int64_t,std::string>;

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
class VisItemStatementNode;
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
class LetChainConditionNode;
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
class ArrayTypeNode;
class SliceTypeNode;
class ReferenceTypeNode;

// Assuming 'Type' class is defined elsewhere and has a proper definition
class Type;

class ASTNode {
public:
    Position pos_;

    explicit ASTNode(const Position pos) : pos_(pos) {
    }

    virtual ~ASTNode() = default;

    virtual void accept(ASTVisitor *visitor) = 0;
};

/****************  Path (Naive Version) ****************/
class PathIndentSegmentNode : public ASTNode {
public:
    TokenType type_;
    std::string identifier_;

    PathIndentSegmentNode(Position pos, TokenType type, std::string identifier)
        : ASTNode(pos), type_(type), identifier_(std::move(identifier)) {
    }

    ~PathIndentSegmentNode() override = default;

    [[nodiscard]] std::string toString() const { return identifier_; }

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

/****************  Items  ****************/
class CrateNode : public ASTNode {
public:
	uint32_t scope_index = 0;
    std::vector<std::shared_ptr<VisItemNode> > items_;

    CrateNode(Position pos, std::vector<std::shared_ptr<VisItemNode> > items)
        : ASTNode(pos), items_(std::move(items)) {
    }

    ~CrateNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class VisItemNode : public ASTNode {
public:
    explicit VisItemNode(Position pos) : ASTNode(pos) {
    }

    ~VisItemNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class FunctionNode : public VisItemNode {
public:
    bool is_const_;
    std::string identifier_;
    std::shared_ptr<FunctionParametersNode> function_parameters_;
    std::shared_ptr<TypeNode> type_;
    std::shared_ptr<BlockExpressionNode> block_expression_;

    FunctionNode(Position pos, bool is_const, std::string identifier,
                 std::shared_ptr<FunctionParametersNode> function_parameters,
                 std::shared_ptr<TypeNode> type,
                 std::shared_ptr<BlockExpressionNode> block_expression)
        : VisItemNode(pos), is_const_(is_const), identifier_(std::move(identifier)),
          function_parameters_(std::move(function_parameters)),
          type_(std::move(type)),
          block_expression_(std::move(block_expression)) {
    }

    ~FunctionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class SelfParamNode;
class FunctionParametersNode : public ASTNode {
public:
    std::shared_ptr<SelfParamNode> self_param_node_;
    std::vector<std::shared_ptr<FunctionParamNode> > function_params_;

    FunctionParametersNode(Position pos, const std::shared_ptr<SelfParamNode>& self_param_node,
        std::vector<std::shared_ptr<FunctionParamNode> > function_params)
        : ASTNode(pos), function_params_(std::move(function_params)) {
        self_param_node_ = self_param_node;
    }

    ~FunctionParametersNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class SelfParamNode : public ASTNode {
public:
    explicit SelfParamNode(Position pos): ASTNode(pos) {}

    ~SelfParamNode() override = default;

    void accept(ASTVisitor* visitor) override {visitor -> visit(this);}
};

class ShortHandSelfNode : public SelfParamNode {
public:
    bool have_and_ = false;
    bool is_mut_ = false;

    ShortHandSelfNode(Position pos, bool have_and, bool is_mut): SelfParamNode(pos) {
        have_and_ = have_and;
        is_mut_ = is_mut;
    }

    ~ShortHandSelfNode() override = default;

    void accept(ASTVisitor *visitor) override {visitor -> visit(this);}
};

class TypedSelfNode : public SelfParamNode {
public:
    bool is_mut_;
    std::shared_ptr<TypeNode> type_node_;

    TypedSelfNode(Position pos, bool is_mut, const std::shared_ptr<TypeNode>& type_node):
        SelfParamNode(pos) {
        is_mut_ = is_mut;
        type_node_ = type_node;
    }

    ~TypedSelfNode() override = default;

    void accept(ASTVisitor *visitor) override {visitor -> visit(this);}
};

class FunctionParamNode : public ASTNode {
public:
    std::shared_ptr<PatternNoTopAltNode> pattern_no_top_alt_node_;
    std::shared_ptr<TypeNode> type_;
    bool is_DotDotDot_;

    FunctionParamNode(Position pos, std::shared_ptr<PatternNoTopAltNode> function_param_pattern,
                      std::shared_ptr<TypeNode> type, bool is_DotDotDot)
        : ASTNode(pos), pattern_no_top_alt_node_(std::move(function_param_pattern)),
          type_(std::move(type)), is_DotDotDot_(is_DotDotDot) {
    }

    ~FunctionParamNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class FunctionParamPatternNode : public ASTNode {
public:
    std::shared_ptr<PatternNoTopAltNode> pattern_no_top_alt_;
    std::shared_ptr<TypeNode> type_;
    bool is_DotDotDot_;

    FunctionParamPatternNode(Position pos, std::shared_ptr<PatternNoTopAltNode> pattern_no_top_alt,
                             std::shared_ptr<TypeNode> type, bool is_DotDotDot)
        : ASTNode(pos), pattern_no_top_alt_(std::move(pattern_no_top_alt)),
          type_(std::move(type)), is_DotDotDot_(is_DotDotDot) {
    }

    ~FunctionParamPatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class StructNode : public VisItemNode {
public:
    std::string identifier_;
    std::vector<std::shared_ptr<StructFieldNode> > struct_field_nodes_;

    StructNode(Position pos, std::string identifier,
               std::vector<std::shared_ptr<StructFieldNode> > struct_field_nodes)
        : VisItemNode(pos), identifier_(std::move(identifier)), struct_field_nodes_(std::move(struct_field_nodes)) {
    }

    ~StructNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class StructFieldNode : public ASTNode {
public:
    std::string identifier_;
    std::shared_ptr<TypeNode> type_node_;

    StructFieldNode(Position pos, std::string identifier, std::shared_ptr<TypeNode> type_node)
        : ASTNode(pos), identifier_(std::move(identifier)), type_node_(std::move(type_node)) {
    }

    ~StructFieldNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class EnumerationNode : public VisItemNode {
public:
    std::string identifier_;
    std::vector<std::shared_ptr<EnumVariantNode> > enum_variant_nodes_;

    EnumerationNode(Position pos, std::string identifier,
                    std::vector<std::shared_ptr<EnumVariantNode> > enum_variant_nodes)
        : VisItemNode(pos), identifier_(std::move(identifier)), enum_variant_nodes_(std::move(enum_variant_nodes)) {
    }

    ~EnumerationNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class EnumVariantNode : public ASTNode {
public:
    std::string identifier_;
    std::shared_ptr<EnumVariantStructNode> enum_variant_struct_node_;
    std::shared_ptr<EnumVariantDiscriminantNode> enum_variant_discriminant_node_;

    EnumVariantNode(Position pos, std::string identifier,
                    std::shared_ptr<EnumVariantStructNode> enum_variant_struct_node,
                    std::shared_ptr<EnumVariantDiscriminantNode> enum_variant_discriminant_node)
        : ASTNode(pos), identifier_(std::move(identifier)),
          enum_variant_struct_node_(std::move(enum_variant_struct_node)),
          enum_variant_discriminant_node_(std::move(enum_variant_discriminant_node)) {
    }

    ~EnumVariantNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class EnumVariantStructNode : public ASTNode {
public:
    std::vector<std::shared_ptr<StructFieldNode> > struct_field_nodes_;

    EnumVariantStructNode(Position pos, std::vector<std::shared_ptr<StructFieldNode> > struct_field_nodes)
        : ASTNode(pos), struct_field_nodes_(std::move(struct_field_nodes)) {
    }

    ~EnumVariantStructNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class EnumVariantDiscriminantNode : public ASTNode {
public:
    std::shared_ptr<ExpressionNode> expression_node_;

    EnumVariantDiscriminantNode(Position pos, std::shared_ptr<ExpressionNode> expression_node)
        : ASTNode(pos), expression_node_(std::move(expression_node)) {
    }

    ~EnumVariantDiscriminantNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ConstantItemNode : public VisItemNode {
public:
    std::string identifier_;
    bool is_underscore_;
    std::shared_ptr<TypeNode> type_node_;
    std::shared_ptr<ExpressionNode> expression_node_;

    ConstantItemNode(Position pos, std::string identifier, bool is_underscore,
                     std::shared_ptr<TypeNode> type_node, std::shared_ptr<ExpressionNode> expression_node)
        : VisItemNode(pos), identifier_(std::move(identifier)), is_underscore_(is_underscore),
          type_node_(std::move(type_node)), expression_node_(std::move(expression_node)) {
    }

    ~ConstantItemNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class AssociatedItemNode : public VisItemNode {
public:
    std::shared_ptr<ConstantItemNode> constant_item_node_;
    std::shared_ptr<FunctionNode> function_node_;

    AssociatedItemNode(Position pos, std::shared_ptr<ConstantItemNode> constant_item_node,
                       std::shared_ptr<FunctionNode> function_node)
        : VisItemNode(pos), constant_item_node_(std::move(constant_item_node)),
          function_node_(std::move(function_node)) {
    }

    ~AssociatedItemNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class TraitNode : public VisItemNode {
public:
    std::string identifier_;
    std::vector<std::shared_ptr<AssociatedItemNode> > associated_item_nodes_;

    TraitNode(Position pos, std::string identifier,
              std::vector<std::shared_ptr<AssociatedItemNode> > associated_item_nodes)
        : VisItemNode(pos), identifier_(std::move(identifier)),
          associated_item_nodes_(std::move(associated_item_nodes)) {
    }

    ~TraitNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ImplementationNode : public VisItemNode {
public:
    explicit ImplementationNode(Position pos) : VisItemNode(pos) {
    }

    ~ImplementationNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class InherentImplNode : public ImplementationNode {
public:
	uint32_t scope_index = 0;
    std::shared_ptr<TypeNode> type_node_;
    std::vector<std::shared_ptr<AssociatedItemNode> > associated_item_nodes_;

    InherentImplNode(Position pos, std::shared_ptr<TypeNode> type_node,
                     std::vector<std::shared_ptr<AssociatedItemNode> > associated_item_nodes)
        : ImplementationNode(pos), type_node_(std::move(type_node)),
          associated_item_nodes_(std::move(associated_item_nodes)) {
    }

    ~InherentImplNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class TraitImplNode : public ImplementationNode {
public:
    std::string identifier_;
    std::shared_ptr<TypeNode> type_node_;
    std::vector<std::shared_ptr<AssociatedItemNode> > associated_item_nodes_;

    TraitImplNode(Position pos, std::string identifier, std::shared_ptr<TypeNode> type_node,
                  std::vector<std::shared_ptr<AssociatedItemNode> > associated_item_nodes)
        : ImplementationNode(pos), identifier_(std::move(identifier)), type_node_(std::move(type_node)),
          associated_item_nodes_(std::move(associated_item_nodes)) {
    }

    ~TraitImplNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

/****************  Expression With Block  ****************/
class ExpressionNode : public ASTNode {
public:
    std::vector<std::shared_ptr<Type> > types;
    bool is_assignable_;
    bool is_mutable_;
    bool is_compiler_known_ = false;
	std::shared_ptr<LocalVar> result_var = nullptr;
    ConstValue value = 0;
    ExpressionNode(Position pos, bool is_assignable)
        : ASTNode(pos), is_assignable_(is_assignable), is_mutable_(false) {
    }

    ~ExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ExpressionWithBlockNode : public ExpressionNode {
public:
    explicit ExpressionWithBlockNode(Position pos) : ExpressionNode(pos, false) {
    }

    ~ExpressionWithBlockNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class BlockExpressionNode : public ExpressionWithBlockNode {
public:
	uint32_t scope_index = 0;
    bool is_const_;
	bool is_function_direct_block = false;
    std::shared_ptr<StatementsNode> statements_;

    BlockExpressionNode(Position pos, bool is_const, std::shared_ptr<StatementsNode> statements)
        : ExpressionWithBlockNode(pos), is_const_(is_const), statements_(std::move(statements)) {
    }

    ~BlockExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class LoopExpressionNode : public ExpressionWithBlockNode {
public:
    explicit LoopExpressionNode(Position pos) : ExpressionWithBlockNode(pos) {
    };

    ~LoopExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class InfiniteLoopExpressionNode : public LoopExpressionNode {
public:
    std::shared_ptr<BlockExpressionNode> block_expression_;

    InfiniteLoopExpressionNode(Position pos, std::shared_ptr<BlockExpressionNode> block_expression)
        : LoopExpressionNode(pos), block_expression_(std::move(block_expression)) {
    }

    ~InfiniteLoopExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class PredicateLoopExpressionNode : public LoopExpressionNode {
public:
    std::shared_ptr<ConditionsNode> conditions_;
    std::shared_ptr<BlockExpressionNode> block_expression_;

    PredicateLoopExpressionNode(Position pos, std::shared_ptr<ConditionsNode> conditions,
                                std::shared_ptr<BlockExpressionNode> block_expression)
        : LoopExpressionNode(pos), conditions_(std::move(conditions)),
          block_expression_(std::move(block_expression)) {
    }

    ~PredicateLoopExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class IfExpressionNode : public ExpressionWithBlockNode {
public:
    std::shared_ptr<ConditionsNode> conditions_;
    std::shared_ptr<BlockExpressionNode> true_block_expression_;
    std::shared_ptr<BlockExpressionNode> false_block_expression_;
    std::shared_ptr<IfExpressionNode> if_expression_;

    IfExpressionNode(Position pos, std::shared_ptr<ConditionsNode> conditions,
                     std::shared_ptr<BlockExpressionNode> true_block_expression,
                     std::shared_ptr<BlockExpressionNode> false_block_expression,
                     std::shared_ptr<IfExpressionNode> if_expression)
        : ExpressionWithBlockNode(pos), conditions_(std::move(conditions)),
          true_block_expression_(std::move(true_block_expression)),
          false_block_expression_(std::move(false_block_expression)),
          if_expression_(std::move(if_expression)) {
    }

    ~IfExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class MatchExpressionNode : public ExpressionWithBlockNode {
public:
    std::shared_ptr<ExpressionNode> expression_;
    std::shared_ptr<MatchArmsNode> match_arms_;

    MatchExpressionNode(Position pos, std::shared_ptr<ExpressionNode> expression,
                        std::shared_ptr<MatchArmsNode> match_arms_node)
        : ExpressionWithBlockNode(pos), expression_(std::move(expression)),
          match_arms_(std::move(match_arms_node)) {
    }

    ~MatchExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

/****************  Expression Without Block  ****************/
class ExpressionWithoutBlockNode : public ExpressionNode {
public:
    ExpressionWithoutBlockNode(Position pos, bool is_assignable) : ExpressionNode(pos, is_assignable) {
    }

    ~ExpressionWithoutBlockNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ContinueExpressionNode : public ExpressionWithoutBlockNode {
public:
    explicit ContinueExpressionNode(Position pos) : ExpressionWithoutBlockNode(pos, false) {
    }

    ~ContinueExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class TupleExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::vector<std::shared_ptr<ExpressionNode> > expressions_;

    TupleExpressionNode(Position pos, std::vector<std::shared_ptr<ExpressionNode> > expressions)
        : ExpressionWithoutBlockNode(pos, false), expressions_(std::move(expressions)) {
    }

    ~TupleExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class UnderscoreExpressionNode : public ExpressionWithoutBlockNode {
public:
    explicit UnderscoreExpressionNode(Position pos) : ExpressionWithoutBlockNode(pos, true) {
    }

    ~UnderscoreExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class JumpExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    std::shared_ptr<ExpressionNode> expression_;

    JumpExpressionNode(Position pos, TokenType type, std::shared_ptr<ExpressionNode> assignment_expression)
        : ExpressionWithoutBlockNode(pos, false), type_(type), expression_(std::move(assignment_expression)) {
    }

    ~JumpExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class AssignmentExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    AssignmentExpressionNode(Position pos, TokenType type,
                             std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~AssignmentExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class LogicOrExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    LogicOrExpressionNode(Position pos, std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~LogicOrExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class LogicAndExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    LogicAndExpressionNode(Position pos, std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~LogicAndExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ComparisonExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    ComparisonExpressionNode(Position pos, TokenType type, std::shared_ptr<ExpressionNode> lhs,
                             std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~ComparisonExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class BitwiseOrExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    BitwiseOrExpressionNode(Position pos, std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~BitwiseOrExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class BitwiseXorExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    BitwiseXorExpressionNode(Position pos, std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~BitwiseXorExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class BitwiseAndExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    BitwiseAndExpressionNode(Position pos, std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~BitwiseAndExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ShiftExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    ShiftExpressionNode(Position pos, TokenType type,
                        std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~ShiftExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class AddMinusExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    AddMinusExpressionNode(Position pos, TokenType type,
                           std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~AddMinusExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class MulDivModExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    MulDivModExpressionNode(Position pos, TokenType type,
                            std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : ExpressionWithoutBlockNode(pos, false), type_(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~MulDivModExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class TypeCastExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<TypeNode> type_;
    std::shared_ptr<ExpressionNode> expression_;

    TypeCastExpressionNode(Position pos, std::shared_ptr<TypeNode> type, std::shared_ptr<ExpressionNode> expression)
        : ExpressionWithoutBlockNode(pos, false), type_(std::move(type)), expression_(std::move(expression)) {
    }

    ~TypeCastExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class UnaryExpressionNode : public ExpressionWithoutBlockNode {
public:
    TokenType type_;
    std::shared_ptr<ExpressionNode> expression_;

    UnaryExpressionNode(Position pos, TokenType type,
                        std::shared_ptr<ExpressionNode> expression, bool is_assignable)
        : ExpressionWithoutBlockNode(pos, is_assignable), type_(type), expression_(std::move(expression)) {
    }

    ~UnaryExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class FunctionCallExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> callee_;
    std::vector<std::shared_ptr<ExpressionNode> > params_;

    FunctionCallExpressionNode(Position pos, std::shared_ptr<ExpressionNode> callee,
                               std::vector<std::shared_ptr<ExpressionNode> > params)
        : ExpressionWithoutBlockNode(pos, false), callee_(std::move(callee)), params_(std::move(params)) {
    }

    ~FunctionCallExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ArrayIndexExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> base_;
    std::shared_ptr<ExpressionNode> index_;

    ArrayIndexExpressionNode(Position pos, std::shared_ptr<ExpressionNode> base,
                             std::shared_ptr<ExpressionNode> index)
        : ExpressionWithoutBlockNode(pos, true), base_(std::move(base)), index_(std::move(index)) {
    }

    ~ArrayIndexExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class MemberAccessExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> base_;
    Token member_;

    MemberAccessExpressionNode(Position pos, std::shared_ptr<ExpressionNode> base, Token member)
        : ExpressionWithoutBlockNode(pos, true), base_(std::move(base)), member_(std::move(member)) {
    }

    ~MemberAccessExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class GroupedExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<ExpressionNode> expression_;

    GroupedExpressionNode(Position pos, std::shared_ptr<ExpressionNode> expression)
        : ExpressionWithoutBlockNode(pos, false), expression_(std::move(expression)) {
    }

    ~GroupedExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class StructExpressionNode : public ExpressionWithoutBlockNode {
public:
    std::shared_ptr<PathInExpressionNode> path_in_expression_node_;
    std::shared_ptr<StructExprFieldsNode> struct_expr_fields_node_;
    std::shared_ptr<StructBaseNode> struct_base_node_;

    StructExpressionNode(Position pos, std::shared_ptr<PathInExpressionNode> path_in_expression_node,
                         std::shared_ptr<StructExprFieldsNode> struct_expr_fields_node,
                         std::shared_ptr<StructBaseNode> struct_base_node)
        : ExpressionWithoutBlockNode(pos, false),
          path_in_expression_node_(std::move(path_in_expression_node)),
          struct_expr_fields_node_(std::move(struct_expr_fields_node)),
          struct_base_node_(std::move(struct_base_node)) {
    }

    ~StructExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class StructExprFieldsNode : public ASTNode {
public:
    std::vector<std::shared_ptr<StructExprFieldNode> > struct_expr_field_nodes_;
    std::shared_ptr<StructBaseNode> struct_base_node_;

    StructExprFieldsNode(Position pos, std::vector<std::shared_ptr<StructExprFieldNode> > struct_expr_field_nodes,
                         std::shared_ptr<StructBaseNode> struct_base)
        : ASTNode(pos), struct_expr_field_nodes_(std::move(struct_expr_field_nodes)),
          struct_base_node_(std::move(struct_base)) {
    }

    ~StructExprFieldsNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class StructExprFieldNode : public ASTNode {
public:
    std::string identifier_;
    std::shared_ptr<ExpressionNode> expression_node_;

    StructExprFieldNode(Position pos, std::string identifier,
                        std::shared_ptr<ExpressionNode> expression_node)
        : ASTNode(pos), identifier_(std::move(identifier)), expression_node_(std::move(expression_node)) {
    }

    ~StructExprFieldNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class StructBaseNode : public ASTNode {
public:
    std::shared_ptr<ExpressionNode> expression_node_;

    StructBaseNode(Position pos, std::shared_ptr<ExpressionNode> expression_node)
        : ASTNode(pos), expression_node_(std::move(expression_node)) {
    }

    ~StructBaseNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class PathExpressionNode : public ExpressionWithoutBlockNode {
public:
    explicit PathExpressionNode(Position pos) : ExpressionWithoutBlockNode(pos, true) {
    }

    ~PathExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class PathInExpressionNode : public PathExpressionNode {
public:
    std::vector<std::shared_ptr<PathIndentSegmentNode> > path_indent_segments_;

    explicit PathInExpressionNode(Position pos,
                                  std::vector<std::shared_ptr<PathIndentSegmentNode> > simple_path_segments)
        : PathExpressionNode(pos), path_indent_segments_(std::move(simple_path_segments)) {
    }

    ~PathInExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

/****************  Literal Node  ****************/
class LiteralExpressionNode : public ExpressionWithoutBlockNode {
public:
    explicit LiteralExpressionNode(Position pos) : ExpressionWithoutBlockNode(pos, false) {
    }

    ~LiteralExpressionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class CharLiteralNode : public LiteralExpressionNode {
public:
    char char_literal_;

    CharLiteralNode(Position pos, const char &char_literal)
        : LiteralExpressionNode(pos), char_literal_(char_literal) {
    }

    ~CharLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class StringLiteralNode : public LiteralExpressionNode {
public:
    std::string string_literal_;

    StringLiteralNode(Position pos, std::string string_literal)
        : LiteralExpressionNode(pos), string_literal_(std::move(string_literal)) {
    }

    ~StringLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class IntLiteralNode : public LiteralExpressionNode {
public:
    int64_t int_literal_;
    bool is_u32_, is_i32_, is_isize_, is_usize_;

    IntLiteralNode(Position pos, const int64_t &int_literal,
                   bool is_u32, bool is_i32, bool is_usize, bool is_isize)
        : LiteralExpressionNode(pos), int_literal_(int_literal), is_u32_(is_u32),
          is_i32_(is_i32), is_isize_(is_isize), is_usize_(is_usize) {
    }

    ~IntLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class BoolLiteralNode : public LiteralExpressionNode {
public:
    bool bool_literal_;

    BoolLiteralNode(Position pos, const bool &bool_literal)
        : LiteralExpressionNode(pos), bool_literal_(bool_literal) {
    }

    ~BoolLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class CStringLiteralNode : public LiteralExpressionNode {
public:
    std::string c_string_literal_;

    CStringLiteralNode(Position pos, std::string c_string_literal)
        : LiteralExpressionNode(pos), c_string_literal_(std::move(c_string_literal)) {
    }

    ~CStringLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ArrayLiteralNode : public LiteralExpressionNode {
public:
    std::vector<std::shared_ptr<ExpressionNode> > expressions_;
    std::shared_ptr<ExpressionNode> lhs_;
    std::shared_ptr<ExpressionNode> rhs_;

    ArrayLiteralNode(Position pos, std::vector<std::shared_ptr<ExpressionNode> > expression_nodes,
                     std::shared_ptr<ExpressionNode> lhs, std::shared_ptr<ExpressionNode> rhs)
        : LiteralExpressionNode(pos), expressions_(std::move(expression_nodes)),
          lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~ArrayLiteralNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

/****************  Support Node for Expression  ****************/
class ConditionsNode : public ASTNode {
public:
    std::shared_ptr<ExpressionNode> expression_;
    std::shared_ptr<LetChainNode> let_chain_node_;

    ConditionsNode(Position pos, std::shared_ptr<ExpressionNode> expression,
                   std::shared_ptr<LetChainNode> let_chain_node)
        : ASTNode(pos), expression_(std::move(expression)), let_chain_node_(std::move(let_chain_node)) {
    }

    ~ConditionsNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class LetChainConditionNode;

class LetChainNode : public ASTNode {
public:
    std::vector<std::shared_ptr<LetChainConditionNode> > let_chain_condition_nodes_;

    LetChainNode(Position pos, std::vector<std::shared_ptr<LetChainConditionNode> > let_chain_condition_nodes)
        : ASTNode(pos), let_chain_condition_nodes_(std::move(let_chain_condition_nodes)) {
    }

    ~LetChainNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class LetChainConditionNode : public ASTNode {
public:
    std::shared_ptr<PatternNode> pattern_node_;
    std::shared_ptr<ExpressionNode> expression_node_;

    LetChainConditionNode(Position pos, std::shared_ptr<PatternNode> pattern_node,
                          std::shared_ptr<ExpressionNode> expression_node)
        : ASTNode(pos), pattern_node_(std::move(pattern_node)),
          expression_node_(std::move(expression_node)) {
    }

    ~LetChainConditionNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class StatementsNode : public ASTNode {
public:
    std::vector<std::shared_ptr<StatementNode> > statements_;
    std::shared_ptr<ExpressionNode> expression_;

    StatementsNode(Position pos, std::vector<std::shared_ptr<StatementNode> > statements,
                   std::shared_ptr<ExpressionNode> expression)
        : ASTNode(pos), statements_(std::move(statements)), expression_(std::move(expression)) {
    }

    ~StatementsNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class MatchArmsNode : public ASTNode {
public:
    std::vector<std::shared_ptr<MatchArmNode> > match_arm_nodes_;
    std::vector<std::shared_ptr<ExpressionNode> > expression_nodes_;

    MatchArmsNode(Position pos, std::vector<std::shared_ptr<MatchArmNode> > match_arm_nodes,
                  std::vector<std::shared_ptr<ExpressionNode> > expression_nodes)
        : ASTNode(pos), match_arm_nodes_(std::move(match_arm_nodes)),
          expression_nodes_(std::move(expression_nodes)) {
    }

    ~MatchArmsNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class MatchArmNode : public ASTNode {
public:
    std::shared_ptr<PatternNode> pattern_node_;
    std::shared_ptr<ExpressionNode> match_arm_guard_;

    MatchArmNode(Position pos, std::shared_ptr<PatternNode> pattern_node,
                 std::shared_ptr<ExpressionNode> match_arm_guard)
        : ASTNode(pos), pattern_node_(std::move(pattern_node)),
          match_arm_guard_(std::move(match_arm_guard)) {
    }

    ~MatchArmNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

/****************  Statement  ****************/
class StatementNode : public ASTNode {
public:
    explicit StatementNode(Position pos) : ASTNode(pos) {
    }

    ~StatementNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class EmptyStatementNode : public StatementNode {
public:
    explicit EmptyStatementNode(Position pos) : StatementNode(pos) {
    }

    ~EmptyStatementNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class LetStatementNode : public StatementNode {
public:
    std::shared_ptr<PatternNoTopAltNode> pattern_no_top_alt_;
    std::shared_ptr<TypeNode> type_;
    std::shared_ptr<ExpressionNode> expression_;
    std::shared_ptr<BlockExpressionNode> block_expression_;

    LetStatementNode(Position pos, std::shared_ptr<PatternNoTopAltNode> pattern_no_top_alt,
                     std::shared_ptr<TypeNode> type,
                     std::shared_ptr<ExpressionNode> expression,
                     std::shared_ptr<BlockExpressionNode> block_expression)
        : StatementNode(pos), pattern_no_top_alt_(std::move(pattern_no_top_alt)),
          type_(std::move(type)), expression_(std::move(expression)),
          block_expression_(std::move(block_expression)) {
    }

    ~LetStatementNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ExpressionStatementNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> expression_;
    bool has_semicolon_ = true;

    ExpressionStatementNode(Position pos, std::shared_ptr<ExpressionNode> expression,
        bool has_semicolon = true)
        : StatementNode(pos), expression_(std::move(expression)) {
        has_semicolon_ = has_semicolon;
    }

    ~ExpressionStatementNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class VisItemStatementNode : public StatementNode {
public:
    std::shared_ptr<VisItemNode> vis_item_node_;

    VisItemStatementNode(Position pos, std::shared_ptr<VisItemNode> vis_item_node)
        : StatementNode(pos), vis_item_node_(std::move(vis_item_node)) {
    }

    ~VisItemStatementNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

/****************  Patterns  ****************/
class PatternNode : public ASTNode {
public:
    std::vector<std::shared_ptr<PatternNoTopAltNode> > pattern_no_top_alts_;

    PatternNode(Position pos, std::vector<std::shared_ptr<PatternNoTopAltNode> > pattern_no_top_alts)
        : ASTNode(pos), pattern_no_top_alts_(std::move(pattern_no_top_alts)) {
    }

    ~PatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class PatternNoTopAltNode : public ASTNode {
public:
    explicit PatternNoTopAltNode(Position pos) : ASTNode(pos) {
    }

    ~PatternNoTopAltNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class PatternWithoutRangeNode : public PatternNoTopAltNode {
public:
    explicit PatternWithoutRangeNode(Position pos) : PatternNoTopAltNode(pos) {
    }

    ~PatternWithoutRangeNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class LiteralPatternNode : public PatternWithoutRangeNode {
public:
    bool have_minus_;
    std::shared_ptr<ExpressionNode> expression_;

    LiteralPatternNode(Position pos, bool have_minus, std::shared_ptr<ExpressionNode> expression)
        : PatternWithoutRangeNode(pos), have_minus_(have_minus), expression_(std::move(expression)) {
    }

    ~LiteralPatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class IdentifierPatternNode : public PatternWithoutRangeNode {
public:
    bool is_ref_, is_mut_;
    std::string identifier_;
    std::shared_ptr<PatternNoTopAltNode> node_;

    IdentifierPatternNode(Position pos, bool is_ref, bool is_mut, std::string identifier,
                          std::shared_ptr<PatternNoTopAltNode> node)
        : PatternWithoutRangeNode(pos), is_ref_(is_ref), is_mut_(is_mut),
          identifier_(std::move(identifier)), node_(std::move(node)) {
    }

    ~IdentifierPatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class WildcardPatternNode : public PatternWithoutRangeNode {
public:
    explicit WildcardPatternNode(Position pos) : PatternWithoutRangeNode(pos) {
    }

    ~WildcardPatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class RestPatternNode : public PatternWithoutRangeNode {
public:
    explicit RestPatternNode(Position pos) : PatternWithoutRangeNode(pos) {
    }

    ~RestPatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class GroupedPatternNode : public PatternWithoutRangeNode {
public:
    std::shared_ptr<PatternNode> pattern_;

    GroupedPatternNode(Position pos, std::shared_ptr<PatternNode> pattern)
        : PatternWithoutRangeNode(pos), pattern_(std::move(pattern)) {
    }

    ~GroupedPatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class SlicePatternNode : public PatternWithoutRangeNode {
public:
    std::vector<std::shared_ptr<PatternNode> > patterns_;

    SlicePatternNode(Position pos, std::vector<std::shared_ptr<PatternNode> > patterns)
        : PatternWithoutRangeNode(pos), patterns_(std::move(patterns)) {
    }

    ~SlicePatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class PathPatternNode : public PatternWithoutRangeNode {
public:
    std::shared_ptr<ExpressionNode> expression_;

    PathPatternNode(Position pos, std::shared_ptr<ExpressionNode> expression)
        : PatternWithoutRangeNode(pos), expression_(std::move(expression)) {
    }

    ~PathPatternNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

/****************  Types  ****************/
class TypeNode : public ASTNode {
public:
    std::shared_ptr<Type> type;

    explicit TypeNode(Position pos) : ASTNode(pos) {
    }

    ~TypeNode() override = default;

    virtual std::string toString() = 0;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class TypeNoBoundsNode : public TypeNode {
public:
    explicit TypeNoBoundsNode(Position pos) : TypeNode(pos) {
    }

    ~TypeNoBoundsNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ParenthesizedTypeNode : public TypeNoBoundsNode {
public:
    std::shared_ptr<TypeNode> type_;

    ParenthesizedTypeNode(Position pos, std::shared_ptr<TypeNode> type)
        : TypeNoBoundsNode(pos), type_(std::move(type)) {
    }

    ~ParenthesizedTypeNode() override = default;

    std::string toString() override { return "( " + type_->toString() + " )"; }

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class TypePathSegmentNode : public ASTNode {
public:
    std::shared_ptr<PathIndentSegmentNode> path_indent_segment_node_;

    TypePathSegmentNode(Position pos, std::shared_ptr<PathIndentSegmentNode> path_indent_segment_node)
        : ASTNode(pos), path_indent_segment_node_(std::move(path_indent_segment_node)) {
    }

    ~TypePathSegmentNode() override = default;

    [[nodiscard]] std::string toString() const { return path_indent_segment_node_->toString(); }

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class TypePathNode : public TypeNoBoundsNode {
public:
    std::shared_ptr<TypePathSegmentNode> type_path_segment_node_;

    TypePathNode(Position pos, std::shared_ptr<TypePathSegmentNode> type_path_segment_node)
        : TypeNoBoundsNode(pos), type_path_segment_node_(std::move(type_path_segment_node)) {
    }

    ~TypePathNode() override = default;

    std::string toString() override { return type_path_segment_node_->toString(); }

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class UnitTypeNode : public TypeNoBoundsNode {
public:
    explicit UnitTypeNode(Position pos): TypeNoBoundsNode(pos) {
    }

    ~UnitTypeNode() override = default;

    [[nodiscard]] std::string toString() override {
        return "()";
    }

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ArrayTypeNode : public TypeNoBoundsNode {
public:
    std::shared_ptr<TypeNode> type_;
    std::shared_ptr<ExpressionNode> expression_node_;

    ArrayTypeNode(Position pos, std::shared_ptr<TypeNode> type, std::shared_ptr<ExpressionNode> expression_node)
        : TypeNoBoundsNode(pos), type_(std::move(type)), expression_node_(std::move(expression_node)) {
    }

    ~ArrayTypeNode() override = default;

    [[nodiscard]] std::string toString() override {
        std::string str = "[";
        str += type_->toString();
        str += ";";

        auto tmp = std::dynamic_pointer_cast<IntLiteralNode>(expression_node_);
        if (!tmp) {
            str += "_";
        } else {
            str += std::to_string(tmp->int_literal_);
        }
        str += "]";
        return str;
    }

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class SliceTypeNode : public TypeNoBoundsNode {
public:
    std::shared_ptr<TypeNode> type_;

    SliceTypeNode(Position pos, std::shared_ptr<TypeNode> type)
        : TypeNoBoundsNode(pos), type_(std::move(type)) {
    }

    ~SliceTypeNode() override = default;

    [[nodiscard]] std::string toString() override { return "[" + type_->toString() + "]"; }

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

class ReferenceTypeNode : public TypeNoBoundsNode {
public:
    bool is_mut_;
    std::shared_ptr<TypeNode> type_node_;

    ReferenceTypeNode(Position pos, bool is_mut, const std::shared_ptr<TypeNode> &type_node)
        : TypeNoBoundsNode(pos), is_mut_(is_mut), type_node_(type_node) {
    }

    [[nodiscard]] std::string toString() override {
        std::string str = "& ";
        if (is_mut_) str += "mut ";
        str += type_node_ -> toString();
        return str;
    }

    ~ReferenceTypeNode() override = default;

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
};

#endif //ASTNODE_H
