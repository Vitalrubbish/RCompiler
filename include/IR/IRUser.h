#ifndef IRUSER_H
#define IRUSER_H
#include <memory>
#include <vector>

#include "IRValue.h"

class IRType;
class IRValue;
class IRUser;

class Use {
    std::shared_ptr<IRValue> value;
    std::shared_ptr<IRUser> user;
public:
    explicit Use(const std::shared_ptr<IRUser> &user) {
        this->user = user;
    }

    ~Use() = default;

    std::shared_ptr<IRValue> getValue() { return value; }

    std::shared_ptr<IRUser> getUser() { return user; }

    void setValue(const std::shared_ptr<IRValue> &value) {
        this->value = value;
    }
};

class IRUser : public IRValue {
    std::vector<Use> operands;
public:
    IRUser(const std::shared_ptr<IRType>& type,  const uint32_t &numOfOperands, const std::string& name = "")
        : IRValue (name, type) {
        operands.resize(numOfOperands);
    }

    std::shared_ptr<IRValue> getOperand(uint32_t i) { return operands[i].getValue(); }

    void setOperand(uint32_t i, const std::shared_ptr<IRValue> &value) { operands[i].setValue(value); }

    [[nodiscard]] uint32_t getOperandSize() const { return operands.size(); }

    void OperandResize(uint32_t s) { operands.resize(s); }
};
#endif //IRUSER_H
