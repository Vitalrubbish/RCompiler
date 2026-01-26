#ifndef ASMOPERAND_H
#define ASMOPERAND_H

#include <cstdint>
#include <string>


enum class OperandType {
    VIRTUAL_REGISTER,
    PHYSICAL_REGISTER,
    IMMEDIATE,
    LABEL,
    STACK_OBJECT
};

class ASMOperand {
public:
	virtual ~ASMOperand() = default;

	OperandType type;

    explicit ASMOperand(OperandType t) : type(t) {}

    virtual void print() = 0;
};

class Register: public ASMOperand {
public:
    uint32_t index = 0;
    bool is_physical = false;


    Register(int idx, bool physical)
        : ASMOperand(physical ? OperandType::PHYSICAL_REGISTER : OperandType::VIRTUAL_REGISTER),
          index(idx), is_physical(physical) {}

    void print() override {}
};

class Immediate: public ASMOperand {
public:
    uint64_t value = 0;

    explicit Immediate(const uint64_t val)
        : ASMOperand(OperandType::IMMEDIATE), value(val) {}

    void print() override {}
};

class Label: public ASMOperand {
public:
    std::string name;

    explicit Label(const std::string &lbl_name)
        : ASMOperand(OperandType::LABEL), name(lbl_name) {}

    void print() override {}
};

class StackObject: public ASMOperand {
public:
	uint32_t index = 0;
	uint32_t size = 0;
	uint32_t offset = 0;

	StackObject(const uint32_t idx, const uint32_t sz)
		: ASMOperand(OperandType::STACK_OBJECT), index(idx), size(sz) {}
};

#endif //ASMOPERAND_H