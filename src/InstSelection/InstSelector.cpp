#include "InstSelection/InstSelector.h"

#include "IR/IRProgram.h"

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
    for (auto &block : node->blocks) {
        block->accept(this);
    }
}

void InstSelector::visit(IRFunctionParam *node) {
    // Empty implementation
}

void InstSelector::visit(IRBasicBlock *node) {
    for (auto &inst : node->instructions) {
        inst->accept(this);
    }
}

void InstSelector::visit(IRInstruction *node) {
    // Empty implementation
}

void InstSelector::visit(UnreachableInstruction *node) {}
void InstSelector::visit(AllocaInstruction *node) {}
void InstSelector::visit(LoadInstruction *node) {}
void InstSelector::visit(StoreInstruction *node) {}
void InstSelector::visit(GetElementPtrInstruction *node) {}
void InstSelector::visit(AddInstruction *node) {}
void InstSelector::visit(SubInstruction *node) {}
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
