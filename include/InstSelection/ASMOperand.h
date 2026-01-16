

enum class OperandType {
    VIRTUAL_REGISTER,
    PHYSICAL_REGISTER,
    IMMEDIATE,
    LABEL,
    STACK_OFFSET
}

class ASMOperand {
public:
    OperandType type;

    ASMOperand(OperandType t) : type(t) {}

    virtual void print() = 0;
}

class Register: public ASMOperand {
public:
    int index = 0;
    bool is_physical = false;


    Register(int idx, bool physical)
        : ASMOperand(physical ? OperandType::PHYSICAL_REGISTER : OperandType::VIRTUAL_REGISTER),
          index(idx), is_physical(physical) {}

    void print() override {}
}

class Immediate: public ASMOperand {
public:
    uint64_t value = 0;

    Immediate(uint64_t val)
        : ASMOperand(OperandType::IMMEDIATE), value(val) {}

    void print() override {}
}

class Label: public ASMOperand {
public:
    std::string name;

    Label(const std::string &lbl_name)
        : ASMOperand(OperandType::LABEL), name(lbl_name) {}

    void print() override {}
}