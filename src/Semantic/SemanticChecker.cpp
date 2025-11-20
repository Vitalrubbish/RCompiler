#include "Semantic/SemanticChecker.h"

#include "Semantic/ASTNode.h"
#include "Semantic/Type.h"
#include "Semantic/Symbol.h"

void SemanticChecker::visit(ASTNode *node) {
}

void SemanticChecker::visit(CrateNode *node) {
    scope_manager_.current_scope = scope_manager_.root;
    scope_manager_.current_scope->index = 0;
    for (const auto &item: node->items_) {
        if (item) item->accept(this);
    }
}

void SemanticChecker::visit(VisItemNode *node) {
}

void SemanticChecker::visit(FunctionNode *node) {
    if (node->function_parameters_) node->function_parameters_->accept(this);
    std::shared_ptr<Type> ret;
    if (node->type_) {
        node->type_->accept(this);
        ret = node->type_->type;
    } else {
        ret = scope_manager_.lookup("void").type_;
    }
    if (node->function_parameters_) {
        scope_manager_.current_scope = scope_manager_.current_scope
                ->next_level_scopes_[scope_manager_.current_scope->index];
        for (const auto &it: node->function_parameters_->function_params_) {
            it->accept(this);
            auto pattern_node = std::dynamic_pointer_cast<IdentifierPatternNode>(it->pattern_no_top_alt_node_);
            if (pattern_node) {
                std::string identifier = pattern_node->identifier_;
                bool is_mut = pattern_node->is_mut_;
                std::shared_ptr<Type> type = it->type_->type;

                auto ref_type = std::dynamic_pointer_cast<ReferenceTypeNode>(it->type_);
                if (ref_type && ref_type->is_mut_) { is_mut = true; }

                Symbol symbol(node->pos_, identifier, type, SymbolType::Variable, is_mut);
                scope_manager_.declare(symbol);
                continue;
            }
            if (auto path_pattern = std::dynamic_pointer_cast<PathPatternNode>(it->pattern_no_top_alt_node_)) {
                bool is_mut;
                std::string identifier;
                if (auto expression = std::dynamic_pointer_cast<PathInExpressionNode>(path_pattern->expression_)) {
                    uint32_t len = expression->path_indent_segments_.size();
                    if (len == 0) {
                        throw SemanticError("Semantic Error: Path Pattern Error", node->pos_);
                    }
                    identifier = expression->path_indent_segments_[len - 1]->identifier_;
                    is_mut = false;
                }
                std::shared_ptr<Type> type = it->type_->type;
                auto ref_type = std::dynamic_pointer_cast<ReferenceTypeNode>(it->type_);
                if (ref_type && ref_type->is_mut_) { is_mut = true; }
                Symbol symbol(node->pos_, identifier, type, SymbolType::Variable, is_mut);
                scope_manager_.declare(symbol);
            }
        }
        scope_manager_.PopScope();
    }
    if (node->block_expression_) {
        node->block_expression_->accept(this);
        if (node->block_expression_) {
            if (function_return_type_.empty()) {
                function_return_type_ = node->block_expression_->types;
            } else {
                function_return_type_ = cap(function_return_type_, node->block_expression_->types);
                if (function_return_type_.empty()) {
                    throw SemanticError("Semantic Error: Return Type is not consistent in function",
                                        node->pos_);
                }
            }
        }
        if (function_return_type_.empty()) {
            function_return_type_.emplace_back(scope_manager_.lookup("void").type_);
        }
        auto check = cap(function_return_type_, std::vector{ret});
        if (check.empty()) {
            throw SemanticError("Semantic Error: Function Return Type not match", node->pos_);
        }
        function_return_type_.clear();
    }
    if (node->identifier_ == "main") {
        if (node->block_expression_) {
            auto statements = node->block_expression_->statements_;
            uint32_t len = statements->statements_.size();
            bool valid = false;
            if (len > 0) {
                auto last = statements->statements_[len - 1];
                auto exprStmt = std::dynamic_pointer_cast<ExpressionStatementNode>(last);
                if (exprStmt) {
                    auto expr = exprStmt->expression_;
                    auto func_call = std::dynamic_pointer_cast<FunctionCallExpressionNode>(expr);
                    auto id = std::dynamic_pointer_cast<PathInExpressionNode>(func_call->callee_);
                    if (id && id->path_indent_segments_[0]->identifier_ == "exit") {
                        valid = true;
                    }
                } else if (statements->expression_) {
                    auto func_call = std::dynamic_pointer_cast<FunctionCallExpressionNode>(statements->expression_);
                    if (func_call) {
                        auto id = std::dynamic_pointer_cast<PathInExpressionNode>(func_call->callee_);
                        if (id && id->path_indent_segments_[0]->identifier_ == "exit") {
                            valid = true;
                        }
                    }
                }
            }
            if (!valid) {
                throw SemanticError("Semantic Error: There is no exit at the end of main", node->pos_);
            }
        }
    }
}

