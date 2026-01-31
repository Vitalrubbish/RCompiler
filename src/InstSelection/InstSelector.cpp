#include "InstSelection/InstSelector.h"

#include "IR/IRProgram.h"
#include "IR/IRFunction.h"
#include "IR/IRBlock.h"
#include "IR/IRInstruction.h"


std::shared_ptr<Register> InstSelector::new_vreg() {
    return std::make_shared<Register>(virt_reg_cnt++, false);
}

std::shared_ptr<Register> InstSelector::get_operand(const std::shared_ptr<IRVar> &var) {
    if (var_map.find(var->true_name) == var_map.end()) {
        var_map[var->true_name] = new_vreg();
        if (var->var_type == VarType::Global || std::dynamic_pointer_cast<ConstVar>(var)) {
            if (cur_block) {
                cur_block->AddInstruction(std::make_shared<ASMLaInstruction>(
                    var_map[var->true_name], var->true_name
                ));
            }
        }
    }
    return var_map[var->true_name];
}

std::shared_ptr<Register> InstSelector::get_temp_operand() {
	return new_vreg();
}

void InstSelector::visit(IRProgram *node) {
    for (auto &global : node->globals) {
        global->accept(this);
    }
    for (auto &konst : node->constants) {
        konst->accept(this);
    }
    for (auto &struc : node->structs) {
        struc->accept(this);
    }
    for (auto &function : node->functions) {
        function->accept(this);
    }
}

void InstSelector::visit(IRFunction *node) {
    cur_func = std::make_shared<ASMFunction>(node->name);
    asm_functions.push_back(cur_func);
    block_map.clear();
    var_map.clear();
    pending_phi_copies.clear();

    for (auto &block : node->blocks) {
        block->accept(this);
    }
}

void InstSelector::visit(IRFunctionParam *node) {
    // Empty implementation
}

void InstSelector::visit(IRBasicBlock *node) {
    cur_block = std::make_shared<ASMBlock>(node->true_label);
    cur_func->AddBlock(cur_block);
    block_map[node->true_label] = cur_block;
    for (auto &inst : node->instructions) {
        inst->accept(this);
    }
}

void InstSelector::visit(IRInstruction *node) {
    // Empty implementation
}

void InstSelector::visit(UnreachableInstruction *node) {}
void InstSelector::visit(AllocaInstruction *node) {
	cur_func->AddStackObject(node->type->size);
	cur_block->AddInstruction(
		std::make_shared<ASMAddiInstruction>(
			get_operand(node->result),
			std::make_shared<Register>(2, true), // sp is physical register 2
			std::make_shared<Immediate>(0) // offset will be set in later stack allocation phase
		)
	);
}

void InstSelector::visit(LoadInstruction *node) {
	cur_block->AddInstruction(
		std::make_shared<ASMLwInstruction>(
			get_operand(node->result),
			get_operand(node->ptr),
			std::make_shared<Immediate>(0)
		)
	);
}
void InstSelector::visit(StoreInstruction *node) {
	auto rs2 = node->var ? get_operand(node->var) : get_temp_operand();
	if (node->literal) {
		auto value = std::dynamic_pointer_cast<LiteralInt>(node->literal)->getValue();
		cur_block->AddInstruction(
			std::make_shared<ASMAddiInstruction>(
				rs2,
				std::make_shared<Register>(0, true), // x0 is physical register 0
				std::make_shared<Immediate>(value)
			)
		);
	}

	cur_block->AddInstruction(
			std::make_shared<ASMSwInstruction>(
				rs2,
				get_operand(node->ptr),
				std::make_shared<Immediate>(0)
			)
		);
}
void InstSelector::visit(GetElementPtrInstruction *node) {
	auto rd = get_operand(node->result);
	auto base_ptr = get_operand(node->ptr);

	if (!node->literals.empty()) {
		for (uint32_t i = 0; i < node->literals.size(); i++) {
			auto index = std::dynamic_pointer_cast<LiteralInt>(node->literals[i])->getValue();
			auto offset = node->types[i]->size * index;

			auto temp = get_temp_operand();
			cur_block->AddInstruction(
				std::make_shared<ASMAddiInstruction>(
					temp,
					base_ptr,
					std::make_shared<Immediate>(offset)
				)
			);
			base_ptr = temp;
		}
	} else {
		for (uint32_t i = 0; i < node->indexes.size(); i++) {
			auto index_reg = get_operand(node->indexes[i]);
			auto offset_reg = get_temp_operand();
			auto imm_reg = std::make_shared<Immediate>(0);

			cur_block->AddInstruction(
					std::make_shared<ASMAddiInstruction>(
						imm_reg,
						std::make_shared<Register>(0, true), // x0 is physical register 0
						std::make_shared<Immediate>(node->types[i]->size)
				)
			);

			cur_block->AddInstruction(
				std::make_shared<ASMMulInstruction>(
					offset_reg,
					index_reg,
					imm_reg
				)
			);

			auto temp = get_temp_operand();
			cur_block->AddInstruction(
				std::make_shared<ASMAddInstruction>(
					temp,
					base_ptr,
					offset_reg
				)
			);
			base_ptr = temp;
		}
	}
}

