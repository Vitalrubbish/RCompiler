#include "Semantic/SemanticChecker.h"
#include "Semantic/ASTNode.h"
#include "Semantic/SymbolCollector.h"
#include "Semantic/Type.h"
#include "Semantic/Symbol.h"

extern SymbolCollector *symbol_collector;

void SemanticChecker::visit(ASTNode *node) {
}

void SemanticChecker::visit(CrateNode *node) {
    scope_manager_.current_scope = scope_manager_.root;
    scope_manager_.current_scope -> index = 0;
    for (const auto &item: node->items_) {
        if (item) item->accept(this);
    }
}

void SemanticChecker::visit(VisItemNode *node) {
}

void SemanticChecker::visit(FunctionNode *node) {
    if (node->function_parameters_) node->function_parameters_->accept(this);
    if (node->type_) node->type_->accept(this);
    if (node -> function_parameters_) {
        scope_manager_.current_scope = scope_manager_.current_scope
            -> next_level_scopes_[scope_manager_.current_scope -> index];
        for (const auto& it: node->function_parameters_->function_params_) {
            it->accept(this);
            auto pattern_node = std::dynamic_pointer_cast<IdentifierPatternNode>(it -> pattern_no_top_alt_node_);
            if (pattern_node) {
                std::string identifier = pattern_node -> identifier_;
                bool is_mut = pattern_node -> is_mut_;
                std::shared_ptr<Type> type = it->type_->type;
                Symbol symbol(node->pos_, identifier, type, SymbolType::Variable, is_mut);
                scope_manager_.declare(symbol);
                continue;
            }
            auto path_pattern = std::dynamic_pointer_cast<PathPatternNode>(it -> pattern_no_top_alt_node_);
            if (path_pattern) {
                bool is_mut;
                std::string identifier;
                auto expression = std::dynamic_pointer_cast<PathInExpressionNode>(path_pattern->expression_);
                if (expression) {
                    uint32_t len = expression -> path_indent_segments_.size();
                    if (len == 0) {
                        throw SemanticError("Semantic Error: Path Pattern Error", node->pos_);
                    }
                    identifier = expression -> path_indent_segments_[len - 1] -> identifier_;
                    is_mut = false;
                }
                std::shared_ptr<Type> type = it->type_->type;
                Symbol symbol(node->pos_, identifier, type, SymbolType::Variable, is_mut);
                scope_manager_.declare(symbol);
            }
        }
        scope_manager_.PopScope();
    }
    if (node->block_expression_) node->block_expression_->accept(this);
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
    for (const auto &item: node->associated_item_nodes_) {
        if (item) item->accept(this);
    }
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
        auto path_pattern = std::dynamic_pointer_cast<PathPatternNode>(node -> pattern_no_top_alt_);
        if (path_pattern) {
            auto expression = std::dynamic_pointer_cast<PathInExpressionNode>(path_pattern->expression_);
            if (expression) {
                uint32_t len = expression -> path_indent_segments_.size();
                if (len == 0) {
                    throw SemanticError("Semantic Error: Path Pattern Error", node->pos_);
                }
                identifier = expression -> path_indent_segments_[len - 1] -> identifier_;
                is_mut = false;
            }
        }
    }
    if (node->type_) {
        node->type_->accept(this);
        type = node -> type_ -> type;
    }
    if (node->expression_) {
        node->expression_->accept(this);
        if (type) {
            bool match = false;
            for (const auto& it: node -> expression_ -> types) {
                if (type -> equal(it)) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Type not match", node -> pos_);
            }
        } else {
            type = node -> expression_ -> types[0];
        }
    }
    if (node->block_expression_) {
        node->block_expression_->accept(this);
        if (type) {
            bool match = false;
            for (const auto& it: node -> block_expression_ -> types) {
                if (type -> equal(it)) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Type not match", node -> pos_);
            }
        } else {
            type = node -> block_expression_ -> types[0];
        }
    }

    Symbol symbol(node -> pos_, identifier, type, SymbolType::Variable, is_mut);
    scope_manager_.declare(symbol);
}