void SemanticChecker::visit(StructNode *node) {
    for (const auto &field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void SemanticChecker::visit(EnumerationNode *node) {
    for (const auto &variant: node->enum_variant_nodes_) {
        if (variant) variant->accept(this);
    }
}

void SemanticChecker::visit(ConstantItemNode *node) {
    std::string type_name;
    if (node->type_node_) {
        node->type_node_->accept(this);
        type_name = node->type_node_->toString();
    }
    if (node->expression_node_) node->expression_node_->accept(this);
    if (type_name != "i32" && type_name != "u32" &&
        type_name != "isize" && type_name != "usize") {
        Symbol symbol(node->pos_, node->identifier_, node->type_node_->type,
                      SymbolType::Variable, false);
        symbol.SetConst(true);
        scope_manager_.declare(symbol);
    }
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
    scope_manager_.current_scope = scope_manager_.current_scope
            ->next_level_scopes_[scope_manager_.current_scope->index++];
    scope_manager_.current_scope->index = 0;
    if (node->type_node_) node->type_node_->accept(this);
    auto type = scope_manager_.lookup(node->type_node_->toString()).type_;
    for (const auto &item: node->associated_item_nodes_) {
        if (item) {
            if (item->function_node_) {
                auto funcNode = item->function_node_;
                scope_manager_.current_scope = scope_manager_.current_scope
                        ->next_level_scopes_[scope_manager_.current_scope->index];
                if (funcNode->function_parameters_) {
                    auto params = funcNode->function_parameters_;
                    if (params->self_param_node_) {
                        auto self_param = params->self_param_node_;
                        auto tmp = std::dynamic_pointer_cast<ShortHandSelfNode>(self_param);
                        bool is_mut = false;
                        if (tmp && tmp->is_mut_) {
                            is_mut = true;
                        }

                        Symbol symbol(node->pos_, "self", type, SymbolType::Variable, is_mut);
                        scope_manager_.declare(symbol);
                    }
                }
                scope_manager_.PopScope();
            }
            item->accept(this);
        }
    }
    scope_manager_.PopScope();
}

void SemanticChecker::visit(TraitImplNode *node) {
}

void SemanticChecker::visit(FunctionParametersNode *node) {
    for (const auto &param: node->function_params_) {
        if (param) param->accept(this);
    }
}

void SemanticChecker::visit(FunctionParamNode *node) {
    // if (node->pattern_no_top_alt_node_) node->pattern_no_top_alt_node_->accept(this);
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
    for (const auto &field: node->struct_field_nodes_) {
        if (field) field->accept(this);
    }
}

void SemanticChecker::visit(EnumVariantDiscriminantNode *node) {
    if (node->expression_node_) node->expression_node_->accept(this);
}


void SemanticChecker::visit(StatementNode *node) {
}

void SemanticChecker::visit(StatementsNode *node) {
    for (const auto &stmt: node->statements_) {
        if (stmt) stmt->accept(this);
        if (interrupt) {
            return;
        }
    }
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(EmptyStatementNode *node) {
}

void SemanticChecker::visit(LetStatementNode *node) {
    // We now only consider variable declaration
    bool is_mut = false;
    std::shared_ptr<Type> type;
    std::string identifier;
    if (node->pattern_no_top_alt_) {
        // node->pattern_no_top_alt_->accept(this);
        auto tmp = std::dynamic_pointer_cast<IdentifierPatternNode>(node->pattern_no_top_alt_);
        if (tmp) {
            identifier = tmp->identifier_;
            is_mut = tmp->is_mut_;
        }
        auto path_pattern = std::dynamic_pointer_cast<PathPatternNode>(node->pattern_no_top_alt_);
        if (path_pattern) {
            auto expression = std::dynamic_pointer_cast<PathInExpressionNode>(path_pattern->expression_);
            if (expression) {
                uint32_t len = expression->path_indent_segments_.size();
                if (len == 0) {
                    throw SemanticError("Semantic Error: Path Pattern Error", node->pos_);
                }
                identifier = expression->path_indent_segments_[len - 1]->identifier_;
                is_mut = false;
            }
        }
    }
    if (node->type_) {
        node->type_->accept(this);
        type = node->type_->type;
        auto ref_type = std::dynamic_pointer_cast<ReferenceTypeNode>(node->type_);
        if (ref_type && ref_type->is_mut_) { is_mut = true; }
    }
    if (node->expression_) {
        node->expression_->accept(this);
        if (type) {
            bool match = false;
            auto never_type = scope_manager_.lookup("never").type_;
            for (const auto &it: node->expression_->types) {
                if (type->equal(it) || it->equal(never_type)) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Type not match", node->pos_);
            }
        } else {
            type = node->expression_->types[0];
        }
    }
    if (node->block_expression_) {
        node->block_expression_->accept(this);
        if (type) {
            bool match = false;
            auto never_type = scope_manager_.lookup("never").type_;
            for (const auto &it: node->block_expression_->types) {
                if (type->equal(it) || it->equal(never_type)) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Type not match", node->pos_);
            }
        } else {
            type = node->block_expression_->types[0];
        }
    }

    Symbol symbol(node->pos_, identifier, type, SymbolType::Variable, is_mut);
    if (!node->expression_) {
        symbol.SetAssign(false);
    }
    scope_manager_.declare(symbol, false);
}

void SemanticChecker::visit(VisItemStatementNode *node) {
    if (node->vis_item_node_) {
        node->vis_item_node_->accept(this);
    }
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
    if (node->lhs_ && node->rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto &it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "i32" || tmp->name_ == "u32" ||
                    tmp->name_ == "isize" || tmp->name_ == "usize" ||
                    tmp->name_ == "char" || tmp->name_ == "string" ||
                    tmp->name_ == "cstring" || tmp->name_ == "bool") {
                    valid = true;
                    break;
                }
            }
            auto enum_type = std::dynamic_pointer_cast<EnumerationType>(it);
            if (enum_type && (node->type_ == TokenType::EqEq || node->type_ == TokenType::NEq)) {
                valid = true;
                break;
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid ComparisonExpressionNode", node->pos_);
        }
    }
    node->types.emplace_back(scope_manager_.lookup("bool").type_);
}

void SemanticChecker::visit(TypeCastExpressionNode *node) {
    if (node->type_) {
        node->type_->accept(this);
        node->types.emplace_back(node->type_->type);
    }
    if (node->expression_) node->expression_->accept(this);
    node->is_compiler_known_ = node->expression_->is_compiler_known_;
    auto* val = std::get_if<int64_t>(&node -> expression_ -> value);
    if (val) {
        node->value = *val;
    }
}

void SemanticChecker::visit(AssignmentExpressionNode *node) {
    if (node->lhs_) {
        node->lhs_->accept(this);
        if (!node->lhs_->is_assignable_) {
            throw SemanticError("Semantic Error: Left Value Error", node->pos_);
        }
        auto var = std::dynamic_pointer_cast<PathInExpressionNode>(node->lhs_);
        bool assigned = true;
        if (var && var->path_indent_segments_.size() == 1) {
            std::string id = var->path_indent_segments_[0]->identifier_;
            assigned = scope_manager_.lookup(id).is_assigned_;
        }
        if (!node->lhs_->is_mutable_ && assigned) {
            throw SemanticError("Semantic Error: Left Value is not mutable", node->pos_);
        }
    }
    if (node->rhs_) node->rhs_->accept(this);
    node->types.emplace_back(scope_manager_.lookup("void").type_);
}

void SemanticChecker::visit(ContinueExpressionNode *node) {
    if (!in_loop_) {
        throw SemanticError("Semantic Error: Continue outside of loop", node->pos_);
    }
    interrupt = true;
}

void SemanticChecker::visit(UnderscoreExpressionNode *node) {
}

void SemanticChecker::visit(JumpExpressionNode *node) {
    node->types.emplace_back(scope_manager_.lookup("never").type_);
    if (node->expression_) {
        node->expression_->accept(this);
        interrupt = true;

        if (node->type_ == TokenType::Break) {
            if (!in_loop_) {
                throw SemanticError("Semantic Error: Break outside of loop", node->pos_);
            }
            if (in_while_loop_) {
                throw SemanticError("Semantic Error: Expression is not allowed after break in while loop",
                                    node->pos_);
            }
            if (loop_return_type_.empty()) {
                loop_return_type_ = node->expression_->types;
            } else {
                loop_return_type_ = cap(loop_return_type_, node->expression_->types);
                if (loop_return_type_.empty()) {
                    throw SemanticError("Semantic Error: Break type is not consistent in loop", node->pos_);
                }
            }
        } else if (node->type_ == TokenType::Return) {
            if (function_return_type_.empty()) {
                function_return_type_ = node->expression_->types;
            } else {
                function_return_type_ = cap(function_return_type_, node->expression_->types);
                if (function_return_type_.empty()) {
                    throw SemanticError("Semantic Error: Return type is not consistent in function",
                                        node->pos_);
                }
            }
        }
        return;
    }
    interrupt = true;
    if (node->type_ == TokenType::Break) {
        if (!in_loop_) {
            throw SemanticError("Semantic Error: Break outside of loop", node->pos_);
        }
        if (loop_return_type_.empty()) {
            loop_return_type_.emplace_back(scope_manager_.lookup("void").type_);
        } else {
            loop_return_type_ = cap(loop_return_type_, {scope_manager_.lookup("void").type_});
            if (loop_return_type_.empty()) {
                throw SemanticError("Semantic Error: Break Type is not consistent in loop", node->pos_);
            }
        }
    } else {
        auto void_ = std::vector{scope_manager_.lookup("void").type_};
        if (function_return_type_.empty()) {
            function_return_type_ = void_;
        } else {
            function_return_type_ = cap(function_return_type_, void_);
            if (function_return_type_.empty()) {
                throw SemanticError("Semantic Error: Return type is not consistent in function",
                                    node->pos_);
            }
        }
    }
}

void SemanticChecker::visit(LogicOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node->lhs_ && node->rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto &it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "bool") {
                    valid = true;
                    node->types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid BitwiseAndExpressionNode", node->pos_);
        }
    }
}

