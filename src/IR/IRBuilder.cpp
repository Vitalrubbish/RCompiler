#include "IR/IRBuilder.h"

#include "IR/IRBlock.h"
#include "IR/IRProgram.h"
#include "IR/IRType.h"
#include "IR/IRLiteral.h"
#include "Semantic/ASTNode.h"
#include "Semantic/SymbolCollector.h"
#include "Semantic/Type.h"
#include "Semantic/Symbol.h"

extern SymbolCollector *symbol_collector;

extern std::shared_ptr<IRProgram> ir_program;

void IRBuilder::visit(ASTNode *node) {
}

void IRBuilder::visit(CrateNode *node) {
    scope_manager_.current_scope = scope_manager_.root;
    scope_manager_.current_scope -> index = 0;
    for (const auto& item: node->items_) {
        auto const_item = std::dynamic_pointer_cast<ConstantItemNode>(item);
        if (const_item) {
        	auto const_var = std::make_shared<ConstVar>(const_item->identifier_, std::make_shared<IRIntegerType>(32));
        	auto val = std::get_if<int64_t>(&const_item->expression_node_->value);
        	if (val) {
        		auto ir_literal = std::make_shared<LiteralInt>(*val);
        		ir_program->constants.emplace_back(std::make_shared<ConstVarDefInstruction>(const_var, ir_literal));
        	}
        }
    }

	for (const auto& item: node->items_) {
		auto struct_item = std::dynamic_pointer_cast<StructNode>(item);
		if (struct_item) {
			auto struct_type = scope_manager_.lookup(struct_item->identifier_).type_;
			ir_manager_.AddType(struct_type);
			auto ir_struct_type = std::dynamic_pointer_cast<IRStructType>(ir_manager_.GetIRType(struct_type));
			ir_program->structs.emplace_back(std::make_shared<StructDefInstruction>(ir_struct_type, ir_struct_type->members));
		}
	}

	for (const auto& item: node->items_) {
		auto func_item = std::dynamic_pointer_cast<FunctionNode>(item);
		if (func_item) {
			std::shared_ptr<IRType> ir_ret_type = std::make_shared<IRVoidType>();
			if (func_item->type_) {
				ir_ret_type = ir_manager_.GetIRType(func_item->type_->type);
			}
			auto parameters = func_item->function_parameters_;
			std::vector<IRFunctionParam> ir_function_params;
			if (parameters) {
				for (auto& param : parameters->function_params_) {
					auto ir_type = ir_manager_.GetIRType(param->type_->type);
					auto identifier_pattern = std::dynamic_pointer_cast<IdentifierPatternNode>(param->pattern_no_top_alt_node_);
					ir_function_params.emplace_back(ir_type, identifier_pattern->identifier_);
				}
			}
			auto ir_function = std::make_shared<IRFunction>(func_item->identifier_, ir_ret_type, ir_function_params);
			ir_program->functions.emplace_back(ir_function);
			ir_manager_.function_map_[func_item->identifier_] = ir_function;
		}
	}

    for (const auto &item: node->items_) {
        if (item) item->accept(this);
    }
}

void IRBuilder::visit(VisItemNode *node) {
}

