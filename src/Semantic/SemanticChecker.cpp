#include "Semantic/SemanticChecker.h"
#include "Semantic/ASTNode.h"
#include "Semantic/SymbolCollector.h"

extern SymbolCollector* symbol_collector;

void SemanticChecker::visit(ASTNode *node) {
}

void SemanticChecker::visit(CrateNode *node) {
    for (auto* item: node -> items_) {
        auto* structItem = dynamic_cast<StructNode*> (item);
        if (structItem) {
            structItem -> accept(symbol_collector);
        }
        auto* functionItem = dynamic_cast<FunctionNode*> (item);
        if (functionItem) {
            functionItem -> accept(symbol_collector);
        }
    }

    for (auto* item: node -> items_) {
        auto* structItem = dynamic_cast<StructNode*> (item);
        if (structItem) {
            std::vector<StructMember> members;
            for (auto* field: structItem -> struct_field_nodes_) {
                Symbol symbol = scope_manager_.lookup(field -> type_node_ -> toString());
                StructMember member{field -> identifier_, symbol.type_};
                members.emplace_back(member);
            }
            auto struct_ = std::make_shared<StructType>(structItem -> identifier_, members);
            scope_manager_.ModifyType(structItem -> identifier_, struct_);
            continue;
        }
        auto* funcItem = dynamic_cast<FunctionNode*> (item);
        if (funcItem) {
            std::vector<std::shared_ptr<Type>> params;
            std::shared_ptr<Type> ret = std::make_shared<PrimitiveType>("void");
            if (funcItem -> function_parameters_) {
                for (auto* param: funcItem -> function_parameters_ -> function_params_) {
                    Symbol symbol = scope_manager_.lookup(param -> type_ -> toString());
                    params.emplace_back(symbol.type_);
                }
            }
            if (funcItem -> type_) {
                Symbol symbol = scope_manager_.lookup(funcItem -> type_ -> toString());
                ret = symbol.type_;
            }
            auto func_ = std::make_shared<FunctionType>(params, ret);
            scope_manager_.ModifyType(funcItem -> identifier_, func_);
        }
    }


    for (auto *item: node->items_) {
        if (item) item->accept(this);
    }
}

void SemanticChecker::visit(VisItemNode *node) {
}

void SemanticChecker::visit(ModuleNode *node) {
}

