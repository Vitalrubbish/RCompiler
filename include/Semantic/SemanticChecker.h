#ifndef SEMANTICCHECKER_H
#define SEMANTICCHECKER_H
#include "ASTNode.h"

class SemanticChecker : public ASTVisitor {
    ScopeManager& scope_manager_;
public:
    explicit SemanticChecker(ScopeManager & scope_manager):
        scope_manager_(scope_manager) {}

    ~SemanticChecker() override = default;

    void visit(ASTNode *node) override;
    void visit(CrateNode *node) override;
    void visit(VisItemNode *node) override;
    void visit(FunctionNode *node) override;
    void visit(StructNode *node) override;
    void visit(EnumerationNode *node) override;
    void visit(ConstantItemNode *node) override;
    void visit(TraitNode *node) override;
    void visit(ImplementationNode *node) override;
    void visit(AssociatedItemNode *node) override;
    void visit(InherentImplNode *node) override;
    void visit(TraitImplNode *node) override;
    void visit(FunctionParametersNode *node) override;
    void visit(FunctionParamNode *node) override;
    void visit(FunctionParamPatternNode *node) override;
    void visit(StructFieldNode *node) override;
    void visit(EnumVariantNode *node) override;
    void visit(EnumVariantStructNode *node) override;
    void visit(EnumVariantDiscriminantNode *node) override;
    void visit(TypeParamBoundsNode *node) override;
    void visit(TypeParamNode *node) override;
    void visit(ConstParamNode *node) override;
    void visit(StatementNode *node) override;
    void visit(StatementsNode *node) override;
    void visit(EmptyStatementNode *node) override;
    void visit(LetStatementNode *node) override;
    void visit(VisItemStatementNode *node) override;
    void visit(ExpressionStatementNode *node) override;
    void visit(ExpressionNode *node) override;
    void visit(ExpressionWithoutBlockNode *node) override;
    void visit(ExpressionWithBlockNode *node) override;
    void visit(ComparisonExpressionNode *node) override;
    void visit(TypeCastExpressionNode *node) override;
    void visit(AssignmentExpressionNode *node) override;
    void visit(ContinueExpressionNode *node) override;
    void visit(UnderscoreExpressionNode *node) override;
    void visit(JumpExpressionNode *node) override;
    void visit(LogicOrExpressionNode *node) override;
    void visit(LogicAndExpressionNode *node) override;
    void visit(BitwiseOrExpressionNode *node) override;
    void visit(BitwiseXorExpressionNode *node) override;
    void visit(BitwiseAndExpressionNode *node) override;
    void visit(ShiftExpressionNode *node) override;
    void visit(AddMinusExpressionNode *node) override;
    void visit(MulDivModExpressionNode *node) override;
    void visit(UnaryExpressionNode *node) override;
    void visit(FunctionCallExpressionNode *node) override;
    void visit(ArrayIndexExpressionNode *node) override;
    void visit(MemberAccessExpressionNode *node) override;
    void visit(BlockExpressionNode *node) override;
    void visit(LoopExpressionNode *node) override;
    void visit(InfiniteLoopExpressionNode *node) override;
    void visit(PredicateLoopExpressionNode *node) override;
    void visit(IfExpressionNode *node) override;
    void visit(MatchExpressionNode *node) override;
    void visit(LiteralExpressionNode *node) override;
    void visit(CharLiteralNode *node) override;
    void visit(StringLiteralNode *node) override;
    void visit(CStringLiteralNode *node) override;
    void visit(IntLiteralNode *node) override;
    void visit(BoolLiteralNode *node) override;
    void visit(ArrayLiteralNode *node) override;
    void visit(PathExpressionNode *node) override;
    void visit(PathInExpressionNode *node) override;
    void visit(QualifiedPathInExpressionNode *node) override;
    void visit(PathIndentSegmentNode *node) override;
    void visit(StructExpressionNode *node) override;
    void visit(StructExprFieldsNode *node) override;
    void visit(StructExprFieldNode *node) override;
    void visit(StructBaseNode *node) override;
    void visit(GroupedExpressionNode *node) override;
    void visit(TupleExpressionNode *node) override;
    void visit(ConditionsNode *node) override;
    void visit(LetChainNode *node) override;
    void visit(LetChainConditionNode *node) override;
    void visit(MatchArmsNode *node) override;
    void visit(MatchArmNode *node) override;
    void visit(MatchArmGuardNode *node) override;
    void visit(PatternNode *node) override;
    void visit(PatternNoTopAltNode *node) override;
    void visit(PatternWithoutRangeNode *node) override;
    void visit(LiteralPatternNode *node) override;
    void visit(IdentifierPatternNode *node) override;
    void visit(WildcardPatternNode *node) override;
    void visit(RestPatternNode *node) override;
    void visit(GroupedPatternNode *node) override;
    void visit(SlicePatternNode *node) override;
    void visit(PathPatternNode *node) override;
    void visit(TypeNode *node) override;
    void visit(TypeNoBoundsNode *node) override;
    void visit(ParenthesizedTypeNode *node) override;
    void visit(TypePathNode *node) override;
    void visit(TypePathSegmentNode *node) override;
    void visit(TupleTypeNode *node) override;
    void visit(ArrayTypeNode *node) override;
    void visit(SliceTypeNode *node) override;
    void visit(ReferenceTypeNode *node) override;

    static std::vector<std::shared_ptr<Type>> cap(const std::vector<std::shared_ptr<Type>>& a,
    const std::vector<std::shared_ptr<Type>>& b);
};
#endif //SEMANTICCHECKER_H