void SemanticChecker::visit(VisItemStatementNode *node) {
    if (node -> vis_item_node_) {
        node -> vis_item_node_ -> accept(this);
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
    if (node -> lhs_ && node -> rhs_) {
        auto cap_types = cap(node -> lhs_ -> types, node -> rhs_ -> types);
        bool valid = false;
        for (const auto& it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp -> name_ == "i32" || tmp -> name_ == "u32" ||
                    tmp -> name_ == "isize" || tmp -> name_ == "usize" ||
                    tmp -> name_ == "char" || tmp -> name_ == "string" ||
                    tmp -> name_ == "cstring") {
                    valid = true;
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid ComparisonExpressionNode", node -> pos_);
        }
    }
    node -> types.emplace_back(scope_manager_.lookup("bool").type_);
}

void SemanticChecker::visit(TypeCastExpressionNode *node) {
    if (node->type_) {
        node->type_->accept(this);
        node -> types.emplace_back(node -> type_ -> type);
    }
    if (node->expression_) node->expression_->accept(this);
}

void SemanticChecker::visit(AssignmentExpressionNode *node) {
    if (node->lhs_) {
        node->lhs_->accept(this);
        if (!node->lhs_->is_assignable_) {
            throw SemanticError("Semantic Error: Left Value Error", node->pos_);
        }
        if (!node -> lhs_ -> is_mutable_) {
            throw SemanticError("Semantic Error: Left Value is not mutable", node -> pos_);
        }
    }
    if (node->rhs_) node->rhs_->accept(this);
    node -> types.emplace_back(scope_manager_.lookup("void").type_);
}

void SemanticChecker::visit(ContinueExpressionNode *node) {
    if (!scope_manager_.in_loop()) {
        throw SemanticError("Semantic Error: Continue outside of loop", node->pos_);
    }
}

void SemanticChecker::visit(UnderscoreExpressionNode *node) {
}

void SemanticChecker::visit(JumpExpressionNode *node) {
    if (node->expression_) {
        if (node -> type_ == TokenType::Break && !scope_manager_.in_loop()) {
            throw SemanticError("Semantic Error: Continue outside of loop", node->pos_);
        }
        node->expression_->accept(this);
    }
}

void SemanticChecker::visit(LogicOrExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node -> lhs_ && node -> rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto& it: cap_types) {
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
    if (node -> lhs_ && node -> rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto& it: cap_types) {
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
    if (node -> lhs_ && node -> rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto& it: cap_types) {
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
    if (node -> lhs_ && node -> rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto& it: cap_types) {
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
    if (node -> lhs_ && node -> rhs_) {
        auto cap_types = cap(node->lhs_->types, node->rhs_->types);
        bool valid = false;
        for (const auto& it: cap_types) {
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
        for (const auto& it: node -> lhs_ -> types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp -> name_ == "i32" || tmp -> name_ == "u32" ||
                    tmp -> name_ == "isize" || tmp -> name_ == "usize") {
                    match = true;
                    node -> types.emplace_back(it);
                }
            }
        }
        if (!match) {
            throw SemanticError("Semantic Error: Invalid ShiftExpressionNode", node -> pos_);
        }
    }
    if (node->rhs_) {
        node->rhs_->accept(this);
        bool match = false;
        for (const auto& it: node -> rhs_ -> types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp -> name_ == "i32" || tmp -> name_ == "u32" ||
                    tmp -> name_ == "isize" || tmp -> name_ == "usize") {
                    match = true;
                }
            }
        }
        if (!match) {
            throw SemanticError("Semantic Error: Invalid ShiftExpressionNode", node -> pos_);
        }
    }
}

void SemanticChecker::visit(AddMinusExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node -> lhs_ && node -> rhs_) {
        auto cap_types = cap(node -> lhs_ -> types, node -> rhs_ -> types);
        bool valid = false;
        for (const auto& it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp -> name_ == "i32" || tmp -> name_ == "u32" ||
                    tmp -> name_ == "isize" || tmp -> name_ == "usize") {
                    valid = true;
                    node -> types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid AddMinusExpressionNode", node -> pos_);
        }
    }
}

void SemanticChecker::visit(MulDivModExpressionNode *node) {
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
    if (node -> lhs_ && node -> rhs_) {
        auto cap_types = cap(node -> lhs_ -> types, node -> rhs_ -> types);
        bool valid = false;
        for (const auto& it: cap_types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp) {
                if (tmp -> name_ == "i32" || tmp -> name_ == "u32" ||
                    tmp -> name_ == "isize" || tmp -> name_ == "usize") {
                    valid = true;
                    node -> types.emplace_back(it);
                }
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid MulDivModExpressionNode", node -> pos_);
        }
    }
}

