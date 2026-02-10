#ifndef ASMOPERAND_H
#define ASMOPERAND_H

#include <cstdint>
#include <string>
#include <iostream>

enum class OperandType {
    VIRTUAL_REGISTER,
    PHYSICAL_REGISTER,
    IMMEDIATE,
    LABEL,
    STACK_OBJECT
};

class ASMOperand : public ASMNode {
public:
	~ASMOperand() override = default;

	OperandType type;

    explicit ASMOperand(OperandType t) : type(t) {}

    void print() override = 0;
};

class Register: public ASMOperand {
public:
    uint32_t index = 0;
    bool is_physical = false;


    Register(int idx, bool physical)
        : ASMOperand(physical ? OperandType::PHYSICAL_REGISTER : OperandType::VIRTUAL_REGISTER),
          index(idx), is_physical(physical) {}

    void print() override {
        static const std::string reg_names[] = {
            "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
            "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
            "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
            "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
        };
        if (is_physical) {
            if (index < 32) {
                std::cout << reg_names[index];
            } else {
                std::cout << "x" << index;
            }
        } else {
            std::cout << "v" << index;
        }
    }
};

class Immediate: public ASMOperand {
public:
    uint64_t value = 0;

    explicit Immediate(const uint64_t val)
        : ASMOperand(OperandType::IMMEDIATE), value(val) {}

    void print() override {
	    std::cout << value;
    }
};

class Label: public ASMOperand {
public:
    std::string name;

    explicit Label(const std::string &lbl_name)
        : ASMOperand(OperandType::LABEL), name(lbl_name) {}

    void print() override {
        std::cout << name;
    }
};



class StackObject: public ASMOperand {
public:
	uint32_t index = 0;
	uint32_t size = 0;
	uint32_t offset = 0;

	StackObject(const uint32_t idx, const uint32_t sz)
		: ASMOperand(OperandType::STACK_OBJECT), index(idx), size(sz) {
            offset = 0;
        }

    void print() override {
    	std::cout << offset;
    }
};

#endif //ASMOPERAND_H