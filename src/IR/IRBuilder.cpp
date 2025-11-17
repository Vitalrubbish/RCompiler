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
    scope_manager_.current_scope = scope_manager_.scope_set_[0];
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

			for (auto& method : struct_type->methods_) {
				auto function_type = std::dynamic_pointer_cast<FunctionType>(method.type_);
				std::vector<IRFunctionParam> ir_function_params;
				if (function_type) {
					std::shared_ptr<IRType> ir_ret_type = std::make_shared<IRVoidType>();
					if (function_type->ret_) {
						ir_ret_type = ir_manager_.GetIRType(function_type->ret_);
					}
					if (function_type->have_self_) {
						auto ir_pointer_struct_type = std::make_shared<IRPointerType>(ir_struct_type);
						auto ir_var = std::make_shared<LocalVar>("self", ir_pointer_struct_type);
						ir_function_params.emplace_back(ir_pointer_struct_type, ir_var);
					}
					uint32_t index = 0; // 开始为 semantic_function_type 没有存 identifier 买单了
					for (auto& param: function_type->params_) {
						auto ir_type = ir_manager_.GetIRType(param);
						auto ir_var = std::make_shared<LocalVar>("member" + std::to_string(index), ir_type);
						ir_function_params.emplace_back(ir_type, ir_var);
						++index;
					}
					std::string ir_identifier = struct_item->identifier_ + "." + method.name_;
					auto ir_function = std::make_shared<IRFunction>(ir_identifier, ir_ret_type, ir_function_params);
					ir_program->functions.emplace_back(ir_function);
					ir_manager_.function_map_[ir_identifier] = ir_function;
					method.function_node_->identifier_ = ir_identifier;
					method.function_node_->accept(this);
				}
			}
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
					auto ir_var = std::make_shared<LocalVar>(identifier_pattern->identifier_, ir_type);
					ir_function_params.emplace_back(ir_type, ir_var);
				}
			}
			auto ir_function = std::make_shared<IRFunction>(func_item->identifier_, ir_ret_type, ir_function_params);
			ir_program->functions.emplace_back(ir_function);
			ir_manager_.function_map_[func_item->identifier_] = ir_function;
			item->accept(this);
		}
	}
}

void IRBuilder::visit(VisItemNode *node) {
}

void IRBuilder::visit(FunctionNode *node) {
	auto saved_current_function = current_function;
	auto saved_current_block = current_block;
	current_function = ir_manager_.function_map_[node->identifier_];
	current_block = std::make_shared<IRBasicBlock>("entry");
	current_function->blocks.emplace_back(current_block);
    if (node->function_parameters_) {
	    node->function_parameters_->accept(this);
    	for (auto& param: current_function->function_params) {
    		auto param_var = std::make_shared<LocalVar>(param.var->name, std::make_shared<IRPointerType>(param.type));
    		current_block->instructions.emplace_back(std::make_shared<AllocaInstruction>(param_var, param.type));
    		current_block->instructions.emplace_back(std::make_shared<StoreInstruction>(param.type, param.var, param_var));
    	}
    }
    if (node->type_) node->type_->accept(this);
    if (node->block_expression_) {
    	node->block_expression_->is_function_direct_block = true;
        node->block_expression_->accept(this);
    }
	current_function = saved_current_function;
	current_block = saved_current_block;
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
	scope_manager_.current_scope = scope_manager_.scope_set_[node->scope_index];
    if (node->type_node_) node->type_node_->accept(this);
    std::string name = node->type_node_->toString();
    for (auto& item : node->associated_item_nodes_) {
        if (item) {
            if (item->function_node_) {
            	auto function_node = item->function_node_;
            	item->function_node_->identifier_ = name + "." + item->function_node_->identifier_;
            	item->accept(this);
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

	/**** Handle Array Type ****/
	auto array_type = std::dynamic_pointer_cast<IRArrayType>(ir_type);
	if (array_type) {
		StoreArrayLiteral(node->expression_, ir_var, array_type);
		return;
	}

	/**** Handle Basic And Sturct Types ****/
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
	if (node->expression_) {
		node->expression_->accept(this);
		auto ir_type = ir_manager_.GetIRType(node->type_->type);
		node->result_var = std::make_shared<LocalVar>("", ir_type);
		if (node->expression_->is_assignable_) {
			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(node->result_var, ir_type, node->expression_->result_var));
		} else {
			node->result_var = node->expression_->result_var;
		}
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
	if (node->type_ == TokenType::PlusEq) {
		auto left_value = std::make_shared<LocalVar>("", ir_type);
		auto result_value = std::make_shared<LocalVar>("", ir_type);
		if (node->lhs_->is_assignable_) {
			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(left_value, ir_type, node->lhs_->result_var));
		} else {
			left_value = node->lhs_->result_var;
		}
		current_block->instructions.emplace_back(std::make_shared<AddInstruction>(result_value, ir_type, left_value, ir_var));
	}
	if (node->type_ == TokenType::MinusEq) {
		auto left_value = std::make_shared<LocalVar>("", ir_type);
		auto result_value = std::make_shared<LocalVar>("", ir_type);
		if (node->lhs_->is_assignable_) {
			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(left_value, ir_type, node->lhs_->result_var));
		} else {
			left_value = node->lhs_->result_var;
		}
		current_block->instructions.emplace_back(std::make_shared<SubInstruction>(result_value, ir_type, left_value, ir_var));
	}
	if (node->type_ == TokenType::MulEq) {
		auto left_value = std::make_shared<LocalVar>("", ir_type);
		auto result_value = std::make_shared<LocalVar>("", ir_type);
		if (node->lhs_->is_assignable_) {
			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(left_value, ir_type, node->lhs_->result_var));
		} else {
			left_value = node->lhs_->result_var;
		}
		current_block->instructions.emplace_back(std::make_shared<MulInstruction>(result_value, ir_type, left_value, ir_var));
	}
	if (node->type_ == TokenType::DivEq) {
		auto left_value = std::make_shared<LocalVar>("", ir_type);
		auto result_value = std::make_shared<LocalVar>("", ir_type);
		if (node->lhs_->is_assignable_) {
			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(left_value, ir_type, node->lhs_->result_var));
		} else {
			left_value = node->lhs_->result_var;
		}
		current_block->instructions.emplace_back(std::make_shared<SDivInstruction>(result_value, ir_type, left_value, ir_var));
	}
	current_block->instructions.emplace_back(std::make_shared<StoreInstruction>(ir_type, ir_var, node->lhs_->result_var));
}

