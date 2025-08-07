#ifndef ASTNODE_H
#define ASTNODE_H
#include <utility>
#include <vector>
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
class FunctionQualifiersNode;
class GenericParamNode; // Use std::vector to perform GenericParamNode
class FunctionParametersNode;
class FunctionReturnTypeNode;
class WhereClauseItemNode;
/* Use std::vector to perform WhereClauseNode, and since we do not have LifeTimeClauseItem,
 * it just represents TypeBoundClauseItem. */
class BlockExpressionNode;
class ItemSafetyNode;
class SelfParamNode;
class FunctionParamNode;
class ShortHandSelfNode;
class TypedSelfNode;
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
class BlockExpressionNode;
class ConstBlockExpressionNode;
class LoopExpressionNode;
class IfExpressionNode;
class MatchExpressionNode;
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
    Position pos;
public:
    explicit ASTNode(const Position pos) {
        this -> pos = pos;
    }

    virtual ~ASTNode() = default;
};

class CrateNode: public ASTNode {
    Position pos;
    std::vector<VisItemNode*> items;
public:
    explicit CrateNode(const Position pos, std::vector<VisItemNode*> items): ASTNode(pos) {
        this -> items = std::move(items);
    }

    ~CrateNode() override {
        for (auto & item : items) {
            item.~VisItemNode();
        }
    }
};

#endif //ASTNODE_H
