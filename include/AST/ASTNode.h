#ifndef ASTNODE_H
#define ASTNODE_H
#include <utility>
#include <vector>
#include <string>
#include "../../util/Position.h"
#include "../Lexer/TokenType.h"

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
class ExpressionNode; // Also StructBaseNode
class ExpressionWithoutBlockNode;
class ExpressionWithBlockNode;
class LiteralExpressionNode;

class CharLiteralNode;
class ByteLiteralNode;
class StringLiteralNode;
class IntLiteralNode;
class BoolLiteralNode;


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

class FunctionParamPatternNode: public ASTNode {
    PatternNoTopAltNode* pattern_no_top_alt_;
    TypeNode* type_;
public:
    FunctionParamPatternNode(Position pos, PatternNoTopAltNode* pattern_no_top_alt,
        TypeNode* type): ASTNode(pos) {
        pattern_no_top_alt_ = pattern_no_top_alt;
        type_ = type;
    }

    ~FunctionParamPatternNode() override;
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
    StatementsNode* statements_;
public:
    BlockExpressionNode(Position pos, StatementsNode* statements):
        ExpressionWithBlockNode(pos) {
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
    std::vector<ExpressionNode*> expressions_;
public:
    LogicOrExpressionNode(Position pos,
        const std::vector<ExpressionNode*>& expressions):
        ExpressionWithoutBlockNode(pos, false) {
        expressions_ = expressions;
    }

    ~LogicOrExpressionNode() override;
};

class LogicAndExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<ExpressionNode*> expressions_;
public:
    LogicAndExpressionNode(Position pos,
        const std::vector<ExpressionNode*>& expressions):
        ExpressionWithoutBlockNode(pos, false) {
        expressions_ = expressions;
    }

    ~LogicAndExpressionNode() override;
};

class BitwiseOrExpressionNode;
class ComparisonExpressionNode: public ExpressionWithoutBlockNode {
    ExpressionNode* lhs_;
    ExpressionNode* rhs_;
public:
    ComparisonExpressionNode(Position pos, ExpressionNode* lhs,
        ExpressionNode* rhs): ExpressionWithoutBlockNode(pos, false) {
        lhs_ = lhs;
        rhs_ = rhs;
    }

    ~ComparisonExpressionNode() override;
};

class BitwiseXorExpressionNode;
class BitwiseOrExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<ExpressionNode*> expressions_;
public:
    BitwiseOrExpressionNode(Position pos,
        const std::vector<ExpressionNode*>& expression):
        ExpressionWithoutBlockNode(pos, false) {
        expressions_ = expression;
    }

    ~BitwiseOrExpressionNode() override;
};

class BitwiseAndExpressionNode;
class BitwiseXorExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<ExpressionNode*> expressions_;
public:
    BitwiseXorExpressionNode(Position pos,
        const std::vector<ExpressionNode*>& expressions):
        ExpressionWithoutBlockNode(pos, false) {
        expressions_ = expressions;
    }

    ~BitwiseXorExpressionNode() override;
};

class ShiftExpressionNode;
class BitwiseAndExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<ExpressionNode*> expressions_;
public:
    BitwiseAndExpressionNode(Position pos,
        const std::vector<ExpressionNode*>& expressions):
        ExpressionWithoutBlockNode(pos, false) {
        expressions_ = expressions;
    }

    ~BitwiseAndExpressionNode() override;
};

class AddMinusExpressionNode;
class ShiftExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<TokenType> types_;
    std::vector<ExpressionNode*> expressions_;
public:
    ShiftExpressionNode(Position pos, const std::vector<TokenType>& types,
        const std::vector<ExpressionNode*>& expressions):
        ExpressionWithoutBlockNode(pos, false) {
        types_ = types;
        expressions_ = expressions;
    }

    ~ShiftExpressionNode() override;
};

class MulDivModExpressionNode;
class AddMinusExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<TokenType> types_;
    std::vector<ExpressionNode*> expressions_;
public:
    AddMinusExpressionNode(Position pos, const std::vector<TokenType>& types,
        const std::vector<ExpressionNode*>& expressions):
        ExpressionWithoutBlockNode(pos, false) {
        types_ = types;
        expressions_ = expressions;
    }

    ~AddMinusExpressionNode() override;
};

class MulDivModExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<TokenType> types_;
    std::vector<ExpressionNode*> expressions_;
public:
    MulDivModExpressionNode(Position pos, const std::vector<TokenType>& types,
        const std::vector<ExpressionNode*>& expressions):
        ExpressionWithoutBlockNode(pos, false) {
        types_ = types;
        expressions_ = expressions;
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

class MemberCallExpressionNode: public ExpressionWithoutBlockNode {
    ExpressionNode* expression_;
    std::vector<ASTNode*> suffixes_;
public:
    MemberCallExpressionNode(Position pos, ExpressionNode* expression,
        const std::vector<ASTNode*>& suffixes, bool is_assignable): ExpressionWithoutBlockNode(pos, is_assignable) {
        expression_ = expression;
        suffixes_ = suffixes;
    } // Notice that assignable is needed to be a param.

    ~MemberCallExpressionNode() override;
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

class SimplePathSegment;
class PathExpressionNode: public ExpressionWithoutBlockNode {
    std::vector<SimplePathSegment*> simple_path_segments_;
public:
    explicit PathExpressionNode(Position pos,
        const std::vector<SimplePathSegment*>& simple_path_segments):
        ExpressionWithoutBlockNode(pos, true) {
        simple_path_segments_ = simple_path_segments;
    }

    ~PathExpressionNode() override; // TODO
};
/* Without templates, PathExpression just means SimplePath.
 * Implementation will be modified when implementing templates.
 */

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
    uint32_t int_literal_;
public:
    IntLiteralNode(Position pos, const uint32_t& int_literal):
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

// TODO Define ByteLiteralNode

// TODO Define ByteStringLiteralNode

// TODO Define CStringLiteralNode

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

// TODO Define MatchArmNode

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

// TODO Define StructPattern

// TODO TupleStructPattern

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
    TypeNode* type_;
public:
    ParenthesizedTypeNode(Position pos, TypeNode* type): TypeNoBoundsNode(pos) {
        type_ = type;
    }

    ~ParenthesizedTypeNode() override;
};

/****************  Path (Naive Version) ****************/
class SimplePathSegment: public ASTNode {
    TokenType type_;
    std::string identifier_;
public:
    SimplePathSegment(Position pos, TokenType type, const std::string& identifier):
        ASTNode(pos) {
        type_ = type;
        identifier_ = identifier;
    }

    ~SimplePathSegment() override = default;
};

#endif //ASTNODE_H