void IRBuilder::visit(ContinueExpressionNode *node) {
}

void IRBuilder::visit(UnderscoreExpressionNode *node) {
}

void IRBuilder::visit(JumpExpressionNode *node) {
    if (node->expression_) {
        node->expression_->accept(this);
    }
	if (node->type_ == TokenType::Return) {
		interrupt = true;
		if (node->expression_) {
			auto ret_type = ir_manager_.GetIRType(node->expression_->types[0]);
			auto value = std::make_shared<LocalVar>("", ret_type);
			if (node->expression_->is_assignable_) {
				current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(value, ret_type, node->expression_->result_var));
			} else {
				value = node->expression_->result_var;
			}
			current_block->instructions.emplace_back(std::make_shared<RetInstruction>(ret_type, value));
		} else {
			auto ret_type = std::make_shared<IRVoidType>();
			current_block->instructions.emplace_back(std::make_shared<RetInstruction>(ret_type, nullptr));
		}
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
    	auto value_type = ir_manager_.GetIRType(node->expression_->types[0]);
        if (node->type_ == TokenType::Minus) {
        	node->result_var = std::make_shared<LocalVar>("", value_type);
            auto value = std::make_shared<LocalVar>("", value_type);
        	if (node->expression_->is_assignable_) {
        		current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(value, value_type, node->expression_->result_var));
        	} else {
        		value = node->expression_->result_var;
        	}
        	current_block->instructions.emplace_back(std::make_shared<SubInstruction>(node->result_var, value_type, value));
        }
    	if (node->type_ == TokenType::Mul) {
    		node->result_var = std::make_shared<LocalVar>("", value_type);
    		current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(node->result_var, value_type, node->expression_->result_var));
    	}
    	if (node->type_ == TokenType::And || node->type_ == TokenType::AndMut) {
    		node->result_var = node->expression_->result_var;
    	}
    }
}