void SemanticChecker::visit(LogicAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node->lhs_ && node->rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto &it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "bool") {
                    valid = true;
                    node->types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid BitwiseAndExpressionNode", node->pos_);
        }
    }
}

void SemanticChecker::visit(BitwiseOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node->lhs_ && node->rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto &it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "i32" || tmp->name_ == "u32" ||
                    tmp->name_ == "isize" || tmp->name_ == "usize" ||
                    tmp->name_ == "bool") {
                    valid = true;
                    node->types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid BitwiseAndExpressionNode", node->pos_);
        }
    }
}

void SemanticChecker::visit(BitwiseXorExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node->lhs_ && node->rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto &it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "i32" || tmp->name_ == "u32" ||
                    tmp->name_ == "isize" || tmp->name_ == "usize" ||
                    tmp->name_ == "bool") {
                    valid = true;
                    node->types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid BitwiseAndExpressionNode", node->pos_);
        }
    }
}

void SemanticChecker::visit(BitwiseAndExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node->lhs_ && node->rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto &it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "i32" || tmp->name_ == "u32" ||
                    tmp->name_ == "isize" || tmp->name_ == "usize" ||
                    tmp->name_ == "bool") {
                    valid = true;
                    node->types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid BitwiseAndExpressionNode", node->pos_);
        }
    }
}

