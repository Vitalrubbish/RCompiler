#include "IR/IRBuilder.h"
#include "Semantic/ASTNode.h"
#include "Semantic/SymbolCollector.h"
#include "Semantic/Type.h"
#include "Semantic/Symbol.h"

extern SymbolCollector *symbol_collector;

void IRBuilder::visit(ASTNode *node) {
}

void IRBuilder::visit(CrateNode *node) {
    scope_manager_.current_scope = scope_manager_.root;
    scope_manager_.current_scope -> index = 0;
    for (const auto& item: node->items_) {
        auto const_item = std::dynamic_pointer_cast<ConstantItemNode>(item);
        if (const_item) {
            item->accept(this);
        }
    }
    for (const auto &item: node->items_) {
        if (item) item->accept(this);
    }
}

void IRBuilder::visit(VisItemNode *node) {
}

void IRBuilder::visit(FunctionNode *node) {
    if (node->function_parameters_) node->function_parameters_->accept(this);
    if (node->type_) node->type_->accept(this);
    if (node->block_expression_) {
        node->block_expression_->accept(this);
    }
}

void IRBuilder::visit(StructNode *node) {
    for (const auto &field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void IRBuilder::visit(EnumerationNode *node) {
    for (const auto &variant: node->enum_variant_nodes_) {
        if (variant) variant->accept(this);
    }
}

void IRBuilder::visit(ConstantItemNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
    if (node->expression_node_) node->expression_node_->accept(this);
    if (!node->expression_node_->is_compiler_known_) {
        throw SemanticError("Semantic Error: The RHS is not a Compiler-known expression", node->pos_);
    }
    scope_manager_.AddConstant(node->identifier_, node->expression_node_->value);
}

void IRBuilder::visit(TraitNode *node) {
}

void IRBuilder::visit(ImplementationNode *node) {
}

void IRBuilder::visit(AssociatedItemNode *node) {
    if (node->constant_item_node_) node->constant_item_node_->accept(this);
    if (node->function_node_) node->function_node_->accept(this);
}

void IRBuilder::visit(InherentImplNode *node) {
    scope_manager_.current_scope = scope_manager_.current_scope
            ->next_level_scopes_[scope_manager_.current_scope->index++];
    scope_manager_.current_scope->index = 0;
    if (node->type_node_) node->type_node_->accept(this);
    std::string name = node->type_node_->toString();
    Symbol symbol = scope_manager_.lookup(name);
    Symbol self_symbol(node->pos_, "Self", symbol.type_, SymbolType::Struct, false);
    scope_manager_.declare(self_symbol);
    auto& name_set = symbol.type_->name_set;
    for (auto& item : node->associated_item_nodes_) {
        if (item) {
            item->accept(this);
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
        }
    }
    scope_manager_.PopScope();
}

void IRBuilder::visit(TraitImplNode *node) {
}

void IRBuilder::visit(FunctionParametersNode *node) {
    for (const auto &param: node->function_params_) {
        if (param) param->accept(this);
    }
}

void IRBuilder::visit(FunctionParamNode *node) {
    // if (node->pattern_no_top_alt_node_) node->pattern_no_top_alt_node_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void IRBuilder::visit(FunctionParamPatternNode *node) {
    if (node->pattern_no_top_alt_) node->pattern_no_top_alt_->accept(this);
    if (node->type_) node->type_->accept(this);
}

void IRBuilder::visit(StructFieldNode *node) {
    if (node->type_node_) node->type_node_->accept(this);
}

void IRBuilder::visit(EnumVariantNode *node) {
    if (node->enum_variant_struct_node_) node->enum_variant_struct_node_->accept(this);
    if (node->enum_variant_discriminant_node_) node->enum_variant_discriminant_node_->accept(this);
}

void IRBuilder::visit(EnumVariantStructNode *node) {
    for (const auto &field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void IRBuilder::visit(EnumVariantDiscriminantNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}


void IRBuilder::visit(StatementNode *node) {
}

void IRBuilder::visit(StatementsNode *node) {
    for (const auto &stmt: node->statements_) {
        if (stmt) stmt->accept(this);
    }
    if (node->expression_) node->expression_->accept(this);
}

void IRBuilder::visit(EmptyStatementNode *node) {
}

void IRBuilder::visit(LetStatementNode *node) {
    if (node->type_) {
        node->type_->accept(this);
    }
    if (node->expression_) {
        node->expression_->accept(this);
    }
    if (node->block_expression_) {
        node->block_expression_->accept(this);
    }
}

void IRBuilder::visit(VisItemStatementNode *node) {
    if (node -> vis_item_node_) {
        node -> vis_item_node_ -> accept(this);
    }
}

void IRBuilder::visit(ExpressionStatementNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void IRBuilder::visit(ExpressionNode *node) {
}

void IRBuilder::visit(ExpressionWithoutBlockNode *node) {
}

void IRBuilder::visit(ExpressionWithBlockNode *node) {
}

void IRBuilder::visit(ComparisonExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void IRBuilder::visit(TypeCastExpressionNode *node) {
    if (node->type_) {
        node->type_->accept(this);
    }
    if (node->expression_) node->expression_->accept(this);
    node->is_compiler_known_ = node->expression_->is_compiler_known_;
    auto* val = std::get_if<int64_t>(&node -> expression_ -> value);
    if (val) {
        node->value = *val;
    }
}

void IRBuilder::visit(AssignmentExpressionNode *node) {
    if (node->lhs_) {
        node->lhs_->accept(this);
    }
    if (node->rhs_) node->rhs_->accept(this);
}

void IRBuilder::visit(ContinueExpressionNode *node) {
}

void IRBuilder::visit(UnderscoreExpressionNode *node) {
}

void IRBuilder::visit(JumpExpressionNode *node) {
    if (node->expression_) {
        node->expression_->accept(this);
    }
}

void IRBuilder::visit(LogicOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void IRBuilder::visit(LogicAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void IRBuilder::visit(BitwiseOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void IRBuilder::visit(BitwiseXorExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
}

void IRBuilder::visit(BitwiseAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
}

void IRBuilder::visit(ShiftExpressionNode *node) {
    if (node->lhs_) { node->lhs_->accept(this); }
    if (node->rhs_) { node->rhs_->accept(this); }
    if (node -> lhs_ ->is_compiler_known_ && node -> rhs_ -> is_compiler_known_) {
        node -> is_compiler_known_ = true;
        auto* l = std::get_if<int64_t>(&node -> lhs_ -> value);
        auto* r = std::get_if<int64_t>(&node -> rhs_ -> value);
        if (l && r) {
            if (node -> type_ == TokenType::SL) {
                node -> value = (*l) << (*r);
            } else {
                node -> value = (*l) >> (*r);
            }
        }
    }
}

void IRBuilder::visit(AddMinusExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node -> lhs_ && node -> rhs_) {
        if (node -> lhs_ ->is_compiler_known_ && node -> rhs_ -> is_compiler_known_) {
            node -> is_compiler_known_ = true;
            auto* l = std::get_if<int64_t>(&node -> lhs_ -> value);
            auto* r = std::get_if<int64_t>(&node -> rhs_ -> value);
            if (l && r) {
                if (node -> type_ == TokenType::Plus) {
                    node -> value = *l + *r;
                } else {
                    node -> value = *l - *r;
                }
            }
        }
    }
}

void IRBuilder::visit(MulDivModExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node -> lhs_ && node -> rhs_) {
        if (node -> lhs_ ->is_compiler_known_ && node -> rhs_ -> is_compiler_known_) {
            node -> is_compiler_known_ = true;
            auto* l = std::get_if<int64_t>(&node -> lhs_ -> value);
            auto* r = std::get_if<int64_t>(&node -> rhs_ -> value);
            if (l && r) {
                if (node -> type_ == TokenType::Mul) {
                    node -> value = (*l) * (*r);
                } else if (node -> type_ == TokenType::Div) {
                    node -> value = (*l) / (*r);
                } else {
                    node -> value = (*l) % (*r);
                }
            }
        }
    }
}

void IRBuilder::visit(UnaryExpressionNode *node) {
    if (node->expression_) {
        node->expression_->accept(this);
        if (node->type_ == TokenType::Minus) {
            node->is_compiler_known_ = node->expression_->is_compiler_known_;
            auto* val = std::get_if<int64_t>(&node -> expression_ -> value);
            node->value = -*val;
        }
    }
}

void IRBuilder::visit(FunctionCallExpressionNode *node) {
    if (node->callee_) {
        node->callee_->accept(this);
    }
    for (const auto &param: node->params_) {
        if (param) {
            param->accept(this);
        }
    }
}

void IRBuilder::visit(ArrayIndexExpressionNode *node) {
    std::shared_ptr<IRArrayType> type;
    if (node->base_) {
        node->base_->accept(this);
    }
    if (node->index_) {
        node->index_->accept(this);
    }
}

void IRBuilder::visit(MemberAccessExpressionNode *node) {
    if (node->base_) {
        node->base_->accept(this);
    }
}

void IRBuilder::visit(BlockExpressionNode *node) {
    scope_manager_.current_scope = scope_manager_.current_scope
        ->next_level_scopes_[scope_manager_.current_scope->index++];
    scope_manager_.current_scope -> index = 0;
    if (node->statements_) {
        node->statements_->accept(this);
    }
    scope_manager_.PopScope();
}

void IRBuilder::visit(LoopExpressionNode *node) {
}

void IRBuilder::visit(InfiniteLoopExpressionNode *node) {
    if (node->block_expression_) {
        node->block_expression_->accept(this);
    }
}

void IRBuilder::visit(PredicateLoopExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->block_expression_) {
        node->block_expression_->accept(this);
    }
}

void IRBuilder::visit(IfExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->true_block_expression_) node->true_block_expression_->accept(this);
    if (node->false_block_expression_) node->false_block_expression_->accept(this);
    if (node->if_expression_) node->if_expression_->accept(this);
}

void IRBuilder::visit(MatchExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
    if (node->match_arms_) node->match_arms_->accept(this);
}

void IRBuilder::visit(LiteralExpressionNode *node) {
}

void IRBuilder::visit(CharLiteralNode *node) {
    node -> is_compiler_known_ = true;
}

void IRBuilder::visit(StringLiteralNode *node) {
    node->is_compiler_known_ = true;
    node->value = node->string_literal_;
}

void IRBuilder::visit(CStringLiteralNode *node) {
    node->is_compiler_known_ = true;
    node->value = node->c_string_literal_;
}

void IRBuilder::visit(IntLiteralNode *node) {
    node->is_compiler_known_ = true;
    node->value = node->int_literal_;
}

void IRBuilder::visit(BoolLiteralNode *node) {
    node -> is_compiler_known_ = true;
}

void IRBuilder::visit(ArrayLiteralNode *node) {
    node -> is_compiler_known_ = true;
    for (const auto &expr: node->expressions_) {
        if (expr) {
            expr->accept(this);
        }
    }
}

void IRBuilder::visit(PathExpressionNode *node) {
}

void IRBuilder::visit(PathInExpressionNode *node) {
    for (const auto &seg: node->path_indent_segments_) {
        if (seg) seg->accept(this);
    }
    uint32_t len = node -> path_indent_segments_.size();
    if (len == 2) {
        std::string type_name = node -> path_indent_segments_[0]->identifier_;
        std::string func_name = node -> path_indent_segments_[1]->identifier_;
        Symbol symbol = scope_manager_.lookup(type_name);
        for (auto& it: symbol.type_ -> constants_) {
            if (it.name_ == func_name) {
                node -> is_compiler_known_ = true;
                auto value = symbol.type_->value_map[it.name_];
                if (auto* tmp = std::get_if<int64_t>(&value)) {
                    node -> value = *tmp;
                }
                return;
            }
        }
    }
    if (len == 1) {
        try {
            Symbol symbol = scope_manager_.lookup(node -> path_indent_segments_[0]->identifier_);
            if (symbol.is_const_) {
                node -> is_compiler_known_ = true;
                auto value = scope_manager_.SearchValue(symbol.name_);
                if (auto* tmp = std::get_if<int64_t>(&value)) {
                    node -> value = *tmp;
                }
            }
        } catch (std::exception&) {}
    }
}

void IRBuilder::visit(PathIndentSegmentNode *node) {
}

void IRBuilder::visit(StructExpressionNode *node) {
    if (node->path_in_expression_node_) {
        node->path_in_expression_node_->accept(this);
    }
    if (node->struct_expr_fields_node_) node->struct_expr_fields_node_->accept(this);
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void IRBuilder::visit(StructExprFieldsNode *node) {
    for (const auto &field: node->struct_expr_field_nodes_) {
        if (field) field->accept(this);
    }
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void IRBuilder::visit(StructExprFieldNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void IRBuilder::visit(StructBaseNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}

void IRBuilder::visit(GroupedExpressionNode *node) {
    if (node->expression_) {
        node->expression_->accept(this);
        node -> types = node -> expression_ -> types;
        node->is_compiler_known_ = node->expression_->is_compiler_known_;
    }
}

void IRBuilder::visit(TupleExpressionNode *node) {
    for (const auto &expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
}

void IRBuilder::visit(ConditionsNode *node) {
    if (node->expression_) {
        node->expression_->accept(this);
    }
}

void IRBuilder::visit(LetChainNode *node) {
}

void IRBuilder::visit(LetChainConditionNode *node) {
}


void IRBuilder::visit(MatchArmsNode *node) {
    for (const auto &arm: node->match_arm_nodes_) {
        if (arm) arm->accept(this);
    }
    for (const auto &expr: node->expression_nodes_) {
        if (expr) expr->accept(this);
    }
}

void IRBuilder::visit(MatchArmNode *node) {
    if (node->pattern_node_) node->pattern_node_->accept(this);
    if (node->match_arm_guard_) node->match_arm_guard_->accept(this);
}

void IRBuilder::visit(MatchArmGuardNode *node) {
}

void IRBuilder::visit(PatternNode *node) {
    for (const auto &pat: node->pattern_no_top_alts_) {
        if (pat) pat->accept(this);
    }
}

void IRBuilder::visit(PatternNoTopAltNode *node) {
}

void IRBuilder::visit(PatternWithoutRangeNode *node) {
}

void IRBuilder::visit(LiteralPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void IRBuilder::visit(IdentifierPatternNode *node) {
    if (node->node_) node->node_->accept(this);
}

void IRBuilder::visit(WildcardPatternNode *node) {
}

void IRBuilder::visit(RestPatternNode *node) {
}

void IRBuilder::visit(GroupedPatternNode *node) {
    if (node->pattern_) node->pattern_->accept(this);
}

void IRBuilder::visit(SlicePatternNode *node) {
    for (const auto &pat: node->patterns_) {
        if (pat) pat->accept(this);
    }
}

void IRBuilder::visit(PathPatternNode *node) {
    if (node->expression_) node->expression_->accept(this);
}

void IRBuilder::visit(TypeNode *node) {
}

void IRBuilder::visit(TypeNoBoundsNode *node) {
}

void IRBuilder::visit(ParenthesizedTypeNode *node) {
    if (node->type_) node->type_->accept(this);
}

void IRBuilder::visit(TypePathNode *node) {
    if (node->type_path_segment_node_) {
        node->type_path_segment_node_->accept(this);
    }
}

void IRBuilder::visit(TypePathSegmentNode *node) {
    if (node->path_indent_segment_node_) node->path_indent_segment_node_->accept(this);
}

void IRBuilder::visit(UnitTypeNode *node) {
}

void IRBuilder::visit(ArrayTypeNode *node) {
    std::shared_ptr<Type> base_type;
    uint32_t size = 0;
    if (node->type_) {
        node->type_->accept(this);
        base_type = node -> type_ -> type;
    }
    if (node->expression_node_) {
        node->expression_node_->accept(this);
    }
    node -> type = std::make_shared<IRArrayType>(base_type, size);
}

void IRBuilder::visit(SliceTypeNode *node) {
    std::shared_ptr<Type> base_type;
    if (node->type_) {
        node->type_->accept(this);
    }
}

void IRBuilder::visit(ReferenceTypeNode *node) {
    if (node->type_node_) {
        node->type_node_->accept(this);
    }
    node -> type = std::make_shared<ReferenceType>(node->type_node_->type, node->is_mut_);
}


void IRBuilder::visit(ConstParamNode *node) {
}

void IRBuilder::visit(TypeParamNode *node) {
}

void IRBuilder::visit(TypeParamBoundsNode *node) {
}

void IRBuilder::visit(QualifiedPathInExpressionNode *node) {
}