void SemanticChecker::visit(FunctionNode *node) {
    if (node->function_parameters_) node->function_parameters_->accept(this);
    if (node->type_) node->type_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SemanticChecker::visit(StructNode *node) {
    for (auto *field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void SemanticChecker::visit(EnumerationNode *node) {
    for (auto *variant: node->enum_variant_nodes_) {
        if (variant) variant->accept(this);
    }
}

void SemanticChecker::visit(ConstantItemNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SemanticChecker::visit(TraitNode *node) {
}

void SemanticChecker::visit(ImplementationNode *node) {
}

void SemanticChecker::visit(AssociatedItemNode *node) {
    if (node->constant_item_node_) node->constant_item_node_->accept(this);
    if (node->function_node_) node->function_node_->accept(this);
}

void SemanticChecker::visit(InherentImplNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
    for (auto *item: node->associated_item_nodes_) {
        if (item) item->accept(this);
    }
}

void SemanticChecker::visit(TraitImplNode *node) {
}

void SemanticChecker::visit(FunctionParametersNode *node) {
    for (auto *param: node->function_params_) {
        if (param) param->accept(this);
    }
}

void SemanticChecker::visit(FunctionParamNode *node) {
    if (node->pattern_no_top_alt_node_) node->pattern_no_top_alt_node_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void SemanticChecker::visit(FunctionParamPatternNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void SemanticChecker::visit(StructFieldNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
}

void SemanticChecker::visit(EnumVariantNode *node) {
    if (node->enum_variant_struct_node_) node->enum_variant_struct_node_->accept(this);
    if (node->enum_variant_discriminant_node_) node->enum_variant_discriminant_node_->accept(this);
}

void SemanticChecker::visit(EnumVariantStructNode *node) {
    for (auto *field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void SemanticChecker::visit(EnumVariantDiscriminantNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SemanticChecker::visit(TypeParamBoundsNode *node) {
}

void SemanticChecker::visit(TypeParamNode *node) {
}

void SemanticChecker::visit(ConstParamNode *node) {
}

void SemanticChecker::visit(StatementNode *node) {
}

void SemanticChecker::visit(StatementsNode *node) {
    for (auto *stmt: node->statements_) {
        if (stmt) stmt->accept(this);
    }
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(EmptyStatementNode *node) {
}

void SemanticChecker::visit(LetStatementNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
    if (node->expression_) node->expression_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SemanticChecker::visit(ExpressionStatementNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(ExpressionNode *node) {
}

void SemanticChecker::visit(ExpressionWithoutBlockNode *node) {
}

void SemanticChecker::visit(ExpressionWithBlockNode *node) {
}

void SemanticChecker::visit(ComparisonExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(TypeCastExpressionNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(AssignmentExpressionNode *node) {
    if (node->lhs_) {
        node->lhs_->accept(this);
        if (!node -> lhs_ -> is_assignable_) {
            throw SemanticError("Semantic Error: Left Value Error: ", node -> pos_);
        }
    }
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(ContinueExpressionNode *node) {
}

void SemanticChecker::visit(UnderscoreExpressionNode *node) {
}

void SemanticChecker::visit(JumpExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(LogicOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(LogicAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(BitwiseOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(BitwiseXorExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(BitwiseAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(ShiftExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(AddMinusExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(MulDivModExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(UnaryExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(FunctionCallExpressionNode *node) {
    if (node->callee_) node->callee_->accept(this);
    for (auto *param: node->params_) {
        if (param) param->accept(this);
    }
}

void SemanticChecker::visit(ArrayIndexExpressionNode *node) {
    if (node->base_) node->base_->accept(this);
    if (node->index_) node->index_->accept(this);
}

void SemanticChecker::visit(MemberAccessExpressionNode *node) {
    if (node->base_) node->base_->accept(this);
}

void SemanticChecker::visit(BlockExpressionNode *node) {
    scope_manager_.pushBack();
    for (auto* item: node -> statements_ -> statements_) {
        auto* structItem = dynamic_cast<StructNode*> (item);
        if (structItem) {
            structItem -> accept(symbol_collector);
        }
        auto* functionItem = dynamic_cast<StructNode*> (item);
        if (functionItem) {
            functionItem -> accept(symbol_collector);
        }
    }
    for (auto* item: node -> statements_ -> statements_) {
        auto* structItem = dynamic_cast<StructNode*> (item);
        if (structItem) {
            std::vector<StructMember> members;
            for (auto* field: structItem -> struct_field_nodes_) {
                Symbol symbol = scope_manager_.lookup(field -> type_node_ -> toString());
                StructMember member{field -> identifier_, symbol.type_};
                members.emplace_back(member);
            }
            auto struct_ = std::make_shared<StructType>(structItem -> identifier_, members);
            scope_manager_.ModifyType(structItem -> identifier_, struct_);
            continue;
        }
        auto* funcItem = dynamic_cast<FunctionNode*> (item);
        if (funcItem) {
            std::vector<std::shared_ptr<Type>> params;
            std::shared_ptr<Type> ret = std::make_shared<PrimitiveType>("void");
            if (funcItem -> function_parameters_) {
                for (auto* param: funcItem -> function_parameters_ -> function_params_) {
                    Symbol symbol = scope_manager_.lookup(param -> type_ -> toString());
                    params.emplace_back(symbol.type_);
                }
            }
            if (funcItem -> type_) {
                Symbol symbol = scope_manager_.lookup(funcItem -> type_ -> toString());
                ret = symbol.type_;
            }
            auto func_ = std::make_shared<FunctionType>(params, ret);
            scope_manager_.ModifyType(funcItem -> identifier_, func_);
        }
    }
    if (node->statements_) node->statements_->accept(this);
    scope_manager_.popBack();
}

void SemanticChecker::visit(LoopExpressionNode *node) {
}

void SemanticChecker::visit(InfiniteLoopExpressionNode *node) {
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SemanticChecker::visit(PredicateLoopExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SemanticChecker::visit(IfExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->true_block_expression_) node->true_block_expression_->accept(this);
    if (node->false_block_expression_) node->false_block_expression_->accept(this);
    if (node->if_expression_) node->if_expression_->accept(this);
}

void SemanticChecker::visit(MatchExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
    if (node->match_arms_) node->match_arms_->accept(this);
}

void SemanticChecker::visit(LiteralExpressionNode *node) {
}

void SemanticChecker::visit(CharLiteralNode *node) {
}

void SemanticChecker::visit(StringLiteralNode *node) {
}

void SemanticChecker::visit(CStringLiteralNode *node) {
}

void SemanticChecker::visit(IntLiteralNode *node) {
}

void SemanticChecker::visit(BoolLiteralNode *node) {
}

void SemanticChecker::visit(ArrayLiteralNode *node) {
    for (auto *expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SemanticChecker::visit(PathExpressionNode *node) {
}

void SemanticChecker::visit(PathInExpressionNode *node) {
    for (auto *seg: node->path_indent_segments_) {
        if (seg) seg->accept(this);
    }
}

void SemanticChecker::visit(QualifiedPathInExpressionNode *node) {
}

void SemanticChecker::visit(PathIndentSegmentNode *node) {
}

void SemanticChecker::visit(StructExpressionNode *node) {
    if (node->path_in_expression_node_) node->path_in_expression_node_->accept(this);
    if (node->struct_expr_fields_node_) node->struct_expr_fields_node_->accept(this);
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void SemanticChecker::visit(StructExprFieldsNode *node) {
    for (auto *field: node->struct_expr_field_nodes_) {
        if (field) field->accept(this);
    }
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void SemanticChecker::visit(StructExprFieldNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SemanticChecker::visit(StructBaseNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SemanticChecker::visit(GroupedExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(TupleExpressionNode *node) {
    for (auto *expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
}

void SemanticChecker::visit(ConditionsNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(LetChainNode *node) {
}

void SemanticChecker::visit(MatchArmsNode *node) {
    for (auto *arm: node->match_arm_nodes_) {
        if (arm) arm->accept(this);
    }
    for (auto *expr: node->expression_nodes_) {
        if (expr) expr->accept(this);
    }
}

void SemanticChecker::visit(MatchArmNode *node) {
    if (node->pattern_node_) node->pattern_node_->accept(this);
    if (node->match_arm_guard_) node->match_arm_guard_->accept(this);
}

void SemanticChecker::visit(MatchArmGuardNode *node) {
}

void SemanticChecker::visit(PatternNode *node) {
    for (auto *pat: node->pattern_no_top_alts_) {
        if (pat) pat->accept(this);
    }
}

void SemanticChecker::visit(PatternNoTopAltNode *node) {
}

void SemanticChecker::visit(PatternWithoutRangeNode *node) {
}

void SemanticChecker::visit(LiteralPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(IdentifierPatternNode *node) {
    if (node->node_) node->node_->accept(this);
}

void SemanticChecker::visit(WildcardPatternNode *node) {
}

void SemanticChecker::visit(RestPatternNode *node) {
}

void SemanticChecker::visit(GroupedPatternNode *node) {
    if (node->pattern_) node->pattern_->accept(this);
}

void SemanticChecker::visit(SlicePatternNode *node) {
    for (auto *pat: node->patterns_) {
        if (pat) pat->accept(this);
    }
}

void SemanticChecker::visit(PathPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(TypeNode *node) {
}

void SemanticChecker::visit(TypeNoBoundsNode *node) {
}

void SemanticChecker::visit(ParenthesizedTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void SemanticChecker::visit(TypePathNode *node) {
    for (auto *seg: node->type_path_segment_nodes_) {
        if (seg) seg->accept(this);
    }
}

void SemanticChecker::visit(TypePathSegmentNode *node) {
    if (node->path_indent_segment_node_) node->path_indent_segment_node_->accept(this);
}

void SemanticChecker::visit(TupleTypeNode *node) {
    for (auto *type: node->type_nodes_) {
        if (type) type->accept(this);
    }
}

void SemanticChecker::visit(ArrayTypeNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SemanticChecker::visit(SliceTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void SemanticChecker::visit(InferredTypeNode *node) {
}