void IRBuilder::visit(FunctionCallExpressionNode *node) {
	std::shared_ptr<FunctionType> function_type = nullptr;
	std::string function_name; // We now only consider basic function call.
    if (node->callee_) {
        node->callee_->accept(this);
    	function_type = std::dynamic_pointer_cast<FunctionType>(node->callee_->types[0]);
    	auto identifier_pattern = std::dynamic_pointer_cast<PathInExpressionNode>(node->callee_);
    	if (identifier_pattern) {
    		function_name = identifier_pattern->path_indent_segments_[0]->identifier_;
    	}
    }
	std::vector<std::shared_ptr<IRVar>> args;
	std::vector<std::shared_ptr<IRType>> arg_types;
	uint32_t index = 0;
    for (const auto &param: node->params_) {
    	auto ir_type = ir_manager_.GetIRType(function_type->params_[index]);
        if (param) {
            param->accept(this);
        	auto ir_var = std::make_shared<LocalVar>("", ir_type);
        	if (param->is_assignable_) {
        		current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(ir_var, ir_type, param->result_var));
        	} else {
        		ir_var = param->result_var;
        	}
        	args.emplace_back(ir_var);
        	arg_types.emplace_back(ir_type);
        }
    	++index;
    }
	if (function_type->ret_) {
		auto semantic_void_type = scope_manager_.lookup("void").type_;
		auto ir_ret_type = ir_manager_.GetIRType(function_type->ret_);
		if (function_type->ret_->equal(semantic_void_type)) {
			current_block->instructions.emplace_back(std::make_shared<CallWithoutRetInstruction>(function_name, args, arg_types));
		} else {
			node->result_var = std::make_shared<LocalVar>("", ir_ret_type);
			current_block->instructions.emplace_back(std::make_shared<CallWithRetInstruction>(node->result_var, ir_ret_type, function_name, args, arg_types));
		}
	}
}

void IRBuilder::visit(ArrayIndexExpressionNode *node) {
	if (node->base_) {
		node->base_->accept(this);
	}
	if (node->index_) {
		node->index_->accept(this);
	}
    std::shared_ptr<IRArrayType> type;
	auto ir_type = ir_manager_.GetIRType(node->types[0]);
	node->result_var = std::make_shared<LocalVar>("", std::make_shared<IRPointerType>(ir_type));
	auto ir_i32_type = std::make_shared<IRIntegerType>(32);
	std::vector<std::shared_ptr<IRType>> index_type({ir_i32_type});
	auto index_var = std::make_shared<LocalVar>("", ir_i32_type);
	if (node->index_->is_assignable_) {
		current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(index_var, ir_i32_type, node->index_->result_var));
	} else {
		index_var = node->index_->result_var;
	}
	std::vector<std::shared_ptr<IRVar>> index_value({node->index_->result_var});
	current_block->instructions.emplace_back(std::make_shared<GetElementPtrInstruction>(node->result_var, ir_type, index_var, index_type, index_value));
}

void IRBuilder::visit(MemberAccessExpressionNode *node) {
    if (node->base_) {
        node->base_->accept(this);
    }
	auto type = node->base_->types[0];
	std::string identifier = node->member_.token;
	auto struct_type = std::dynamic_pointer_cast<StructType>(type);
	if (struct_type) {
		for (uint32_t i = 0; i < struct_type->members_.size(); i++) {
			if (struct_type->members_[i].name_ == identifier) {
				auto ir_i32_type = std::make_shared<IRIntegerType>(32);
				auto ir_type = ir_manager_.GetIRType(struct_type->members_[i].type_);
				std::vector<std::shared_ptr<IRType>> index_type({ir_i32_type});
				std::vector<std::shared_ptr<IRLiteral>> index_value({std::make_shared<LiteralInt>(i)});
				node->result_var = std::make_shared<LocalVar>("", std::make_shared<IRPointerType>(ir_type));
				current_block->instructions.emplace_back(std::make_shared<GetElementPtrInstruction>(node->result_var,
					ir_type, node->base_->result_var, index_type, index_value));
			}
		}
	}
}