void SemanticChecker::visit(ShiftExpressionNode *node) {
    if (node->lhs_) {
        node->lhs_->accept(this);
        bool match = false;
        for (const auto &it: node->lhs_->types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "i32" || tmp->name_ == "u32" ||
                    tmp->name_ == "isize" || tmp->name_ == "usize") {
                    match = true;
                    node->types.emplace_back(it);
                }
            }
        }
        if (!match) {
            throw SemanticError("Semantic Error: Invalid ShiftExpressionNode", node->pos_);
        }
    }
    if (node->rhs_) {
        node->rhs_->accept(this);
        bool match = false;
        for (const auto &it: node->rhs_->types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "i32" || tmp->name_ == "u32" ||
                    tmp->name_ == "isize" || tmp->name_ == "usize") {
                    match = true;
                }
            }
        }
        if (!match) {
            throw SemanticError("Semantic Error: Invalid ShiftExpressionNode", node->pos_);
        }
    }
}

void SemanticChecker::visit(AddMinusExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node->lhs_ && node->rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto &it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "i32" || tmp->name_ == "u32" ||
                    tmp->name_ == "isize" || tmp->name_ == "usize") {
                    valid = true;
                    node->types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid AddMinusExpressionNode", node->pos_);
        }

        if (node->lhs_->is_compiler_known_ && node->rhs_->is_compiler_known_) {
            node->is_compiler_known_ = true;
            auto *l = std::get_if<int64_t>(&node->lhs_->value);
            auto *r = std::get_if<int64_t>(&node->rhs_->value);
            if (l && r) {
                if (node->type_ == TokenType::Plus) {
                    node->value = *l + *r;
                } else {
                    node->value = *l - *r;
                }
            }
        }
    }
}

void SemanticChecker::visit(MulDivModExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node->lhs_ && node->rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto &it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp->name_ == "i32" || tmp->name_ == "u32" ||
                    tmp->name_ == "isize" || tmp->name_ == "usize") {
                    valid = true;
                    node->types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid MulDivModExpressionNode", node->pos_);
        }

        if (node->lhs_->is_compiler_known_ && node->rhs_->is_compiler_known_) {
            node->is_compiler_known_ = true;
            auto *l = std::get_if<int64_t>(&node->lhs_->value);
            auto *r = std::get_if<int64_t>(&node->rhs_->value);
            if (l && r) {
                if (node->type_ == TokenType::Mul) {
                    node->value = (*l) * (*r);
                } else if (node->type_ == TokenType::Div) {
                    node->value = (*l) / (*r);
                } else {
                    node->value = (*l) % (*r);
                }
            }
        }
    }
}

