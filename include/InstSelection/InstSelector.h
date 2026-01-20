#ifndef RCOMPILER_INSTSELECTOR_H
#define RCOMPILER_INSTSELECTOR_H

#include "IR/IRVisitor.h"

class InstSelector : public IRVisitor {
public:
    InstSelector() = default;

    void visit(IRProgram *node) override;
    void visit(IRFunction *node) override;
    void visit(IRBasicBlock *node) override;
    void visit(IRFunctionParam *node) override;

    void visit(IRInstruction *node) override;

    void visit(UnreachableInstruction *node) override;
    void visit(AllocaInstruction *node) override;
    void visit(LoadInstruction *node) override;
    void visit(StoreInstruction *node) override;
    void visit(GetElementPtrInstruction *node) override;
    void visit(AddInstruction *node) override;
    void visit(SubInstruction *node) override;
    void visit(MulInstruction *node) override;
    void visit(SDivInstruction *node) override;
    void visit(UDivInstruction *node) override;
    void visit(SremInstruction *node) override;
    void visit(UremInstruction *node) override;
    void visit(ShlInstruction *node) override;
    void visit(AShrInstruction *node) override;
    void visit(AndInstruction *node) override;
    void visit(OrInstruction *node) override;
    void visit(XorInstruction *node) override;
    void visit(RetInstruction *node) override;
    void visit(ConditionalBrInstruction *node) override;
    void visit(UnconditionalBrInstruction *node) override;
    void visit(ICmpInstruction *node) override;
    void visit(CallWithRetInstruction *node) override;
    void visit(CallWithoutRetInstruction *node) override;
    void visit(PhiInstruction *node) override;
    void visit(SelectInstruction *node) override;
    void visit(ZextInstruction *node) override;

    void visit(StructDefInstruction *node) override;
    void visit(ConstVarDefInstruction *node) override;
    void visit(GlobalVarDefInstruction *node) override;
};


#endif //RCOMPILER_INSTSELECTOR_H