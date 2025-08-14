#include "Semantic/ASTVisitor.h"
#include "Semantic/ASTNode.h"

void ASTVisitor::visit(ASTNode *node) {
}

void ASTVisitor::visit(CrateNode *node) {
    for (auto *item: node->items_) {
        if (item) item->accept(this);
    }
}

void ASTVisitor::visit(VisItemNode *node) {
}

void ASTVisitor::visit(ModuleNode *node) {
}

void ASTVisitor::visit(FunctionNode *node) {
    if (node->function_parameters_) node->function_parameters_->accept(this);
    if (node->type_) node->type_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void ASTVisitor::visit(StructNode *node) {
    for (auto *field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void ASTVisitor::visit(EnumerationNode *node) {
    for (auto *variant: node->enum_variant_nodes_) {
        if (variant) variant->accept(this);
    }
}

void ASTVisitor::visit(ConstantItemNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
    if (node->expression_node_) node->expression_node_->accept(this);
}

void ASTVisitor::visit(TraitNode *node) {
}

void ASTVisitor::visit(ImplementationNode *node) {
}

void ASTVisitor::visit(AssociatedItemNode *node) {
    if (node->constant_item_node_) node->constant_item_node_->accept(this);
    if (node->function_node_) node->function_node_->accept(this);
}

void ASTVisitor::visit(InherentImplNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
    for (auto *item: node->associated_item_nodes_) {
        if (item) item->accept(this);
    }
}

void ASTVisitor::visit(TraitImplNode *node) {
}

void ASTVisitor::visit(FunctionParametersNode *node) {
    for (auto *param: node->function_params_) {
        if (param) param->accept(this);
    }
}

void ASTVisitor::visit(FunctionParamNode *node) {
    if (node->function_param_pattern_) node->function_param_pattern_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void ASTVisitor::visit(FunctionParamPatternNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void ASTVisitor::visit(StructFieldNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
}

void ASTVisitor::visit(EnumVariantNode *node) {
    if (node->enum_variant_struct_node_) node->enum_variant_struct_node_->accept(this);
    if (node->enum_variant_discriminant_node_) node->enum_variant_discriminant_node_->accept(this);
}

void ASTVisitor::visit(EnumVariantStructNode *node) {
    for (auto *field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void ASTVisitor::visit(EnumVariantDiscriminantNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void ASTVisitor::visit(TypeParamBoundsNode *node) {
}

void ASTVisitor::visit(TypeParamNode *node) {
}

void ASTVisitor::visit(ConstParamNode *node) {
}

void ASTVisitor::visit(StatementNode *node) {
}

void ASTVisitor::visit(StatementsNode *node) {
    for (auto *stmt: node->statements_) {
        if (stmt) stmt->accept(this);
    }
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(EmptyStatementNode *node) {
}

void ASTVisitor::visit(LetStatementNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
    if (node->expression_) node->expression_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void ASTVisitor::visit(ExpressionStatementNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(ExpressionNode *node) {
}

void ASTVisitor::visit(ExpressionWithoutBlockNode *node) {
}

void ASTVisitor::visit(ExpressionWithBlockNode *node) {
}

void ASTVisitor::visit(ComparisonExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(TypeCastExpressionNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(AssignmentExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(ContinueExpressionNode *node) {
}

void ASTVisitor::visit(UnderscoreExpressionNode *node) {
}

void ASTVisitor::visit(JumpExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(LogicOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(LogicAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(BitwiseOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(BitwiseXorExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(BitwiseAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(ShiftExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(AddMinusExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(MulDivModExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(UnaryExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(FunctionCallExpressionNode *node) {
    if (node->callee_) node->callee_->accept(this);
    for (auto *param: node->params_) {
        if (param) param->accept(this);
    }
}

void ASTVisitor::visit(ArrayIndexExpressionNode *node) {
    if (node->base_) node->base_->accept(this);
    if (node->index_) node->index_->accept(this);
}

void ASTVisitor::visit(MemberAccessExpressionNode *node) {
    if (node->base_) node->base_->accept(this);
}

void ASTVisitor::visit(BlockExpressionNode *node) {
    if (node->statements_) node->statements_->accept(this);
}

void ASTVisitor::visit(LoopExpressionNode *node) {
}

void ASTVisitor::visit(InfiniteLoopExpressionNode *node) {
    if (node->block_expression_) node->block_expression_->accept(this);
}

void ASTVisitor::visit(PredicateLoopExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void ASTVisitor::visit(IfExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->true_block_expression_) node->true_block_expression_->accept(this);
    if (node->false_block_expression_) node->false_block_expression_->accept(this);
    if (node->if_expression_) node->if_expression_->accept(this);
}

void ASTVisitor::visit(MatchExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
    if (node->match_arms_) node->match_arms_->accept(this);
}

void ASTVisitor::visit(LiteralExpressionNode *node) {
}

void ASTVisitor::visit(CharLiteralNode *node) {
}

void ASTVisitor::visit(StringLiteralNode *node) {
}

void ASTVisitor::visit(CStringLiteralNode *node) {
}

void ASTVisitor::visit(IntLiteralNode *node) {
}

void ASTVisitor::visit(BoolLiteralNode *node) {
}

void ASTVisitor::visit(ArrayLiteralNode *node) {
    for (auto *expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void ASTVisitor::visit(PathExpressionNode *node) {
}

void ASTVisitor::visit(PathInExpressionNode *node) {
    for (auto *seg: node->path_indent_segments_) {
        if (seg) seg->accept(this);
    }
}

void ASTVisitor::visit(QualifiedPathInExpressionNode *node) {
}

void ASTVisitor::visit(PathIndentSegmentNode *node) {
}

void ASTVisitor::visit(StructExpressionNode *node) {
    if (node->path_in_expression_node_) node->path_in_expression_node_->accept(this);
    if (node->struct_expr_fields_node_) node->struct_expr_fields_node_->accept(this);
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void ASTVisitor::visit(StructExprFieldsNode *node) {
    for (auto *field: node->struct_expr_field_nodes_) {
        if (field) field->accept(this);
    }
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void ASTVisitor::visit(StructExprFieldNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void ASTVisitor::visit(StructBaseNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void ASTVisitor::visit(GroupedExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(TupleExpressionNode *node) {
    for (auto *expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
}

void ASTVisitor::visit(ConditionsNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(LetChainNode *node) {
}

void ASTVisitor::visit(MatchArmsNode *node) {
    for (auto *arm: node->match_arm_nodes_) {
        if (arm) arm->accept(this);
    }
    for (auto *expr: node->expression_nodes_) {
        if (expr) expr->accept(this);
    }
}

void ASTVisitor::visit(MatchArmNode *node) {
    if (node->pattern_node_) node->pattern_node_->accept(this);
    if (node->match_arm_guard_) node->match_arm_guard_->accept(this);
}

void ASTVisitor::visit(MatchArmGuardNode *node) {
}

void ASTVisitor::visit(PatternNode *node) {
    for (auto *pat: node->pattern_no_top_alts_) {
        if (pat) pat->accept(this);
    }
}

void ASTVisitor::visit(PatternNoTopAltNode *node) {
}

void ASTVisitor::visit(PatternWithoutRangeNode *node) {
}

void ASTVisitor::visit(LiteralPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(IdentifierPatternNode *node) {
    if (node->node_) node->node_->accept(this);
}

void ASTVisitor::visit(WildcardPatternNode *node) {
}

void ASTVisitor::visit(RestPatternNode *node) {
}

void ASTVisitor::visit(GroupedPatternNode *node) {
    if (node->pattern_) node->pattern_->accept(this);
}

void ASTVisitor::visit(SlicePatternNode *node) {
    for (auto *pat: node->patterns_) {
        if (pat) pat->accept(this);
    }
}

void ASTVisitor::visit(PathPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void ASTVisitor::visit(TypeNode *node) {
}

void ASTVisitor::visit(TypeNoBoundsNode *node) {
}

void ASTVisitor::visit(ParenthesizedTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void ASTVisitor::visit(TypePathNode *node) {
    for (auto *seg: node->type_path_segment_nodes_) {
        if (seg) seg->accept(this);
    }
}

void ASTVisitor::visit(TypePathSegmentNode *node) {
    if (node->path_indent_segment_node_) node->path_indent_segment_node_->accept(this);
}

void ASTVisitor::visit(TupleTypeNode *node) {
    for (auto *type: node->type_nodes_) {
        if (type) type->accept(this);
    }
}

void ASTVisitor::visit(ArrayTypeNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->expression_node_) node->expression_node_->accept(this);
}

void ASTVisitor::visit(SliceTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void ASTVisitor::visit(InferredTypeNode *node) {
}
