#include "Semantic/SymbolManager.h"
#include "Semantic/ASTNode.h"
#include "Semantic/Type.h"
#include "Semantic/Symbol.h"


void SymbolManager::visit(ASTNode *node) {
}

void SymbolManager::visit(CrateNode *node) {
    scope_manager_.current_scope = scope_manager_.root;
    scope_manager_.current_scope->index = 0;
    for (const auto &item: node->items_) {
        if (item) item->accept(this);
        auto structItem = std::dynamic_pointer_cast<StructNode>(item);
        if (structItem) {
            std::vector<StructMember> members;
            for (const auto &field: structItem->struct_field_nodes_) {
                auto type_node = field -> type_node_;
                std::shared_ptr<Type> member_type = scope_manager_.lookupType(type_node);
                type_node->type = member_type;
                StructMember member{field->identifier_, member_type};
                members.emplace_back(member);
            }
            auto struct_ = std::make_shared<StructType>(structItem->identifier_, members);
            scope_manager_.ModifyType(structItem->identifier_, struct_);
            continue;
        }
        auto funcItem = std::dynamic_pointer_cast<FunctionNode>(item);
        if (funcItem) {
            std::vector<std::shared_ptr<Type> > params;
            std::shared_ptr<Type> ret = std::make_shared<PrimitiveType>("void");
            if (funcItem->function_parameters_) {
                for (const auto &param: funcItem->function_parameters_->function_params_) {
                    std::shared_ptr<Type> type = scope_manager_.lookupType(param->type_);
                    param->type_->type = type;
                    params.emplace_back(type);
                }
            }
            if (funcItem->type_) {
                ret = scope_manager_.lookupType(funcItem->type_);
                funcItem->type_->type = ret;
            }
            auto func_ = std::make_shared<FunctionType>(params, ret);
            scope_manager_.ModifyType(funcItem->identifier_, func_);
        }
    }
}

void SymbolManager::visit(VisItemNode *node) {
}

void SymbolManager::visit(FunctionNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->function_parameters_) node->function_parameters_->accept(this);
    if (node->block_expression_) {
        node->block_expression_->accept(this);
    }
}

void SymbolManager::visit(StructNode *node) {
}

void SymbolManager::visit(EnumerationNode *node) {
    for (const auto &variant: node->enum_variant_nodes_) {
        if (variant) variant->accept(this);
    }
}

