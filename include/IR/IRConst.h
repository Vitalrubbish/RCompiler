#ifndef IRCONSTANT_H
#define IRCONSTANT_H
#include "IRUser.h"

class IRConst : public IRUser {
public:
    explicit IRConst(const std::shared_ptr<IRType> &type, const uint32_t &numOfOperands = 0)
        : IRUser(type, numOfOperands) {}
};

class IRConstInt : public IRConst {
    int64_t value = 0;
public:
    IRConstInt(const std::shared_ptr<IRType> &type, const int64_t value)
        : IRConst(type, 0) { this->value = value; }

    [[nodiscard]] int64_t getValue() const { return value; }

    void setValue(const int64_t value) { this->value = value; }
};

class IRConstArray : public IRConst {
    std::vector<std::shared_ptr<IRConst>> elements;
public:
    IRConstArray(const std::shared_ptr<IRType> &type, const std::vector<std::shared_ptr<IRConst>> &elements)
        : IRConst(type, elements.size()) {
        for (uint32_t i = 0; i < elements.size(); i++) {
            setOperand(i, elements[i]);
        }
    }
};

class IRConstStruct : public IRConst {
    std::vector<std::shared_ptr<IRConst>> elements;
public:
    IRConstStruct(const std::shared_ptr<IRType> &type, const std::vector<std::shared_ptr<IRConst>> &elements)
        : IRConst(type, elements.size()) {
        for (uint32_t i = 0; i < elements.size(); i++) {
            setOperand(i, elements[i]);
        }
    }
};

class IRUndef : public IRConst {
public:
    explicit IRUndef(const std::shared_ptr<IRType> &type) : IRConst(type, 0) {}
};
#endif //IRCONSTANT_H