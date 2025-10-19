#ifndef IRCONSTANT_H
#define IRCONSTANT_H
#include "IRVar.h"
#include "IRType.h"

enum class LiteralType {
    Boolean, Integer, Nullptr
};

class IRLiteral : public IRNode {
protected:
    std::shared_ptr<IRType> type;
    LiteralType literal_type{};
public:
    explicit IRLiteral(const std::shared_ptr<IRType> &type_): IRNode() {
        type = type_;
    }
};

class LiteralInt : public IRLiteral {
    int64_t value = 0;
public:
    LiteralInt(const std::shared_ptr<IRType> &type, const int64_t value)
        : IRLiteral(type) {
        this->value = value;
        literal_type = LiteralType::Integer;
    }

    [[nodiscard]] int64_t getValue() const { return value; }

    void setValue(const int64_t &value) { this->value = value; }
};

class LiteralBool : public IRLiteral {
    bool value = false;
public:
    LiteralBool(const std::shared_ptr<IRType> &type, bool value) : IRLiteral(type) {
        this->value = value;
        literal_type = LiteralType::Boolean;
    }
};

class Nullptr : public IRLiteral {
public:
    explicit Nullptr(const std::shared_ptr<IRType> &type) : IRLiteral(type) {
        literal_type = LiteralType::Nullptr;
    }
};
#endif //IRCONSTANT_H