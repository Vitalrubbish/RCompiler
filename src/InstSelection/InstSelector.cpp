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
    }
    return var_map[var->true_name];
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

void InstSelector::visit(LoadInstruction *node) {}
void InstSelector::visit(StoreInstruction *node) {}
void InstSelector::visit(GetElementPtrInstruction *node) {}

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

void InstSelector::visit(MulInstruction *node) {}
void InstSelector::visit(SDivInstruction *node) {}
void InstSelector::visit(UDivInstruction *node) {}
void InstSelector::visit(SremInstruction *node) {}
void InstSelector::visit(UremInstruction *node) {}
void InstSelector::visit(ShlInstruction *node) {}
void InstSelector::visit(AShrInstruction *node) {}
void InstSelector::visit(AndInstruction *node) {}
void InstSelector::visit(OrInstruction *node) {}
void InstSelector::visit(XorInstruction *node) {}
void InstSelector::visit(RetInstruction *node) {}
void InstSelector::visit(ConditionalBrInstruction *node) {}
void InstSelector::visit(UnconditionalBrInstruction *node) {}
void InstSelector::visit(ICmpInstruction *node) {}
void InstSelector::visit(CallWithRetInstruction *node) {}
void InstSelector::visit(CallWithoutRetInstruction *node) {}
void InstSelector::visit(PhiInstruction *node) {}
void InstSelector::visit(SelectInstruction *node) {}
void InstSelector::visit(ZextInstruction *node) {}
void InstSelector::visit(StructDefInstruction *node) {}
void InstSelector::visit(ConstVarDefInstruction *node) {}
void InstSelector::visit(GlobalVarDefInstruction *node) {}