void SemanticChecker::visit(UnaryExpressionNode *node) {
    if (node->expression_) {
        node->expression_->accept(this);
        if (node -> type_ == TokenType::Minus) {
            bool match = false;
            for (const auto& it: node -> expression_ -> types) {
                auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
                if (tmp && (tmp -> name_ == "i32" || tmp -> name_ == "isize")) {
                    match = true;
                    node -> types.emplace_back(it);
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Invalid UnaryExpressionNode",
                    node -> pos_);
            }
            return;
        }
        if (node -> type_ == TokenType::Not) {
            bool match = false;
            for (const auto& it: node -> expression_ -> types) {
                auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
                if (tmp && (tmp -> name_ == "i32" || tmp -> name_ == "isize" ||
                    tmp -> name_ == "u32" || tmp -> name_ == "usize" ||
                    tmp -> name_ == "bool")) {
                    match = true;
                    node -> types.emplace_back(it);
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Invalid UnaryExpressionNode",
                    node -> pos_);
            }
            return;
        }
        // TODO Handle *, &, &mut, &&, &&mut
    }
}

void SemanticChecker::visit(FunctionCallExpressionNode *node) {
    std::shared_ptr<FunctionType> type;
    if (node->callee_) {
        node->callee_->accept(this);
        type = std::dynamic_pointer_cast<FunctionType>(node -> callee_ -> types[0]);
        if (!type) {
            throw SemanticError("Semantic Error: Invalid Function Type", node -> pos_);
        }
    }
    if (node -> params_.size() != type -> params_.size()) {
        throw SemanticError("Semantic Error: Incorrect Parameter Numbers", node -> pos_);
    }
    uint32_t index = 0;
    for (const auto &param: node->params_) {
        if (param) {
            param->accept(this);
            bool match = false;
            for (const auto& it: param -> types) {
                if (type -> params_[index] -> equal(it)) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                throw SemanticError("Semantic Error: Function Param Type Not Match", node -> pos_);
            }
            ++index;
        }
    }
    if (index != type -> params_.size()) {
        throw SemanticError("Semantic Error: Incorrect Parameter Numbers", node -> pos_);
    }
    node -> types.emplace_back(type -> ret_);
}

void SemanticChecker::visit(ArrayIndexExpressionNode *node) {
    std::shared_ptr<ArrayType> type;
    if (node->base_) {
        node->base_->accept(this);
        auto tmp = node -> base_ -> types[0];
        node -> is_mutable_ = node -> base_ -> is_mutable_;
        type = std::dynamic_pointer_cast<ArrayType>(tmp);
        if (!type) {
            throw SemanticError("Semantic Error: Not an array type before the arrayIndexExpression",
                node -> pos_);
        }
    }
    if (node->index_) {
        node->index_->accept(this);
        bool valid = false;
        for (const auto& it: node -> index_ -> types) {
            auto tmp = std::dynamic_pointer_cast<PrimitiveType>(it);
            if (tmp && tmp -> name_ == "usize") {
                valid = true;
                break;
            }
        }
        if (!valid) {
            throw SemanticError("Semantic Error: Invalid Index Type", node -> pos_);
        }
    }
    node -> types.emplace_back(type -> base_);
}

void SemanticChecker::visit(MemberAccessExpressionNode *node) {
    if (node->base_) {
        node->base_->accept(this);
        std::shared_ptr<Type> type = node -> base_ -> types[0];
        node -> is_mutable_ = node -> base_ -> is_mutable_;
        auto tmp = std::dynamic_pointer_cast<StructType>(type);
        if (tmp) {
            for (const auto& it: tmp -> members_) {
                if (it.name_ == node -> member_.token) {
                    node -> types.emplace_back(it.type_);
                    return;
                }
            }
        }
        for (const auto& it: type -> methods_) {
            if (it.name_ == node -> member_.token) {
                node -> types.emplace_back(it.type_);
                return;
            }
        }
        throw SemanticError("Semantic Error: Invalid Member Access Expression", node -> pos_);
    }
}

void SemanticChecker::visit(BlockExpressionNode *node) {
    scope_manager_.current_scope = scope_manager_.current_scope
        ->next_level_scopes_[scope_manager_.current_scope->index++];
    scope_manager_.current_scope -> index = 0;
    if (node->statements_) {
        node->statements_->accept(this);
        if (node -> statements_ -> expression_) {
            node -> types = node -> statements_ -> expression_ -> types;
        } else {
            node -> types.emplace_back(scope_manager_.lookup("void").type_);
        }
    }
    scope_manager_.PopScope();
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
    if (node -> true_block_expression_ && node -> false_block_expression_) {
        std::vector<std::shared_ptr<Type>> types = cap(node -> true_block_expression_ -> types,
            node -> false_block_expression_ -> types);
        if (types.empty()) {
            throw SemanticError("Semantic Error: Type not match in if Expression", node -> pos_);
        }
        node -> types = types;
        return;
    }
    if (node -> true_block_expression_ && node -> if_expression_) {
        std::vector<std::shared_ptr<Type>> types = cap(node -> true_block_expression_ -> types,
            node -> if_expression_ -> types);
        if (types.empty()) {
            throw SemanticError("Semantic Error: Type not match in if Expression", node -> pos_);
        }
        node -> types = types;
        return;
    }
    node -> types.emplace_back(scope_manager_.lookup("void").type_);
    // TODO: To be Tested
}