void SemanticChecker::visit(UnaryExpressionNode *node) {
    if (node->expression_) {
        node->expression_->accept(this);
        if (node->type_ == TokenType::Minus) {
            bool match = false;
            for (const auto &it: node->expression_->types) {
                auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
                if (tmp && (tmp->name_ == "i32" || tmp->name_ == "isize")) {
                    match = true;
                    node->types.emplace_back(it);
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Invalid UnaryExpressionNode",
                                    node->pos_);
            }
            return;
        }
        if (node->type_ == TokenType::Not) {
            bool match = false;
            for (const auto &it: node->expression_->types) {
                auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
                if (tmp && (tmp->name_ == "i32" || tmp->name_ == "isize" ||
                            tmp->name_ == "u32" || tmp->name_ == "usize" ||
                            tmp->name_ == "bool")) {
                    match = true;
                    node->types.emplace_back(it);
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Invalid UnaryExpressionNode",
                                    node->pos_);
            }
            return;
        }
        if (node->type_ == TokenType::And) {
            for (const auto &it: node->expression_->types) {
                auto type = std::make_shared<ReferenceType>(it);
                node->types.emplace_back(type);
            }
        }
        if (node->type_ == TokenType::AndMut) {
            for (const auto &it: node->expression_->types) {
                auto type = std::make_shared<ReferenceType>(it, true);
                node->types.emplace_back(type);
            }
        }
        if (node->type_ == TokenType::Mul) {
            bool valid = false;
            for (const auto &it: node->expression_->types) {
                auto type = std::dynamic_pointer_cast<ReferenceType>(it);
                if (type) {
                    node->types.emplace_back(type->type_);
                    valid = true;
                }
            }
            node->is_assignable_ = true;
            node->is_mutable_ = node->expression_->is_mutable_;
            if (!valid) {
                throw SemanticError("Semantic Error: The Type in Dereference Expression is not a pointer type",
                                    node->pos_);
            }
        }
    }
}

void SemanticChecker::visit(FunctionCallExpressionNode *node) {
    std::shared_ptr<FunctionType> type;
    if (node->callee_) {
        node->callee_->accept(this);
        type = std::dynamic_pointer_cast<FunctionType>(node->callee_->types[0]);
        if (!type) {
            throw SemanticError("Semantic Error: Invalid Function Type", node->pos_);
        }
        if (type->is_mut_ && !node->callee_->is_mutable_) {
            throw SemanticError("Semantic Error: Invalid conversion from immutable callee to mutable self", node->pos_);
        }
    }
    if (node->params_.size() != type->params_.size()) {
        throw SemanticError("Semantic Error: Incorrect Parameter Numbers", node->pos_);
    }
    uint32_t index = 0;
    for (const auto &param: node->params_) {
        if (param) {
            param->accept(this);
            bool match = false;
            for (const auto &it: param->types) {
                auto val = std::get_if<int64_t>(&param->value);
                if (val && *val > 2147483647) {
                    throw SemanticError("Semantic Error: Integer Overflow", node->pos_);
                }
                if (type->params_[index]->equal(it)) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Function Param Type Not Match", node->pos_);
            }
            ++index;
        }
    }
    if (index != type->params_.size()) {
        throw SemanticError("Semantic Error: Incorrect Parameter Numbers", node->pos_);
    }
    node->types.emplace_back(type->ret_);
}

void SemanticChecker::visit(ArrayIndexExpressionNode *node) {
    std::shared_ptr<ArrayType> type;
    if (node->base_) {
        node->base_->accept(this);
        if (!node->base_->is_assignable_) {
            throw SemanticError("Semantic Error: Left Value Error", node->pos_);
        }
        auto tmp = node->base_->types[0];
        node->is_mutable_ = node->base_->is_mutable_;
        while (true) {
            type = std::dynamic_pointer_cast<ArrayType>(tmp);
            if (type) { break; }
            auto ref_type = std::dynamic_pointer_cast<ReferenceType>(tmp);
            if (!ref_type) {
                throw SemanticError("Semantic Error: Not an array type before the arrayIndexExpression",
                                    node->pos_);
            }
            tmp = ref_type->type_;
        	node->auto_deref_count++;
        }
    }
    if (node->index_) {
        node->index_->accept(this);
        bool valid = false;
        for (const auto &it: node->index_->types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp && tmp->name_ == "usize") {
                valid = true;
                break;
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid Index Type", node->pos_);
        }
    }
    node->types.emplace_back(type->base_);
}

void SemanticChecker::visit(MemberAccessExpressionNode *node) {
    if (node->base_) {
        node->base_->accept(this);
        node->is_mutable_ = node->base_->is_mutable_;
        std::shared_ptr<Type> type = node->base_->types[0];
        while (true) {
            auto tmp = std::dynamic_pointer_cast<StructType>(type);
            if (tmp) {
                for (const auto &it: tmp->members_) {
                    if (it.name_ == node->member_.token) {
                        node->types.emplace_back(it.type_);
                        return;
                    }
                }
            }
            for (const auto &it: type->methods_) {
                if (it.name_ == node->member_.token) {
                    node->types.emplace_back(it.type_);
                    return;
                }
            }
            auto ref_type = std::dynamic_pointer_cast<ReferenceType>(type);
            if (!ref_type) { break; }
            type = ref_type->type_;
        	node->auto_deref_count++;
        }
        throw SemanticError("Semantic Error: Invalid Member Access Expression", node->pos_);
    }
}

