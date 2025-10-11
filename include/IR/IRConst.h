#ifndef IRCONSTANT_H
#define IRCONSTANT_H
#include "IRVar.h"
#include "IRType.h"

enum class ConstType {
    Boolean, Integer, Nullptr
};

class IRConst : public IRVar {
protected:
    std::shared_ptr<IRType> type;
    ConstType const_type{};
public:
    explicit IRConst(const std::shared_ptr<IRType> &type_): IRVar("", VarType::Constant) {
        type = type_;
    }
};

class ConstInt : public IRConst {
    int64_t value = 0;
public:
    ConstInt(const std::shared_ptr<IRType> &type, const int64_t value)
        : IRConst(type) {
        this->value = value;
        const_type = ConstType::Integer;
    }

    [[nodiscard]] int64_t getValue() const { return value; }

    void setValue(const int64_t value) { this->value = value; }
};

class ConstBool : public IRConst {
    bool value = false;
public:
    ConstBool(const std::shared_ptr<IRType> &type, bool value) : IRConst(type) {
        this->value = value;
        const_type = ConstType::Boolean;
    }
};

class Nullptr : public IRConst {
public:
    explicit Nullptr(const std::shared_ptr<IRType> &type) : IRConst(type) {
        const_type = ConstType::Nullptr;
    }
};
#endif //IRCONSTANT_H