void IRBuilder::visit(BlockExpressionNode *node) {
    scope_manager_.current_scope = scope_manager_.scope_set_[node->scope_index];

	if (node->statements_) {
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
				for (auto& method : struct_type->methods_) {
					auto function_type = std::dynamic_pointer_cast<FunctionType>(method.type_);
					std::vector<IRFunctionParam> ir_function_params;
					if (function_type) {
						std::shared_ptr<IRType> ir_ret_type = std::make_shared<IRVoidType>();
						if (function_type->ret_) {
							ir_ret_type = ir_manager_.GetIRType(function_type->ret_);
						}
						if (function_type->have_self_) {
							auto ir_pointer_struct_type = std::make_shared<IRPointerType>(ir_struct_type);
							auto ir_var = std::make_shared<LocalVar>("self_ptr", ir_pointer_struct_type);
							ir_function_params.emplace_back(ir_pointer_struct_type, ir_var);
						}
						for (auto& param: function_type->params_) {
							auto ir_type = ir_manager_.GetIRType(param);
							auto ir_var = std::make_shared<LocalVar>("member", ir_type);
							ir_function_params.emplace_back(ir_type, ir_var);
						}
						std::string ir_identifier = struct_item->identifier_ + "." + method.name_;
						auto ir_function = std::make_shared<IRFunction>(ir_identifier, ir_ret_type, ir_function_params);
						ir_program->functions.emplace_back(ir_function);
						ir_manager_.function_map_[ir_identifier] = ir_function;
					}
				}
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
						auto ir_var = std::make_shared<LocalVar>(identifier_pattern->identifier_, ir_type);
						ir_function_params.emplace_back(ir_type, ir_var);
					}
				}
				auto ir_function = std::make_shared<IRFunction>(func_item->identifier_, ir_ret_type, ir_function_params);
				ir_program->functions.emplace_back(ir_function);
				ir_manager_.function_map_[func_item->identifier_] = ir_function;
			}
		}
	}

	auto ir_type = ir_manager_.GetIRType(node->types[0]);
	node->result_var = std::make_shared<LocalVar>("", ir_type);
	if (node->statements_) {
        node->statements_->accept(this);
		if (node->statements_->expression_) {
			auto trailing_expression = node->statements_->expression_;
			if (trailing_expression->is_assignable_) {
				current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(node->result_var, ir_type, trailing_expression->result_var));
			} else {
				node->result_var = trailing_expression->result_var;
			}
		}
    }
	if (node->is_function_direct_block && ir_type) {
		auto ir_void_type = std::dynamic_pointer_cast<IRVoidType>(ir_type);
		if (ir_void_type) {
			current_block->instructions.emplace_back(std::make_shared<RetInstruction>(ir_type, nullptr));
		} else {
			current_block->instructions.emplace_back(std::make_shared<RetInstruction>(ir_type, node->result_var));
		}
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
	auto condition_block = std::make_shared<IRBasicBlock>("condition");
	auto body_block = std::make_shared<IRBasicBlock>("body");
	auto combine_block = std::make_shared<IRBasicBlock>("combine");
    if (node->conditions_) {
    	current_block->instructions.emplace_back(std::make_shared<UnconditionalBrInstruction>(condition_block->true_label));
    	current_block = condition_block;
	    node->conditions_->accept(this);
    	auto cmp_expr = std::dynamic_pointer_cast<ComparisonExpressionNode>(node->conditions_->expression_);
    	if (cmp_expr) {
    		auto ir_bool_type = std::make_shared<IRIntegerType>(1);
    		auto compare_type = ir_manager_.GetIRType(cmp_expr->rhs_->types[0]);
    		cmp_expr->result_var = std::make_shared<LocalVar>("", ir_bool_type);
    		auto lhs_value = std::make_shared<LocalVar>("", compare_type);
    		if (cmp_expr->lhs_->is_assignable_) {
    			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(lhs_value, compare_type, cmp_expr->lhs_->result_var));
    		} else {
    			lhs_value = cmp_expr->lhs_->result_var;
    		}
    		auto rhs_value = std::make_shared<LocalVar>("", compare_type);
    		if (cmp_expr->rhs_->is_assignable_) {
    			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(rhs_value, compare_type, cmp_expr->rhs_->result_var));
    		} else {
    			rhs_value = cmp_expr->rhs_->result_var;
    		}
    		if (cmp_expr->type_ == TokenType::EqEq) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::eq));
    		} else if (cmp_expr->type_ == TokenType::LEq) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::sle));
    		} else if (cmp_expr->type_ == TokenType::Lt) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::slt));
    		} else if (cmp_expr->type_ == TokenType::GEq) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::sge));
    		} else if (cmp_expr->type_ == TokenType::Gt) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::sgt));
    		} else if (cmp_expr->type_ == TokenType::NEq) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::ne));
    		}
    		current_block->instructions.emplace_back(std::make_shared<ConditionalBrInstruction>(cmp_expr->result_var, body_block->true_label, combine_block->true_label));
    	}
    }
    if (node->block_expression_) {
    	current_block = body_block;
        node->block_expression_->accept(this);
    	current_block->instructions.emplace_back(std::make_shared<UnconditionalBrInstruction>(condition_block->true_label));
    }
	current_block = combine_block;
	current_function->blocks.emplace_back(condition_block);
	current_function->blocks.emplace_back(body_block);
	current_function->blocks.emplace_back(combine_block);
}