void SemanticChecker::visit(BlockExpressionNode *node) {
    scope_manager_.current_scope = scope_manager_.current_scope
            ->next_level_scopes_[scope_manager_.current_scope->index++];
    scope_manager_.current_scope->index = 0;
    if (node->statements_) {
        node->statements_->accept(this);
        if (interrupt) {
            node->types.emplace_back(scope_manager_.lookup("never").type_);
            scope_manager_.PopScope();
            interrupt = false;
            return;
        }
        node->types.emplace_back(scope_manager_.lookup("void").type_);
        if (node->statements_->expression_) {
            node->types = node->statements_->expression_->types;
        } else if (!node->statements_->statements_.empty()) {
            auto last_stmt = node->statements_->statements_.back();
            auto expr_stmt = std::dynamic_pointer_cast<ExpressionStatementNode>(last_stmt);
            if (expr_stmt && !expr_stmt->has_semicolon_) {
                node->types = expr_stmt->expression_->types;
            }
        }
    } else {
        node->types.emplace_back(scope_manager_.lookup("void").type_);
    }
    scope_manager_.PopScope();
}

void SemanticChecker::visit(LoopExpressionNode *node) {
}

void SemanticChecker::visit(InfiniteLoopExpressionNode *node) {
    if (node->block_expression_) {
        bool prev_in_loop = in_loop_;
        bool prev_in_for_loop = in_for_loop_;
        bool prev_in_while_loop = in_while_loop_;
        in_loop_ = true;
        in_for_loop_ = true;
        in_while_loop_ = false;
        node->block_expression_->accept(this);
        node->types = loop_return_type_;
        loop_return_type_.clear();
        in_loop_ = prev_in_loop;
        in_for_loop_ = prev_in_for_loop;
        in_while_loop_ = prev_in_while_loop;
        if (loop_return_type_.empty()) {
            node->types.emplace_back(scope_manager_.lookup("never").type_);
        } else {
            node->types = loop_return_type_;
        }
    }
}

void SemanticChecker::visit(PredicateLoopExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->block_expression_) {
        bool prev_in_loop = in_loop_;
        bool prev_in_for_loop = in_for_loop_;
        bool prev_in_while_loop = in_while_loop_;
        in_loop_ = true;
        in_for_loop_ = false;
        in_while_loop_ = true;
        node->block_expression_->accept(this);
        node->types = loop_return_type_;
        loop_return_type_.clear();
        in_loop_ = prev_in_loop;
        in_for_loop_ = prev_in_for_loop;
        in_while_loop_ = prev_in_while_loop;
    }
    node->types.emplace_back(scope_manager_.lookup("void").type_);
}

void SemanticChecker::visit(IfExpressionNode *node) {
    if (node->conditions_) node->conditions_->accept(this);
    if (node->true_block_expression_) node->true_block_expression_->accept(this);
    if (node->false_block_expression_) node->false_block_expression_->accept(this);
    if (node->if_expression_) node->if_expression_->accept(this);
    if (node->true_block_expression_ && node->false_block_expression_) {
        std::vector<std::shared_ptr<Type> > types = cap(node->true_block_expression_->types,
                                                        node->false_block_expression_->types);
        if (types.empty()) {
            throw SemanticError("Semantic Error: Type not match in if Expression", node->pos_);
        }
        node->types = types;
        return;
    }
    if (node->true_block_expression_ && node->if_expression_) {
        std::vector<std::shared_ptr<Type> > types = cap(node->true_block_expression_->types,
                                                        node->if_expression_->types);
        if (types.empty()) {
            throw SemanticError("Semantic Error: Type not match in if Expression", node->pos_);
        }
        node->types = types;
        return;
    }
    node->types.emplace_back(scope_manager_.lookup("void").type_);
}

void SemanticChecker::visit(MatchExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
    if (node->match_arms_) node->match_arms_->accept(this);
}

void SemanticChecker::visit(LiteralExpressionNode *node) {
}

void SemanticChecker::visit(CharLiteralNode *node) {
    std::shared_ptr<Type> type = scope_manager_.lookup("char").type_;
    node->is_compiler_known_ = true;
    node->types.emplace_back(type);
}

void SemanticChecker::visit(StringLiteralNode *node) {
    std::shared_ptr<Type> type = scope_manager_.lookup("str").type_;
    type = std::make_shared<ReferenceType>(type);
    node->is_compiler_known_ = true;
    node->value = node->string_literal_;
    node->types.emplace_back(type);
}

void SemanticChecker::visit(CStringLiteralNode *node) {
    std::shared_ptr<Type> type = scope_manager_.lookup("cstring").type_;
    node->is_compiler_known_ = true;
    node->value = node->c_string_literal_;
    node->types.emplace_back(type);
}