void InstSelector::visit(AddInstruction *node) {
    auto rd = get_operand(node->result);

    if (node->op1 && node->op2) {
        auto rs1 = get_operand(node->op1);
        auto rs2 = get_operand(node->op2);
        auto inst = std::make_shared<ASMAddInstruction>(rd, rs1, rs2);
        cur_block->AddInstruction(inst);
    } else if (!node->op1 && !node->op2) {
        int val = node->imm1 + node->imm2;
        auto x0 = std::make_shared<Register>(0, true); // x0 is physical register 0
        auto imm = std::make_shared<Immediate>(val);
        auto inst = std::make_shared<ASMAddiInstruction>(rd, x0, imm);
        cur_block->AddInstruction(inst);
    }
}

void InstSelector::visit(SubInstruction *node) {
	auto rd = get_operand(node->result);

	if (node->op1 && node->op2) {
		auto rs1 = get_operand(node->op1);
		auto rs2 = get_operand(node->op2);
		auto inst = std::make_shared<ASMSubInstruction>(rd, rs1, rs2);
		cur_block->AddInstruction(inst);
	} else if (!node->op1) {
		auto x0 = std::make_shared<Register>(0, true); // x0 is physical register 0
		auto rs2 = get_operand(node->op2);
		auto inst = std::make_shared<ASMSubInstruction>(rd, x0, rs2);
		cur_block->AddInstruction(inst);
	}
}