void IRBuilder::visit(IfExpressionNode *node) {
	auto if_true_block = std::make_shared<IRBasicBlock>("if_true");
	auto if_false_block = std::make_shared<IRBasicBlock>("if_false");
	auto combine_block = std::make_shared<IRBasicBlock>("combine");
    if (node->conditions_) {
	    node->conditions_->accept(this);
    	auto cmp_expr = std::dynamic_pointer_cast<ComparisonExpressionNode>(node->conditions_->expression_);
    	if (cmp_expr) {
    		auto ir_bool_type = std::make_shared<IRIntegerType>(1);
    		auto compare_type = ir_manager_.GetIRType(cmp_expr->rhs_->types[0]);
    		cmp_expr->result_var = std::make_shared<LocalVar>("", ir_bool_type);
    		auto lhs_value = std::make_shared<LocalVar>("", compare_type);
    		if (cmp_expr->lhs_->is_assignable_) {
    			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(lhs_value, compare_type, cmp_expr->lhs_->result_var));
    		} else {
    			lhs_value = cmp_expr->lhs_->result_var;
    		}
    		auto rhs_value = std::make_shared<LocalVar>("", compare_type);
    		if (cmp_expr->rhs_->is_assignable_) {
    			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(rhs_value, compare_type, cmp_expr->rhs_->result_var));
    		} else {
    			rhs_value = cmp_expr->rhs_->result_var;
    		}
    		if (cmp_expr->type_ == TokenType::EqEq) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::eq));
    		} else if (cmp_expr->type_ == TokenType::LEq) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::sle));
    		} else if (cmp_expr->type_ == TokenType::Lt) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::slt));
    		} else if (cmp_expr->type_ == TokenType::GEq) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::sge));
    		} else if (cmp_expr->type_ == TokenType::Gt) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::sgt));
    		} else if (cmp_expr->type_ == TokenType::NEq) {
    			current_block->instructions.emplace_back(std::make_shared<ICmpInstruction>(cmp_expr->result_var, compare_type, lhs_value, rhs_value, ConditionType::ne));
    		}
    		current_function->blocks.emplace_back(if_true_block);
    		current_function->blocks.emplace_back(if_false_block);
    		current_function->blocks.emplace_back(combine_block);
    		current_block->instructions.emplace_back(std::make_shared<ConditionalBrInstruction>(cmp_expr->result_var, if_true_block->true_label, if_false_block->true_label));
    	}
    }

	current_block = if_true_block;
    if (node->true_block_expression_) {
	    node->true_block_expression_->accept(this);
    	if (!interrupt) {
    		current_block->instructions.emplace_back(std::make_shared<UnconditionalBrInstruction>(combine_block->true_label));
    	}
    	interrupt = false;
    }
	current_block = if_false_block;
	if (node->false_block_expression_) node->false_block_expression_->accept(this);
    if (node->if_expression_) {
	    node->if_expression_->accept(this);
    }
	if (!interrupt) {
		current_block->instructions.emplace_back(std::make_shared<UnconditionalBrInstruction>(combine_block->true_label));
	}
	interrupt = false;
	current_block = combine_block;
	if (node->false_block_expression_) {
		if (node->true_block_expression_->result_var && node->false_block_expression_->result_var) {
			std::shared_ptr<IRVar> true_value = node->true_block_expression_->result_var;
			std::shared_ptr<IRVar> false_value = node->false_block_expression_->result_var;
			auto ir_type = ir_manager_.GetIRType(node->types[0]);
			if (ir_type) {
				node->result_var = std::make_shared<LocalVar>("", ir_type);
				std::vector value_table{true_value, false_value};
				current_block->instructions.emplace_back(std::make_shared<PhiInstruction>(node->result_var, ir_type,
					value_table, std::vector{if_true_block->true_label, if_false_block->true_label}));
			}
		}
	}
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
	if (node->lhs_) {
		node->lhs_->accept(this);
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
	node->is_assignable_ = true;
    if (node->path_in_expression_node_) {
        node->path_in_expression_node_->accept(this);
    }
	auto struct_type = std::dynamic_pointer_cast<IRStructType>(ir_manager_.GetIRType(node->types[0]));
	node->result_var = std::make_shared<LocalVar>("", std::make_shared<IRPointerType>(struct_type));
	current_block->instructions.emplace_back(std::make_shared<AllocaInstruction>(node->result_var, struct_type));
	if (node->struct_expr_fields_node_) {
	    node->struct_expr_fields_node_->accept(this);
    	auto struct_expr_field_nodes = node->struct_expr_fields_node_->struct_expr_field_nodes_;
    	uint32_t index = 0;
    	for (const auto& field: struct_expr_field_nodes) {
    		std::shared_ptr<IRType> index_type = std::make_shared<IRIntegerType>(32);
    		std::shared_ptr<IRLiteral> index_value = std::make_shared<LiteralInt>(index);
    		auto element_type = struct_type->members[index];
    		auto local_ptr = std::make_shared<LocalVar>("", std::make_shared<IRPointerType>(element_type));
    		current_block->instructions.emplace_back(std::make_shared<GetElementPtrInstruction>
    			(local_ptr, element_type, node->result_var, std::vector{index_type}, std::vector{index_value}));
    		auto element_value = std::make_shared<LocalVar>("", element_type);
    		if (field->expression_node_->is_assignable_) {
    			current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(element_value, element_type, field->expression_node_->result_var));
    		} else {
    			element_value = field->expression_node_->result_var;
    		}
    		current_block->instructions.emplace_back(std::make_shared<StoreInstruction>(element_type, element_value, local_ptr));
    		index++;
    	}
    }
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
        node->types = node->expression_->types;
    	auto ir_type = ir_manager_.GetIRType(node->types[0]);
    	node->result_var = std::make_shared<LocalVar>("", ir_type);
    	if (node->expression_->is_assignable_) {
    		current_block->instructions.emplace_back(std::make_shared<LoadInstruction>(node->result_var, ir_type, node->expression_->result_var));
    	} else {
    		node->result_var = node->expression_->result_var;
    	}
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