void SymbolManager::visit(ConstantItemNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolManager::visit(TraitNode *node) {
}

void SymbolManager::visit(ImplementationNode *node) {
}

void SymbolManager::visit(AssociatedItemNode *node) {
    if (node->constant_item_node_) node->constant_item_node_->accept(this);
    if (node->function_node_) node->function_node_->accept(this);
}

void SymbolManager::visit(InherentImplNode *node) {
    scope_manager_.current_scope = scope_manager_.current_scope
            ->next_level_scopes_[scope_manager_.current_scope->index++];
    scope_manager_.current_scope->index = 0;
    if (node->type_node_) node->type_node_->accept(this);
    std::string name = node->type_node_->toString();
    Symbol symbol = scope_manager_.lookup(name);
    auto& name_set = symbol.type_->name_set;
    for (const auto &item: node->associated_item_nodes_) {
        if (item) item->accept(this);
        if (item->constant_item_node_) {
            auto constant_item = item->constant_item_node_;
            auto sym = scope_manager_.lookupType(constant_item->type_node_);
            Method method{constant_item->identifier_, sym};
            if (name_set.find(constant_item->identifier_) != name_set.end()) {
                throw SemanticError("Semantic Error: Duplicate Definition of " + constant_item->identifier_,
                    node->pos_);
            }
            name_set[constant_item->identifier_] = true;
            symbol.type_->constants_.emplace_back(method);
            scope_manager_.RemoteModifyType(name, symbol.type_);
        }
        if (item->function_node_) {
            auto funcItem = item->function_node_;
            bool have_and = false, is_mut = false, have_self = false;
            std::vector<std::shared_ptr<Type> > params;
            std::shared_ptr<Type> ret = std::make_shared<PrimitiveType>("void");
            if (funcItem->function_parameters_) {
                auto self_param = funcItem->function_parameters_->self_param_node_;
                if (auto short_hand_self = std::dynamic_pointer_cast<ShortHandSelfNode>(self_param)) {
                    have_self = true;
                    have_and = short_hand_self->have_and_;
                    is_mut = short_hand_self->is_mut_;
                }
                for (const auto &param: funcItem->function_parameters_->function_params_) {
                    auto type = scope_manager_.lookupType(param->type_);
                    params.emplace_back(type);
                }
            }
            if (funcItem->type_) {
                auto type = scope_manager_.lookupType(funcItem->type_);
                ret = type;
            }
            auto func_ = std::make_shared<FunctionType>(params, ret);
            func_->SetParam(have_self, have_and, is_mut);
            Method method{funcItem->identifier_, func_};
            if (name_set.find(funcItem->identifier_) != name_set.end()) {
                throw SemanticError("Semantic Error: Duplicate Definition of " + funcItem->identifier_,
                    node->pos_);
            }
            name_set[funcItem->identifier_] = true;
            if (have_self) {
                symbol.type_->methods_.emplace_back(method);
            } else {
                symbol.type_->inline_functions_.emplace_back(method);
            }
            scope_manager_.RemoteModifyType(name, symbol.type_);
        }
    }

    scope_manager_.PopScope();
}

void SymbolManager::visit(TraitImplNode *node) {
}

void SymbolManager::visit(FunctionParametersNode *node) {
    for (const auto &param: node->function_params_) {
        if (param) param->accept(this);
    }
}

void SymbolManager::visit(FunctionParamNode *node) {
    if (node->pattern_no_top_alt_node_) node->pattern_no_top_alt_node_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void SymbolManager::visit(FunctionParamPatternNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void SymbolManager::visit(StructFieldNode *node) {
}

void SymbolManager::visit(EnumVariantNode *node) {
    if (node->enum_variant_struct_node_) node->enum_variant_struct_node_->accept(this);
    if (node->enum_variant_discriminant_node_) node->enum_variant_discriminant_node_->accept(this);
}

void SymbolManager::visit(EnumVariantStructNode *node) {
    for (const auto &field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void SymbolManager::visit(EnumVariantDiscriminantNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolManager::visit(TypeParamBoundsNode *node) {
}

void SymbolManager::visit(TypeParamNode *node) {
}

void SymbolManager::visit(ConstParamNode *node) {
}

void SymbolManager::visit(StatementNode *node) {
}

void SymbolManager::visit(StatementsNode *node) {
    for (const auto &stmt: node->statements_) {
        if (stmt) stmt->accept(this);
    }
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(EmptyStatementNode *node) {
}

void SymbolManager::visit(LetStatementNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
    if (node->expression_) node->expression_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SymbolManager::visit(VisItemStatementNode *node) {
    if (node->vis_item_node_) {
        node->vis_item_node_->accept(this);
    }
}

void SymbolManager::visit(ExpressionStatementNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(ExpressionNode *node) {
}

void SymbolManager::visit(ExpressionWithoutBlockNode *node) {
}

void SymbolManager::visit(ExpressionWithBlockNode *node) {
}

void SymbolManager::visit(ComparisonExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(TypeCastExpressionNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(AssignmentExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(ContinueExpressionNode *node) {
}

void SymbolManager::visit(UnderscoreExpressionNode *node) {
}

void SymbolManager::visit(JumpExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(LogicOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(LogicAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(BitwiseOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(BitwiseXorExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(BitwiseAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(ShiftExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(AddMinusExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(MulDivModExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(UnaryExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(FunctionCallExpressionNode *node) {
    if (node->callee_) node->callee_->accept(this);
    for (const auto &param: node->params_) {
        if (param) param->accept(this);
    }
}

void SymbolManager::visit(ArrayIndexExpressionNode *node) {
    if (node->base_) node->base_->accept(this);
    if (node->index_) node->index_->accept(this);
}

void SymbolManager::visit(MemberAccessExpressionNode *node) {
    if (node->base_) node->base_->accept(this);
}

void SymbolManager::visit(BlockExpressionNode *node) {
    scope_manager_.current_scope = scope_manager_.current_scope
            ->next_level_scopes_[scope_manager_.current_scope->index++];
    scope_manager_.current_scope->index = 0;
    if (node->statements_) {
        for (const auto &item: node->statements_->statements_) {
            auto visItemStmt = std::dynamic_pointer_cast<VisItemStatementNode>(item);
            if (!visItemStmt || !visItemStmt->vis_item_node_) {
                continue;
            }

            auto structItem = std::dynamic_pointer_cast<StructNode>(visItemStmt -> vis_item_node_);
            if (structItem) {
                std::vector<StructMember> members;
                for (const auto &field: structItem->struct_field_nodes_) {
                    auto type_node = field -> type_node_;
                    std::shared_ptr<Type> member_type = scope_manager_.lookupType(type_node);
                    type_node->type = member_type;
                    StructMember member{field->identifier_, member_type};
                    members.emplace_back(member);
                }
                auto struct_ = std::make_shared<StructType>(structItem->identifier_, members);
                scope_manager_.ModifyType(structItem->identifier_, struct_);
                continue;
            }
            auto funcItem = std::dynamic_pointer_cast<FunctionNode>(visItemStmt -> vis_item_node_);
            if (funcItem) {
                std::vector<std::shared_ptr<Type> > params;
                std::shared_ptr<Type> ret = std::make_shared<PrimitiveType>("void");
                if (funcItem->function_parameters_) {
                    for (const auto &param: funcItem->function_parameters_->function_params_) {
                        std::shared_ptr<Type> type = scope_manager_.lookupType(param->type_);
                        param->type_->type = type;
                        params.emplace_back(type);
                    }
                }
                if (funcItem->type_) {
                    ret = scope_manager_.lookupType(funcItem->type_);
                    funcItem->type_->type = ret;
                }
                auto func_ = std::make_shared<FunctionType>(params, ret);
                scope_manager_.ModifyType(funcItem->identifier_, func_);
            }
        }
        node->statements_->accept(this);
    }
    scope_manager_.PopScope();
}

void SymbolManager::visit(LoopExpressionNode *node) {
}

void SymbolManager::visit(InfiniteLoopExpressionNode *node) {
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SymbolManager::visit(PredicateLoopExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->block_expression_) node->block_expression_->accept(this);
}

void SymbolManager::visit(IfExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->true_block_expression_) node->true_block_expression_->accept(this);
    if (node->false_block_expression_) node->false_block_expression_->accept(this);
    if (node->if_expression_) node->if_expression_->accept(this);
}

void SymbolManager::visit(MatchExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
    if (node->match_arms_) node->match_arms_->accept(this);
}

void SymbolManager::visit(LiteralExpressionNode *node) {
}

void SymbolManager::visit(CharLiteralNode *node) {
}

void SymbolManager::visit(StringLiteralNode *node) {
}

void SymbolManager::visit(CStringLiteralNode *node) {
}

void SymbolManager::visit(IntLiteralNode *node) {
}

void SymbolManager::visit(BoolLiteralNode *node) {
}

void SymbolManager::visit(ArrayLiteralNode *node) {
    for (const auto &expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void SymbolManager::visit(PathExpressionNode *node) {
}

void SymbolManager::visit(PathInExpressionNode *node) {
    for (const auto &seg: node->path_indent_segments_) {
        if (seg) seg->accept(this);
    }
}

void SymbolManager::visit(QualifiedPathInExpressionNode *node) {
}

void SymbolManager::visit(PathIndentSegmentNode *node) {
}

void SymbolManager::visit(StructExpressionNode *node) {
    if (node->path_in_expression_node_) node->path_in_expression_node_->accept(this);
    if (node->struct_expr_fields_node_) node->struct_expr_fields_node_->accept(this);
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void SymbolManager::visit(StructExprFieldsNode *node) {
    for (const auto &field: node->struct_expr_field_nodes_) {
        if (field) field->accept(this);
    }
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void SymbolManager::visit(StructExprFieldNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolManager::visit(StructBaseNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolManager::visit(GroupedExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(TupleExpressionNode *node) {
    for (const auto &expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
}

void SymbolManager::visit(ConditionsNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(LetChainNode *node) {
}

void SymbolManager::visit(LetChainConditionNode *node) {
}


void SymbolManager::visit(MatchArmsNode *node) {
    for (const auto &arm: node->match_arm_nodes_) {
        if (arm) arm->accept(this);
    }
    for (const auto &expr: node->expression_nodes_) {
        if (expr) expr->accept(this);
    }
}

void SymbolManager::visit(MatchArmNode *node) {
    if (node->pattern_node_) node->pattern_node_->accept(this);
    if (node->match_arm_guard_) node->match_arm_guard_->accept(this);
}

void SymbolManager::visit(MatchArmGuardNode *node) {
}

void SymbolManager::visit(PatternNode *node) {
    for (const auto &pat: node->pattern_no_top_alts_) {
        if (pat) pat->accept(this);
    }
}

void SymbolManager::visit(PatternNoTopAltNode *node) {
}

void SymbolManager::visit(PatternWithoutRangeNode *node) {
}

void SymbolManager::visit(LiteralPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(IdentifierPatternNode *node) {
    if (node->node_) node->node_->accept(this);
}

void SymbolManager::visit(WildcardPatternNode *node) {
}

void SymbolManager::visit(RestPatternNode *node) {
}

void SymbolManager::visit(GroupedPatternNode *node) {
    if (node->pattern_) node->pattern_->accept(this);
}

void SymbolManager::visit(SlicePatternNode *node) {
    for (const auto &pat: node->patterns_) {
        if (pat) pat->accept(this);
    }
}

void SymbolManager::visit(PathPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void SymbolManager::visit(TypeNode *node) {
}

void SymbolManager::visit(TypeNoBoundsNode *node) {
}

void SymbolManager::visit(ParenthesizedTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void SymbolManager::visit(TypePathNode *node) {
    if (node->type_path_segment_node_) node->type_path_segment_node_->accept(this);
}

void SymbolManager::visit(TypePathSegmentNode *node) {
    if (node->path_indent_segment_node_) node->path_indent_segment_node_->accept(this);
}

void SymbolManager::visit(UnitTypeNode *node) {
}

void SymbolManager::visit(ArrayTypeNode *node) {
    if (node->type_) node->type_->accept(this);
    if (node->expression_node_) node->expression_node_->accept(this);
}

void SymbolManager::visit(SliceTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void SymbolManager::visit(ReferenceTypeNode *node) {
    if (node->type_node_) {
        node->type_node_->accept(this);
        node->type = std::make_shared<ReferenceType>(node->type_node_->type, node->is_mut_);
    }
}
