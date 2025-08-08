#ifndef ASTNODE_H
#define ASTNODE_H
#include <utility>
#include <vector>
#include <string>
#include "../../util/Position.h"

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
class StructStructNode;
class TupleStructNode;
class StructFieldNode; // Use std::vector to perform StructFields
class TupleFieldNode;  // Use std::vector to perform TupleFields
class EnumVariantNode; // Use std::vector to perform EnumVariants
class EnumVariantTupleNode;
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
class PatternNoTopAltNode;
class ExpressionNode; // Also StructBaseNode
class ExpressionWithoutBlockNode;
class ExpressionWithBlockNode;
class LiteralExpressionNode;
class PathExpressionNode;
class OperatorExpressionNode;
class GroupedExpressionNode;
class ArrayExpressionNode;
class IndexExpressionNode;
class TupleExpressionNode;
class TupleIndexingExpressionNode;
class StructExpressionNode;
class CallExpressionNode;
class MethodCallExpressionNode;
class FieldExpressionNode;
class ContinueExpressionNode;
class BreakExpressionNode;
class ReturnExpressionNode;
class UnderscoreExpressionNode;
class BlockExpressionNode; // use is_const_ to show whether this block expression is constant
class LoopExpressionNode;
class IfExpressionNode;
class MatchExpressionNode;
class MatchArmsNode;
class PathInExpressionNode;
class QualifiedPathInExpressionNode;
class StatementsNode;
class OperatorExpressionNode;
class NegationExpressionNode;
class ArithmeticOrLogicalExpressionNode;
class ComparisonExpressionNode;
class LazyBooleanExpressionNode;
class TypeCastExpressionNode;
class AssignmentExpressionNode;
class CompoundAssignmentExpressionNode;
class StructExprFieldNode;
class ConditionsNode;
class LetChainNode;


class ASTNode {
protected:
    Position pos_;
public:
    explicit ASTNode(const Position pos) {
        pos_ = pos;
    }

    virtual ~ASTNode() = default;
};

class CrateNode: public ASTNode {
    std::vector<ASTNode*> items;
public:
    CrateNode(const Position pos, std::vector<ASTNode*> items): ASTNode(pos) {
        this -> items = std::move(items);
    }

    ~CrateNode() override {
        for (auto & item : items) {
            delete item;
        }
    }
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
    std::vector<GenericParamNode*> generic_param_;
    FunctionParametersNode* function_parameters_ = nullptr;
    TypeNode* type_ = nullptr;
    std::vector<WhereClauseItemNode*> where_clause_items_;
    BlockExpressionNode* block_expression_ = nullptr;
public:
    FunctionNode(Position pos, bool is_const, const std::string& identifier,
        const std::vector<GenericParamNode*>& generic_param, FunctionParametersNode* function_parameters,
        TypeNode* type, const std::vector<WhereClauseItemNode*>& where_clause_items,
        BlockExpressionNode* block_expression): ASTNode(pos) {
        is_const_ = is_const;
        identifier_ = identifier;
        generic_param_ = generic_param;
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
public:
    FunctionParamNode(Position pos, FunctionParamPatternNode* function_param_pattern,
        TypeNode* type): ASTNode(pos) {
        function_param_pattern_ = function_param_pattern;
        type_ = type;
    }

    ~FunctionParamNode() override;
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
    bool is_const_ = false;
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

class InfiniteLoopExpression: public LoopExpressionNode {
    BlockExpressionNode* block_expression_ = nullptr;
public:
    InfiniteLoopExpression(Position pos, BlockExpressionNode* block_expression):
        LoopExpressionNode(pos) {
        block_expression_ = block_expression;
    }

    ~InfiniteLoopExpression() override;
};

class PredicateLoopExpression: public LoopExpressionNode {
    ConditionsNode* conditions_ = nullptr;
    BlockExpressionNode* block_expression_ = nullptr;
public:
    PredicateLoopExpression(Position pos, ConditionsNode* conditions,
        BlockExpressionNode* block_expression): LoopExpressionNode(pos) {
        conditions_ = conditions;
        block_expression_ = block_expression;
    }

    ~PredicateLoopExpression() override;
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
    MatchExpressionNode(Position pos, ExpressionNode* expression): ExpressionWithBlockNode(pos) {
        expression_ = expression;
    }

    ~MatchExpressionNode() override;
};

/****************  Expression Without Block  ****************/
class ExpressionWithoutBlockNode: public ExpressionNode {

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

// TODO Define MatchArmNode

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

/****************  Support Node for Statement  ****************/

// TODO: Define Statements

/****************  Patterns  ****************/

// TODO: Define FunctionParamPattern

// TODO: Define PatternNoTopAlt

/****************  Types  ****************/

// TODO: Define TypeNode
#endif //ASTNODE_H