/**************** Supporting Functions ****************/
void IRBuilder::StoreArrayLiteral(const std::shared_ptr<ExpressionNode>& expr_node, const std::shared_ptr<LocalVar>& array_var,
                                  const std::shared_ptr<IRArrayType>& array_type) {
	auto array_literal = std::dynamic_pointer_cast<ArrayLiteralNode>(expr_node);
	if (array_literal) {
		if (!array_literal->expressions_.empty()) {
			uint32_t cnt = 0;
			for (const auto& expr : array_literal->expressions_) {
				auto ir_element_type = array_type->baseType;
				auto element_var = std::make_shared<LocalVar>("", ir_element_type);
				std::vector index_types({ir_element_type});
				std::vector<std::shared_ptr<IRLiteral>> index_values({std::make_shared<LiteralInt>(cnt)});
				auto get_element_ptr_inst = std::make_shared<GetElementPtrInstruction>(element_var, ir_element_type, array_var, index_types, index_values);
				current_block->instructions.emplace_back(get_element_ptr_inst);
				auto possible_array_type = std::dynamic_pointer_cast<IRArrayType>(ir_element_type);
				if (possible_array_type) {
					StoreArrayLiteral(expr, element_var, possible_array_type);
				} else {
					current_block->instructions.emplace_back(std::make_shared<StoreInstruction>(ir_element_type, expr->result_var, element_var));
				}
				cnt++;
			}
		}
		if (array_literal->lhs_ && array_literal->rhs_) {
			auto value = *std::get_if<int64_t>(&array_literal->rhs_->value);
			for (uint32_t i = 0; i < value; i++) {
				auto ir_element_type = array_type->baseType;
				auto element_var = std::make_shared<LocalVar>("", ir_element_type);
				std::vector index_types({ir_element_type});
				std::vector<std::shared_ptr<IRLiteral>> index_values({std::make_shared<LiteralInt>(i)});
				auto get_element_ptr_inst = std::make_shared<GetElementPtrInstruction>(element_var, ir_element_type, array_var, index_types, index_values);
				current_block->instructions.emplace_back(get_element_ptr_inst);
				auto possible_array_type = std::dynamic_pointer_cast<IRArrayType>(ir_element_type);
				if (possible_array_type) {
					StoreArrayLiteral(array_literal->lhs_, element_var, possible_array_type);
				} else {
					current_block->instructions.emplace_back(std::make_shared<StoreInstruction>(ir_element_type, array_literal->lhs_->result_var, element_var));
				}
			}
		}
	}
}