void SemanticChecker::visit(MatchExpressionNode *node) {
    if (node->expression_) node->expression_->accept(this);
    if (node->match_arms_) node->match_arms_->accept(this);
}

void SemanticChecker::visit(LiteralExpressionNode *node) {
}

void SemanticChecker::visit(CharLiteralNode *node) {
    std::shared_ptr<Type> type = scope_manager_.lookup("char").type_;
    node->types.emplace_back(type);
}

void SemanticChecker::visit(StringLiteralNode *node) {
    std::shared_ptr<Type> type = scope_manager_.lookup("string").type_;
    node->types.emplace_back(type);
}

void SemanticChecker::visit(CStringLiteralNode *node) {
    std::shared_ptr<Type> type = scope_manager_.lookup("cstring").type_;
    node->types.emplace_back(type);
}

void SemanticChecker::visit(IntLiteralNode *node) {
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
    node->types.emplace_back(type);
}

void SemanticChecker::visit(ArrayLiteralNode *node) {
    bool init = true;
    std::vector<std::shared_ptr<Type> > element_types;
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
        auto tmp = std::dynamic_pointer_cast<IntLiteralNode>(node->rhs_);
        if (!tmp) {
            throw SemanticError(
                "Semantic Error: The size of an array must be an integer literal", node->pos_);
        }
        size = tmp->int_literal_;
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
    uint32_t len = node -> path_indent_segments_.size();
    Symbol symbol = scope_manager_.lookup(node -> path_indent_segments_[len - 1]->identifier_);
    node -> types.emplace_back(symbol.type_);
    node -> is_mutable_ = symbol.is_mutable_;
}

void SemanticChecker::visit(PathIndentSegmentNode *node) {
}

void SemanticChecker::visit(StructExpressionNode *node) {
    if (node->path_in_expression_node_) {
        node->path_in_expression_node_->accept(this);
        node -> types.emplace_back(node -> path_in_expression_node_ -> types[0]);
    }
    if (node->struct_expr_fields_node_) node->struct_expr_fields_node_->accept(this);
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
        node -> types = node -> expression_ -> types;
    }
}

void SemanticChecker::visit(TupleExpressionNode *node) {
    for (const auto &expr: node->expressions_) {
        if (expr) expr->accept(this);
    }
}

void SemanticChecker::visit(ConditionsNode *node) {
    if (node->expression_) node->expression_->accept(this);
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
        node -> type = sym.type_;
    }
}

void SemanticChecker::visit(TypePathSegmentNode *node) {
    if (node->path_indent_segment_node_) node->path_indent_segment_node_->accept(this);
}

void SemanticChecker::visit(TupleTypeNode *node) {
    for (const auto &type: node->type_nodes_) {
        if (type) type->accept(this);
    }
}

void SemanticChecker::visit(ArrayTypeNode *node) {
    std::shared_ptr<Type> base_type;
    uint32_t size = 0;
    if (node->type_) {
        node->type_->accept(this);
        base_type = node -> type_ -> type;
    }
    if (node->expression_node_) {
        node->expression_node_->accept(this);
        auto tmp = std::dynamic_pointer_cast<IntLiteralNode>(node -> expression_node_);
        if (!tmp) {
            throw SemanticError(
                "Semantic Error: The size of an array must be an integer literal", node->pos_);
        }
        size = tmp -> int_literal_;
    }
    node -> type = std::make_shared<ArrayType>(base_type, size);
}

void SemanticChecker::visit(SliceTypeNode *node) {
    std::shared_ptr<Type> base_type;
    if (node->type_) {
        node->type_->accept(this);
        base_type = node -> type_ -> type;
    }
    node -> type = std::make_shared<SliceType>(base_type);
}

void SemanticChecker::visit(ReferenceTypeNode *node) {
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
    for (const auto &it: a) {
        for (const auto &itp: b) {
            if (it -> equal(itp)) {
                ret.emplace_back(it);
                break;
            }
        }
    }
    return ret;
}