void IRBuilder::visit(FunctionNode *node) {
	current_function = ir_manager_.function_map_[node->identifier_];
	current_block = std::make_shared<IRBasicBlock>("entry");
	current_function->blocks.emplace_back(current_block);
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
	std::string identifier;
	auto identifier_pattern = std::dynamic_pointer_cast<IdentifierPatternNode>(node->pattern_no_top_alt_);
	if (identifier_pattern) {
		identifier = identifier_pattern->identifier_;
	}
	auto ir_type = ir_manager_.GetIRType(node->type_->type);
	auto ir_var = std::make_shared<LocalVar>(identifier, ir_type);
	current_block->instructions.emplace_back(std::make_shared<AllocaInstruction>(ir_var, ir_type));
	auto value = std::make_shared<LocalVar>("", ir_type);
	if (node->expression_) {
		if (node->expression_->is_assignable_) {
			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(value, ir_type, node->expression_->result_var));
		} else {
			value = node->expression_->result_var;
		}
		current_block->instructions.emplace_back(std::make_shared<StoreInstruction>(ir_type, value, ir_var));
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
	auto ir_type = ir_manager_.GetIRType(node->lhs_->types[0]);
	auto ir_var = std::make_shared<LocalVar>("", ir_type);
	if (node->rhs_->is_assignable_) {
		current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(ir_var, ir_type, node->rhs_->result_var));
	} else {
		ir_var = node->rhs_->result_var;
	}
	if (node->type_ == TokenType::Eq) {
		current_block->instructions.emplace_back(std::make_shared<StoreInstruction>(ir_type, ir_var, node->lhs_->result_var));
	}
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
	auto ir_type = ir_manager_.GetIRType(node->types[0]);
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
	auto l_value = std::make_shared<LocalVar>("", ir_type);
	auto r_value = std::make_shared<LocalVar>("", ir_type);
	auto ir_type_ptr = std::make_shared<IRPointerType>(ir_type);
	node->result_var = std::make_shared<LocalVar>("", ir_type_ptr);
	if (node->type_ == TokenType::Plus) {
		if (node->lhs_->is_assignable_) {
			auto load_instruction_l = std::make_shared<LoadInstruction>(l_value, ir_type, node->lhs_->result_var);
			current_block->instructions.emplace_back(load_instruction_l);
		} else {
			l_value = node->lhs_->result_var;
		}
		if (node->rhs_->is_assignable_) {
			auto load_instruction_r = std::make_shared<LoadInstruction>(r_value, ir_type, node->rhs_->result_var);
			current_block->instructions.emplace_back(load_instruction_r);
		} else {
			r_value = node->rhs_->result_var;
		}
		auto add_instruction = std::make_shared<AddInstruction>(node->result_var, ir_type, l_value, r_value);
		current_block->instructions.emplace_back(add_instruction);
	} else if (node->type_ == TokenType::Minus) {
		if (node->lhs_->is_assignable_) {
			auto load_instruction_l = std::make_shared<LoadInstruction>(l_value, ir_type, node->lhs_->result_var);
			current_block->instructions.emplace_back(load_instruction_l);
		} else {
			l_value = node->lhs_->result_var;
		}
		if (node->rhs_->is_assignable_) {
			auto load_instruction_r = std::make_shared<LoadInstruction>(r_value, ir_type, node->rhs_->result_var);
			current_block->instructions.emplace_back(load_instruction_r);
		} else {
			r_value = node->rhs_->result_var;
		}
		auto sub_instruction = std::make_shared<SubInstruction>(node->result_var, ir_type, l_value, r_value);
		current_block->instructions.emplace_back(sub_instruction);
	}
}

