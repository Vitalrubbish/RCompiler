#ifndef IRVISITOR_H
#define IRVISITOR_H

#include <vector>

class IRProgram;
class IRFunction;
class IRBasicBlock;
class IRFunctionParam;
class IRInstruction;
class UnreachableInstruction;
class AllocaInstruction;
class LoadInstruction;
class StoreInstruction;
class GetElementPtrInstruction;
class AddInstruction;
class SubInstruction;
class MulInstruction;
class SDivInstruction;
class UDivInstruction;
class SremInstruction;
class UremInstruction;
class ShlInstruction;
class AShrInstruction;
class AndInstruction;
class OrInstruction;
class XorInstruction;
class RetInstruction;
class ConditionalBrInstruction;
class UnconditionalBrInstruction;
class ICmpInstruction;
class CallWithRetInstruction;
class CallWithoutRetInstruction;
class PhiInstruction;
class SelectInstruction;
class ZextInstruction;
class StructDefInstruction;
class ConstVarDefInstruction;
class GlobalVarDefInstruction;

class IRVisitor {
public:
    virtual ~IRVisitor() = default;

    // High Level Nodes
    virtual void visit(IRProgram *node) = 0;
    virtual void visit(IRFunction *node) = 0;
    virtual void visit(IRBasicBlock *node) = 0;
    virtual void visit(IRFunctionParam *node) = 0;

    // Instructions
    virtual void visit(IRInstruction *node) = 0; // Dispatcher

    virtual void visit(UnreachableInstruction *node) = 0;
    virtual void visit(AllocaInstruction *node) = 0;
    virtual void visit(LoadInstruction *node) = 0;
    virtual void visit(StoreInstruction *node) = 0;
    virtual void visit(GetElementPtrInstruction *node) = 0;
    virtual void visit(AddInstruction *node) = 0;
    virtual void visit(SubInstruction *node) = 0;
    virtual void visit(MulInstruction *node) = 0;
    virtual void visit(SDivInstruction *node) = 0;
    virtual void visit(UDivInstruction *node) = 0;
    virtual void visit(SremInstruction *node) = 0;
    virtual void visit(UremInstruction *node) = 0;
    virtual void visit(ShlInstruction *node) = 0;
    virtual void visit(AShrInstruction *node) = 0;
    virtual void visit(AndInstruction *node) = 0;
    virtual void visit(OrInstruction *node) = 0;
    virtual void visit(XorInstruction *node) = 0;
    virtual void visit(RetInstruction *node) = 0;
    virtual void visit(ConditionalBrInstruction *node) = 0;
    virtual void visit(UnconditionalBrInstruction *node) = 0;
    virtual void visit(ICmpInstruction *node) = 0;
    virtual void visit(CallWithRetInstruction *node) = 0;
    virtual void visit(CallWithoutRetInstruction *node) = 0;
    virtual void visit(PhiInstruction *node) = 0;
    virtual void visit(SelectInstruction *node) = 0;
    virtual void visit(ZextInstruction *node) = 0;

    // Definition
    virtual void visit(StructDefInstruction *node) = 0;
    virtual void visit(ConstVarDefInstruction *node) = 0;
    virtual void visit(GlobalVarDefInstruction *node) = 0;
};

#endif //IRVISITOR_H
