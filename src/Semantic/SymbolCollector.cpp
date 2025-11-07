#include "Semantic/SymbolCollector.h"
#include "Semantic/ASTNode.h"
#include "Semantic/Type.h"
#include "Semantic/Symbol.h"


void SymbolCollector::visit(ASTNode *node) {
}

void SymbolCollector::visit(CrateNode *node) {
	node->scope_index = scope_manager_.current_scope->scope_index;
    for (const auto &item: node->items_) {
        if (item) item->accept(this);
    }
}

void SymbolCollector::visit(VisItemNode *node) {
}

void SymbolCollector::visit(FunctionNode *node) {
    std::shared_ptr<Type> type = std::make_shared<FunctionType>(
        std::vector<std::shared_ptr<Type> >{}, std::make_shared<PrimitiveType>("void"));
    Symbol symbol(node->pos_, node->identifier_, type, SymbolType::Function, false);
    scope_manager_.declare(symbol);
    if (node->block_expression_) {
        node->block_expression_->accept(this);
    }
}

void SymbolCollector::visit(StructNode *node) {
    std::shared_ptr<Type> type = std::make_shared<StructType>(
        node->identifier_,
        std::vector<StructMember>{}
    );
    Symbol symbol(node->pos_, node->identifier_, type, SymbolType::Struct, false);
    scope_manager_.declare(symbol);
}

void SymbolCollector::visit(EnumerationNode *node) {
    std::vector<std::string> variants;
    for (const auto &variant: node->enum_variant_nodes_) {
        if (variant) {
            variant->accept(this);
            variants.emplace_back(variant -> identifier_);
        }
    }
    std::shared_ptr<Type> type = std::make_shared<EnumerationType>(
        node->identifier_, variants);
    Symbol symbol(node->pos_, node -> identifier_, type, SymbolType::Enumeration, false);
    scope_manager_.declare(symbol);
}

void SymbolCollector::visit(ConstantItemNode *node) {
    std::string type_name;
    if (node->type_node_) {
        node->type_node_->accept(this);
        type_name = node->type_node_->toString();
    }
    if (node->expression_node_) node->expression_node_->accept(this);
    if (type_name == "i32" || type_name == "u32" ||
        type_name == "isize" || type_name == "usize") {
        auto type = scope_manager_.lookup(type_name).type_;
        Symbol symbol(node -> pos_, node -> identifier_, type, SymbolType::Variable, false);
        symbol.SetConst(true);
        scope_manager_.declare(symbol);
    }
}

void SymbolCollector::visit(TraitNode *node) {
}

void SymbolCollector::visit(ImplementationNode *node) {
}

void SymbolCollector::visit(AssociatedItemNode *node) {
    if (node->constant_item_node_) node->constant_item_node_->accept(this);
    if (node->function_node_) node->function_node_->accept(this);
}

void SymbolCollector::visit(InherentImplNode *node) {
    scope_manager_.AddScope();
    if (node->type_node_) node->type_node_->accept(this);\
    for (const auto &item: node->associated_item_nodes_) {
        if (item) item->accept(this);
    }
    scope_manager_.PopScope();
}

void SymbolCollector::visit(TraitImplNode *node) {
}

void SymbolCollector::visit(FunctionParametersNode *node) {
    for (const auto &param: node->function_params_) {
        if (param) param->accept(this);
    }
}

void SymbolCollector::visit(FunctionParamNode *node) {
    // if (node->pattern_no_top_alt_node_) node->pattern_no_top_alt_node_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void SymbolCollector::visit(FunctionParamPatternNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void SymbolCollector::visit(StructFieldNode *node) {
}

void SymbolCollector::visit(EnumVariantNode *node) {
    if (node->enum_variant_struct_node_) node->enum_variant_struct_node_->accept(this);
    if (node->enum_variant_discriminant_node_) node->enum_variant_discriminant_node_->accept(this);
}