void IRBuilder::visit(MulDivModExpressionNode *node) {
    auto ir_type = ir_manager_.GetIRType(node->types[0]);
	auto result_value = std::make_shared<LocalVar>("", ir_type);
    if (node->lhs_) node->lhs_->accept(this);
    if (node->rhs_) node->rhs_->accept(this);
	auto l_value = std::make_shared<LocalVar>("", ir_type);
	auto r_value = std::make_shared<LocalVar>("", ir_type);
	auto ir_type_ptr = std::make_shared<IRPointerType>(ir_type);
	node->result_var = std::make_shared<LocalVar>("", ir_type_ptr);
	if (node->type_ == TokenType::Mul) {
		if (node->lhs_->is_assignable_) {
			auto load_instruction_l = std::make_shared<LoadInstruction>(l_value, ir_type, node->lhs_->result_var);
			current_block->instructions.emplace_back(load_instruction_l);
		} else {
			l_value = node->lhs_->result_var;
		}
		if (node->rhs_->is_assignable_) {
			auto load_instruction_r = std::make_shared<LoadInstruction>(r_value, ir_type, node->rhs_->result_var);
			current_block->instructions.emplace_back(load_instruction_r);
		} else {
			r_value = node->rhs_->result_var;
		}
		auto mul_instruction = std::make_shared<MulInstruction>(node->result_var, ir_type, l_value, r_value);
		current_block->instructions.emplace_back(mul_instruction);
	} else if (node->type_ == TokenType::Div) {
		if (node->lhs_->is_assignable_) {
			auto load_instruction_l = std::make_shared<LoadInstruction>(l_value, ir_type, node->lhs_->result_var);
			current_block->instructions.emplace_back(load_instruction_l);
		} else {
			l_value = node->lhs_->result_var;
		}
		if (node->rhs_->is_assignable_) {
			auto load_instruction_r = std::make_shared<LoadInstruction>(r_value, ir_type, node->rhs_->result_var);
			current_block->instructions.emplace_back(load_instruction_r);
		} else {
			r_value = node->rhs_->result_var;
		}
		auto sdiv_instruction = std::make_shared<SDivInstruction>(node->result_var, ir_type, l_value, r_value);
		current_block->instructions.emplace_back(sdiv_instruction);
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

	for (const auto& item: node->statements_->statements_) {
		auto vis_item_stmt = std::dynamic_pointer_cast<VisItemStatementNode>(item);
		if (!vis_item_stmt || !vis_item_stmt->vis_item_node_) continue;
		auto const_item = std::dynamic_pointer_cast<ConstantItemNode>(vis_item_stmt->vis_item_node_);
		if (const_item) {
			auto const_var = std::make_shared<ConstVar>(const_item->identifier_, std::make_shared<IRIntegerType>(32));
			auto val = std::get_if<int64_t>(&const_item->expression_node_->value);
			if (val) {
				auto ir_literal = std::make_shared<LiteralInt>(*val);
				ir_program->constants.emplace_back(std::make_shared<ConstVarDefInstruction>(const_var, ir_literal));
			}
		}
	}

	for (const auto& item: node->statements_->statements_) {
		auto vis_item_stmt = std::dynamic_pointer_cast<VisItemStatementNode>(item);
		if (!vis_item_stmt || !vis_item_stmt->vis_item_node_) continue;
		auto struct_item = std::dynamic_pointer_cast<StructNode>(vis_item_stmt->vis_item_node_);
		if (struct_item) {
			auto struct_type = scope_manager_.lookup(struct_item->identifier_).type_;
			ir_manager_.AddType(struct_type);
			auto ir_struct_type = std::dynamic_pointer_cast<IRStructType>(ir_manager_.GetIRType(struct_type));
			ir_program->structs.emplace_back(std::make_shared<StructDefInstruction>(ir_struct_type, ir_struct_type->members));
		}
	}

	for (const auto& item: node->statements_->statements_) {
		auto vis_item_stmt = std::dynamic_pointer_cast<VisItemStatementNode>(item);
		if (!vis_item_stmt || !vis_item_stmt->vis_item_node_) continue;
		auto func_item = std::dynamic_pointer_cast<FunctionNode>(vis_item_stmt->vis_item_node_);
		if (func_item) {
			std::shared_ptr<IRType> ir_ret_type = std::make_shared<IRVoidType>();
			if (func_item->type_) {
				ir_ret_type = ir_manager_.GetIRType(func_item->type_->type);
			}
			auto parameters = func_item->function_parameters_;
			std::vector<IRFunctionParam> ir_function_params;
			if (parameters) {
				for (auto& param : parameters->function_params_) {
					auto ir_type = ir_manager_.GetIRType(param->type_->type);
					auto identifier_pattern = std::dynamic_pointer_cast<IdentifierPatternNode>(param->pattern_no_top_alt_node_);
					ir_function_params.emplace_back(ir_type, identifier_pattern->identifier_);
				}
			}
			ir_program->functions.emplace_back(std::make_shared<IRFunction>(func_item->identifier_, ir_ret_type, ir_function_params));
		}
	}

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
    node->value = node->int_literal_;
	auto ir_i32_type = std::make_shared<IRIntegerType>(32);
	node->result_var = std::make_shared<LocalVar>("", ir_i32_type);
	auto add_instruction = std::make_shared<AddInstruction>(node->result_var, ir_i32_type, node->int_literal_, 0);
	current_block->instructions.emplace_back(add_instruction);
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
    	std::string name = node->path_indent_segments_[0]->identifier_;
    	Symbol sym = scope_manager_.lookup(name);
    	if (sym.symbol_type_ == SymbolType::Variable) {
    		auto ir_type = ir_manager_.GetIRType(node->types[0]);
    		node->result_var = std::make_shared<LocalVar>(name, ir_type, false);
    	}
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
    node -> type = std::make_shared<ArrayType>(base_type, size);
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