void SemanticChecker::visit(IntLiteralNode *node) {
    node->is_compiler_known_ = true;
    node->value = node->int_literal_;
    if (node->is_i32_) {
        std::shared_ptr<Type> type = scope_manager_.lookup("i32").type_;
        node->types.emplace_back(type);
    }
    if (node->is_u32_) {
        std::shared_ptr<Type> type = scope_manager_.lookup("u32").type_;
        node->types.emplace_back(type);
    }
    if (node->is_usize_) {
        std::shared_ptr<Type> type = scope_manager_.lookup("usize").type_;
        node->types.emplace_back(type);
    }
    if (node->is_isize_) {
        std::shared_ptr<Type> type = scope_manager_.lookup("isize").type_;
        node->types.emplace_back(type);
    }
}

void SemanticChecker::visit(BoolLiteralNode *node) {
    std::shared_ptr<Type> type = scope_manager_.lookup("bool").type_;
    node->is_compiler_known_ = true;
    node->types.emplace_back(type);
}

void SemanticChecker::visit(ArrayLiteralNode *node) {
    bool init = true;
    std::vector<std::shared_ptr<Type> > element_types;
    node->is_compiler_known_ = true;
    for (const auto &expr: node->expressions_) {
        if (expr) {
            expr->accept(this);
            std::vector<std::shared_ptr<Type> > tmp;
            if (init) {
                element_types = expr->types;
                init = false;
            } else {
                element_types = cap(element_types, expr->types);
            }
        }
    }
    if (!init) {
        if (element_types.empty()) {
            throw SemanticError("Semantic Error: Elements in array literal is not consistent",
                                node->pos_);
        }
        for (const auto &it: element_types) {
            node->types.emplace_back(std::make_shared<ArrayType>(it,
                                                                 node->expressions_.size()));
        }
        return;
    }
    if (node->lhs_) {
        node->lhs_->accept(this);
        element_types = node->lhs_->types;
    }
    uint32_t size = 0;
    if (node->rhs_) {
        node->rhs_->accept(this);
        if (!node->rhs_->is_compiler_known_) {
            throw SemanticError("Semantic Error: The size of array is not a Compiler-known Constant",
                                node->pos_);
        }
        const auto *tmp = std::get_if<int64_t>(&node->rhs_->value);
        size = *tmp;
    }
    for (const auto &it: element_types) {
        node->types.emplace_back(std::make_shared<ArrayType>(it, size));
    }
}

void SemanticChecker::visit(PathExpressionNode *node) {
}

void SemanticChecker::visit(PathInExpressionNode *node) {
    for (const auto &seg: node->path_indent_segments_) {
        if (seg) seg->accept(this);
    }
    uint32_t len = node->path_indent_segments_.size();
    if (len == 2) {
        std::string type_name = node->path_indent_segments_[0]->identifier_;
        std::string func_name = node->path_indent_segments_[1]->identifier_;
        Symbol symbol = scope_manager_.lookup(type_name);
        if (symbol.symbol_type_ == SymbolType::Struct) {
            for (auto &it: symbol.type_->inline_functions_) {
                if (it.name_ == func_name) {
                    node->types.emplace_back(it.type_);
                    return;
                }
            }
            for (auto &it: symbol.type_->constants_) {
                if (it.name_ == func_name) {
                    node->is_compiler_known_ = true;
                    auto value = symbol.type_->value_map[it.name_];
                    if (auto *tmp = std::get_if<int64_t>(&value)) {
                        node->value = *tmp;
                    }
                    node->types.emplace_back(it.type_);
                    return;
                }
            }
        }
        if (symbol.symbol_type_ == SymbolType::Enumeration) {
            auto enum_type = std::dynamic_pointer_cast<EnumerationType>(symbol.type_);
            for (auto &it: enum_type->variants_) {
                if (it == func_name) {
                    node->is_compiler_known_ = true;
                    node->types.emplace_back(symbol.type_);
                    return;
                }
            }
        }
        throw SemanticError("Semantic Error: Invalid PathInExpression", node->pos_);
    }
    if (len == 1) {
        if (node->path_indent_segments_[0]->identifier_ == "exit") {
            if (has_exit) {
                throw SemanticError("Semantic Error: Duplicate exit function in code", node->pos_);
            }
            has_exit = true;
        }
        Symbol symbol = scope_manager_.lookup(node->path_indent_segments_[0]->identifier_);
        node->types.emplace_back(symbol.type_);
        node->is_mutable_ = symbol.is_mutable_;
        if (symbol.is_const_) {
            node->is_compiler_known_ = true;
            auto value = scope_manager_.SearchValue(symbol.name_);
            if (auto *tmp = std::get_if<int64_t>(&value)) {
                node->value = *tmp;
            }
        }
    }
}

void SemanticChecker::visit(PathIndentSegmentNode *node) {
}