void InstSelector::visit(MulInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMMulInstruction>(rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(SDivInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMDivInstruction> (rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(UDivInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMDivuInstruction> (rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(SremInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMRemInstruction> (rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(UremInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMRemuInstruction> (rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(ShlInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMSllInstruction> (rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(AShrInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMSraInstruction>(rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(AndInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMAndInstruction>(rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}
void InstSelector::visit(OrInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMOrInstruction>(rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(XorInstruction *node) {
	auto rd = get_operand(node->result);
	auto rs1 = get_operand(node->op1);
	auto rs2 = get_operand(node->op2);
	auto inst = std::make_shared<ASMXorInstruction>(rd, rs1, rs2);
	cur_block->AddInstruction(inst);
}

void InstSelector::visit(RetInstruction *node) {
	if (node->var) {
		auto rd = std::make_shared<Register>(10, true); // a0 is physical register 10
		auto rs = get_operand(node->var);
		cur_block->AddInstruction(
			std::make_shared<ASMAddInstruction>(
				rd,
				rs,
				std::make_shared<Register>(0, true) // x0 is physical register 0
			)
		);
	}

	cur_block->AddInstruction(
		std::make_shared<ASMLwInstruction>(
			std::make_shared<Register>(1, true), // ra is physical register 1
			std::make_shared<Register>(2, true), // sp is physical register 2
			std::make_shared<Immediate>(0)
		)
	);

	cur_block->AddInstruction(
		std::make_shared<ASMLwInstruction>(
			std::make_shared<Register>(8, true), // s0 is physical register 8
			std::make_shared<Register>(2, true), // sp is physical register 2
			std::make_shared<Immediate>(0)
		)
	);

	cur_block->AddInstruction(
		std::make_shared<ASMAddiInstruction>(
			std::make_shared<Register>(2, true), // sp is physical register 2
			std::make_shared<Register>(2, true), // sp is physical register 2
			std::make_shared<Immediate>(0)
		)
	);

	cur_block->AddInstruction(
		std::make_shared<ASMRetInstruction>()
	);
}

void InstSelector::visit(ConditionalBrInstruction *node) {
    if (pending_phi_copies.count(cur_block->label)) {
        for (auto &pair : pending_phi_copies[cur_block->label]) {
            cur_block->AddInstruction(std::make_shared<ASMAddiInstruction>(
                pair.first, pair.second, std::make_shared<Immediate>(0)));
        }
    }
	if (node->condition) {
		auto rs = get_operand(node->condition);
		cur_block->AddInstruction(
			std::make_shared<ASMBeqInstruction>(
				rs,
				std::make_shared<Register>(0, true), // x0 is physical register 0
				std::make_shared<Label>(node->if_false)
			)
		);
		cur_block->AddInstruction(
			std::make_shared<ASMJalInstruction>(
				std::make_shared<Register>(0, true), // x0 is physical register 0
				std::make_shared<Label>(node->if_true)
			)
		);
	}
}
void InstSelector::visit(UnconditionalBrInstruction *node) {
    if (pending_phi_copies.count(cur_block->label)) {
        for (auto &pair : pending_phi_copies[cur_block->label]) {
            cur_block->AddInstruction(std::make_shared<ASMAddiInstruction>(
                pair.first, pair.second, std::make_shared<Immediate>(0)));
        }
    }
	cur_block->AddInstruction(
		std::make_shared<ASMJalInstruction>(
			std::make_shared<Register>(0, true), // x0 is physical register 0
			std::make_shared<Label>(node->label)
		)
	);
}
void InstSelector::visit(ICmpInstruction *node) {
	if (node->condition_type == ConditionType::eq) {
		auto rs = get_temp_operand();
		cur_block->AddInstruction(
			std::make_shared<ASMSubInstruction>(
				rs,
				get_operand(node->op1),
				get_operand(node->op2)
			)
		);
		cur_block->AddInstruction(
			std::make_shared<ASMSeqzInstruction>(
				get_operand(node->result),
				rs
			)
		);
	}
	if (node->condition_type == ConditionType::ne) {
		auto rs = get_temp_operand();
		cur_block->AddInstruction(
			std::make_shared<ASMSubInstruction>(
				rs,
				get_operand(node->op1),
				get_operand(node->op2)
			)
		);
		cur_block->AddInstruction(
			std::make_shared<ASMSnezInstruction>(
				get_operand(node->result),
				rs
			)
		);
	}
	if (node->condition_type == ConditionType::slt) {
		cur_block->AddInstruction(
			std::make_shared<ASMSltInstruction>(
				get_operand(node->result),
				get_operand(node->op1),
				get_operand(node->op2)
			)
		);
	}
	if (node->condition_type == ConditionType::sgt) {
		cur_block->AddInstruction(
			std::make_shared<ASMSltInstruction>(
				get_operand(node->result),
				get_operand(node->op2),
				get_operand(node->op1)
			)
		);
	}
	if (node->condition_type == ConditionType::sle) {
		auto rs = get_temp_operand();
		cur_block->AddInstruction(std::make_shared<ASMSltInstruction>(rs,
			get_operand(node->op2),
			get_operand(node->op1)
		));
		cur_block->AddInstruction(std::make_shared<ASMXoriInstruction>(get_operand(node->result), rs,std::make_shared<Immediate>(1)));
	}
	if (node->condition_type == ConditionType::sge) {
		auto rs = get_temp_operand();
		cur_block->AddInstruction(std::make_shared<ASMSltInstruction>(rs,
			get_operand(node->op1),
			get_operand(node->op2)
		));
		cur_block->AddInstruction(std::make_shared<ASMXoriInstruction>(get_operand(node->result), rs,std::make_shared<Immediate>(1)));
	}
}

void InstSelector::visit(CallWithRetInstruction *node) {
	for (size_t i = 0; i < node->args.size(); i++) {
		auto arg_reg = std::make_shared<Register>(10 + i, true); // a0-a7 are physical registers 10-17
		auto arg_operand = get_operand(node->args[i]);
		cur_block->AddInstruction(
			std::make_shared<ASMAddInstruction>(
				arg_reg,
				arg_operand,
				std::make_shared<Register>(0, true) // x0 is physical register 0
			)
		);
		if (i > 7) {
			cur_block->AddInstruction(
				std::make_shared<ASMSwInstruction>(
					arg_reg,
					std::make_shared<Register>(2, true), // sp is physical register 2
					std::make_shared<Immediate>(4 * (i - 8))
				)
			);
		}
	}
	cur_block->AddInstruction(std::make_shared<ASMCallInstruction>(std::make_shared<Label>(node->func_name)));
	cur_block->AddInstruction(std::make_shared<ASMAddiInstruction>(
		get_operand(node->result),
		std::make_shared<Register>(10, true), // a0 is physical register 10
		std::make_shared<Immediate>(0)
	));
}

void InstSelector::visit(CallWithoutRetInstruction *node) {
	for (size_t i = 0; i < node->args.size(); i++) {
		auto arg_reg = std::make_shared<Register>(10 + i, true); // a0-a7 are physical registers 10-17
		auto arg_operand = get_operand(node->args[i]);
		cur_block->AddInstruction(
			std::make_shared<ASMAddInstruction>(
				arg_reg,
				arg_operand,
				std::make_shared<Register>(0, true) // x0 is physical register 0
			)
		);
		if (i > 7) {
			cur_block->AddInstruction(
				std::make_shared<ASMSwInstruction>(
					arg_reg,
					std::make_shared<Register>(2, true), // sp is physical register 2
					std::make_shared<Immediate>(4 * (i - 8))
				)
			);
		}
	}
	cur_block->AddInstruction(std::make_shared<ASMCallInstruction>(std::make_shared<Label>(node->func_name)));
}
void InstSelector::visit(PhiInstruction *node) {
    auto dest = get_operand(node->result);
    for (size_t i = 0; i < node->values.size(); ++i) {
        auto val = get_operand(node->values[i]);
        auto label = node->labels[i];
        if (block_map.count(label)) {
            auto blk = block_map[label];
            if (!blk->instructions.empty()) {
                auto insert_pos = blk->instructions.end();
                auto r_it = blk->instructions.rbegin();
                while (r_it != blk->instructions.rend()) {
                    auto opcode = (*r_it)->opcode;
                    bool is_term = (opcode == ASMOpcode::JAL || opcode == ASMOpcode::JALR || 
                       opcode == ASMOpcode::BEQ || opcode == ASMOpcode::BNE || 
                       opcode == ASMOpcode::BLT || opcode == ASMOpcode::BGE || 
                       opcode == ASMOpcode::BLTU || opcode == ASMOpcode::BGEU); 
                    
                    if (is_term) {
                        if (opcode == ASMOpcode::JAL && std::dynamic_pointer_cast<ASMCallInstruction>(*r_it)) {
                            is_term = false; // Call is not terminator
                        }
                    }
                    
                    if (is_term) {
                        insert_pos = r_it.base(); 
                        --insert_pos;
                    } else {
                        break;
                    }
                    ++r_it;
                }
                blk->instructions.insert(insert_pos, std::make_shared<ASMAddiInstruction>(dest, val, std::make_shared<Immediate>(0)));
            } else {
                blk->AddInstruction(std::make_shared<ASMAddiInstruction>(dest, val, std::make_shared<Immediate>(0)));
            }
        } else {
            pending_phi_copies[label].emplace_back(dest, val);
        }
    }
}
void InstSelector::visit(SelectInstruction *node) {}
void InstSelector::visit(ZextInstruction *node) {
	cur_block->AddInstruction(std::make_shared<ASMAndiInstruction>(
		get_operand(node->result),
		get_operand(node->op),
		std::make_shared<Immediate>(1)
	));
}
void InstSelector::visit(StructDefInstruction *node) {
    // Struct definition only provides type info, no code generation needed.
}
void InstSelector::visit(ConstVarDefInstruction *node) {
     asm_globals.push_back(std::make_shared<ASMGlobalVariable>(
        node->const_var->true_name,
        node->const_var->type,
        node->value
    ));
}
void InstSelector::visit(GlobalVarDefInstruction *node) {
     asm_globals.push_back(std::make_shared<ASMGlobalVariable>(
        node->global_var->true_name,
        node->global_var->type,
        node->value
    ));
}
