#ifndef IRINSTRUCTION_H
#define IRINSTRUCTION_H

#include <memory>

#include "IRBasicBlock.h"
#include "IRUser.h"

class IRValue;
class IRType;
class IRBasicBlock;
class IRVisitor;

/**************** ENUM TYPES ****************/
enum class OpType {
    Ret, Br,
    Add, Sub, Mul, SDiv, UDiv,
    Alloca, Load, Store, GetElementPtr,
    BitCast, ZExt, SExt,
    ICmp, Call, Phi
};

/**************** BASE CLASS ****************/
class IRInstruction : public IRUser {
    std::shared_ptr<IRBasicBlock> parent_block;
    OpType opType;
public:

    [[nodiscard]] virtual std::string toString() const = 0;

    explicit IRInstruction(const std::shared_ptr<IRType> &type,
        const std::shared_ptr<IRBasicBlock>& parent_block, OpType opType, const uint32_t &numOfOperands
        ) : IRUser(type, numOfOperands) {
        this->parent_block = parent_block;
        this->opType = opType;
        this->parent_block->instructions.push_back(std::make_shared<IRInstruction>(*this));
    }
};

/*************** DERIVED CLASS ****************/
class TerminatorInstruction : public IRInstruction {
public:
    explicit TerminatorInstruction(const std::shared_ptr<IRType> &type,
        const std::shared_ptr<IRBasicBlock>& parent_block, OpType opType, const uint32_t &numOfOperands
        ) : IRInstruction(type, parent_block, opType, numOfOperands) {}
};

class ReturnInstruction : public TerminatorInstruction {
public:
    explicit ReturnInstruction()
};


#endif //IRINSTRUCTION_H