void SemanticChecker::visit(StructExpressionNode *node) {
    if (node->path_in_expression_node_) {
        node->path_in_expression_node_->accept(this);
        node->types.emplace_back(node->path_in_expression_node_->types[0]);
    }
    if (node->struct_expr_fields_node_) {
        node->struct_expr_fields_node_->accept(this);
        auto struct_type = std::dynamic_pointer_cast<StructType>(node->types[0]);
        if (struct_type->members_.size() != node->struct_expr_fields_node_->struct_expr_field_nodes_.size()) {
            throw SemanticError("Semantic Error: Invalid StructExpression", node->pos_);
        }
        for (uint32_t i = 0; i < struct_type->members_.size(); i++) {
            auto element = node->struct_expr_fields_node_->struct_expr_field_nodes_[i];
            bool match = false;
            for (auto &it: element->expression_node_->types) {
                if (struct_type->members_[i].type_->equal(it)) {
                    match = true;
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Invalid StructExpression", node->pos_);
            }
        }
    }
    if (node->struct_base_node_) node->struct_base_node_->accept(this);
}

void SemanticChecker::visit(StructExprFieldsNode *node) {
    for (const auto &field: node->struct_expr_field_nodes_) {
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
    if (node->expression_) {
        node->expression_->accept(this);
        node->types = node->expression_->types;
    }
}

void SemanticChecker::visit(TupleExpressionNode *node) {
    for (const auto &expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
}

void SemanticChecker::visit(ConditionsNode *node) {
    if (node->expression_) {
        node->expression_->accept(this);
        bool valid = false;
        for (auto &it: node->expression_->types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp && tmp->name_ == "bool") {
                valid = true;
                break;
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Expression in condition is not a bool type", node->pos_);
        }
    }
}

void SemanticChecker::visit(LetChainNode *node) {
}

void SemanticChecker::visit(LetChainConditionNode *node) {
}


void SemanticChecker::visit(MatchArmsNode *node) {
    for (const auto &arm: node->match_arm_nodes_) {
        if (arm) arm->accept(this);
    }
    for (const auto &expr: node->expression_nodes_) {
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
    for (const auto &pat: node->pattern_no_top_alts_) {
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
    for (const auto &pat: node->patterns_) {
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
    if (node->type_path_segment_node_) {
        node->type_path_segment_node_->accept(this);
        Symbol sym = scope_manager_.lookup(node->type_path_segment_node_->
            path_indent_segment_node_->identifier_);
        node->type = sym.type_;
    }
}

void SemanticChecker::visit(TypePathSegmentNode *node) {
    if (node->path_indent_segment_node_) node->path_indent_segment_node_->accept(this);
}

void SemanticChecker::visit(UnitTypeNode *node) {
    node->type = scope_manager_.lookup("void").type_;
}

void SemanticChecker::visit(ArrayTypeNode *node) {
    std::shared_ptr<Type> base_type;
    uint32_t size = 0;
    if (node->type_) {
        node->type_->accept(this);
        base_type = node->type_->type;
    }
    if (node->expression_node_) {
        node->expression_node_->accept(this);
        if (!node->expression_node_->is_compiler_known_) {
            throw SemanticError("Semantic Error: The size of array is not a Compiler-known Constant",
                                node->pos_);
        }
        const auto *tmp = std::get_if<int64_t>(&node->expression_node_->value);
        size = *tmp;
    }
    node->type = std::make_shared<ArrayType>(base_type, size);
}

void SemanticChecker::visit(SliceTypeNode *node) {
    std::shared_ptr<Type> base_type;
    if (node->type_) {
        node->type_->accept(this);
        base_type = node->type_->type;
    }
    node->type = std::make_shared<SliceType>(base_type);
}

void SemanticChecker::visit(ReferenceTypeNode *node) {
    if (node->type_node_) {
        node->type_node_->accept(this);
        node->type = std::make_shared<ReferenceType>(node->type_node_->type, node->is_mut_);
    }
}


void SemanticChecker::visit(ConstParamNode *node) {
}

void SemanticChecker::visit(TypeParamNode *node) {
}

void SemanticChecker::visit(TypeParamBoundsNode *node) {
}

void SemanticChecker::visit(QualifiedPathInExpressionNode *node) {
}

/****************  Supportive Function  ****************/
std::vector<std::shared_ptr<Type> > SemanticChecker::cap(const std::vector<std::shared_ptr<Type> > &a,
                                                         const std::vector<std::shared_ptr<Type> > &b) {
    std::vector<std::shared_ptr<Type> > ret;
    std::shared_ptr<Type> never = scope_manager_.lookup("never").type_;
    for (const auto &it: a) {
        for (const auto &itp: b) {
            if (it->equal(never)) {
                return b;
            }
            if (itp->equal(never)) {
                return a;
            }
            if (it->equal(itp)) {
                ret.emplace_back(it);
                break;
            }
        }
    }
    return ret;
}