void SymbolCollector::visit(EnumVariantStructNode *node) {
    for (const auto &field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void SymbolCollector::visit(EnumVariantDiscriminantNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolCollector::visit(TypeParamBoundsNode *node) {
}

void SymbolCollector::visit(TypeParamNode *node) {
}

void SymbolCollector::visit(ConstParamNode *node) {
}

void SymbolCollector::visit(StatementNode *node) {
}

void SymbolCollector::visit(StatementsNode *node) {
    for (const auto &stmt: node->statements_) {
        if (stmt) stmt->accept(this);
    }
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(EmptyStatementNode *node) {
}

void SymbolCollector::visit(LetStatementNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
    if (node->expression_) node->expression_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SymbolCollector::visit(VisItemStatementNode *node) {
    if (node->vis_item_node_) {
        node->vis_item_node_->accept(this);
    }
}

void SymbolCollector::visit(ExpressionStatementNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(ExpressionNode *node) {
}

void SymbolCollector::visit(ExpressionWithoutBlockNode *node) {
}

void SymbolCollector::visit(ExpressionWithBlockNode *node) {
}

void SymbolCollector::visit(ComparisonExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(TypeCastExpressionNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(AssignmentExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(ContinueExpressionNode *node) {
}

void SymbolCollector::visit(UnderscoreExpressionNode *node) {
}

void SymbolCollector::visit(JumpExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(LogicOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(LogicAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(BitwiseOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(BitwiseXorExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(BitwiseAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(ShiftExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(AddMinusExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(MulDivModExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(UnaryExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(FunctionCallExpressionNode *node) {
    if (node->callee_) node->callee_->accept(this);
    for (const auto &param: node->params_) {
        if (param) param->accept(this);
    }
}

void SymbolCollector::visit(ArrayIndexExpressionNode *node) {
    if (node->base_) node->base_->accept(this);
    if (node->index_) node->index_->accept(this);
}

void SymbolCollector::visit(MemberAccessExpressionNode *node) {
    if (node->base_) node->base_->accept(this);
}

void SymbolCollector::visit(BlockExpressionNode *node) {
    scope_manager_.AddScope();
    if (node->statements_) node->statements_->accept(this);
	node->scope_index = scope_manager_.current_scope->scope_index;
    scope_manager_.PopScope();
}

void SymbolCollector::visit(LoopExpressionNode *node) {
}

void SymbolCollector::visit(InfiniteLoopExpressionNode *node) {
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SymbolCollector::visit(PredicateLoopExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SymbolCollector::visit(IfExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->true_block_expression_) node->true_block_expression_->accept(this);
    if (node->false_block_expression_) node->false_block_expression_->accept(this);
    if (node->if_expression_) node->if_expression_->accept(this);
}

void SymbolCollector::visit(MatchExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
    if (node->match_arms_) node->match_arms_->accept(this);
}

void SymbolCollector::visit(LiteralExpressionNode *node) {
}

void SymbolCollector::visit(CharLiteralNode *node) {
}

void SymbolCollector::visit(StringLiteralNode *node) {
}

void SymbolCollector::visit(CStringLiteralNode *node) {
}

void SymbolCollector::visit(IntLiteralNode *node) {
}

void SymbolCollector::visit(BoolLiteralNode *node) {
}

void SymbolCollector::visit(ArrayLiteralNode *node) {
    for (const auto &expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolCollector::visit(PathExpressionNode *node) {
}

void SymbolCollector::visit(PathInExpressionNode *node) {
    for (const auto &seg: node->path_indent_segments_) {
        if (seg) seg->accept(this);
    }
}

void SymbolCollector::visit(QualifiedPathInExpressionNode *node) {
}

void SymbolCollector::visit(PathIndentSegmentNode *node) {
}

void SymbolCollector::visit(StructExpressionNode *node) {
    if (node->path_in_expression_node_) node->path_in_expression_node_->accept(this);
    if (node->struct_expr_fields_node_) node->struct_expr_fields_node_->accept(this);
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void SymbolCollector::visit(StructExprFieldsNode *node) {
    for (const auto &field: node->struct_expr_field_nodes_) {
        if (field) field->accept(this);
    }
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void SymbolCollector::visit(StructExprFieldNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolCollector::visit(StructBaseNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolCollector::visit(GroupedExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(TupleExpressionNode *node) {
    for (const auto &expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
}

void SymbolCollector::visit(ConditionsNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(LetChainNode *node) {
}

void SymbolCollector::visit(LetChainConditionNode *node) {
}


void SymbolCollector::visit(MatchArmsNode *node) {
    for (const auto &arm: node->match_arm_nodes_) {
        if (arm) arm->accept(this);
    }
    for (const auto &expr: node->expression_nodes_) {
        if (expr) expr->accept(this);
    }
}

void SymbolCollector::visit(MatchArmNode *node) {
    if (node->pattern_node_) node->pattern_node_->accept(this);
    if (node->match_arm_guard_) node->match_arm_guard_->accept(this);
}

void SymbolCollector::visit(MatchArmGuardNode *node) {
}

void SymbolCollector::visit(PatternNode *node) {
    for (const auto &pat: node->pattern_no_top_alts_) {
        if (pat) pat->accept(this);
    }
}

void SymbolCollector::visit(PatternNoTopAltNode *node) {
}

void SymbolCollector::visit(PatternWithoutRangeNode *node) {
}

void SymbolCollector::visit(LiteralPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(IdentifierPatternNode *node) {
    if (node->node_) node->node_->accept(this);
}

void SymbolCollector::visit(WildcardPatternNode *node) {
}

void SymbolCollector::visit(RestPatternNode *node) {
}

void SymbolCollector::visit(GroupedPatternNode *node) {
    if (node->pattern_) node->pattern_->accept(this);
}

void SymbolCollector::visit(SlicePatternNode *node) {
    for (const auto &pat: node->patterns_) {
        if (pat) pat->accept(this);
    }
}

void SymbolCollector::visit(PathPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolCollector::visit(TypeNode *node) {
}

void SymbolCollector::visit(TypeNoBoundsNode *node) {
}

void SymbolCollector::visit(ParenthesizedTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void SymbolCollector::visit(TypePathNode *node) {
    if (node->type_path_segment_node_) node->type_path_segment_node_->accept(this);
}

void SymbolCollector::visit(TypePathSegmentNode *node) {
    if (node->path_indent_segment_node_) node->path_indent_segment_node_->accept(this);
}

void SymbolCollector::visit(UnitTypeNode *node) {
}

void SymbolCollector::visit(ArrayTypeNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolCollector::visit(SliceTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void SymbolCollector::visit(ReferenceTypeNode *node) {
}
