#ifndef IRCONSTANT_H
#define IRCONSTANT_H
#include "IRVar.h"
#include "IRType.h"

enum class LiteralType {
    Boolean, Integer, Nullptr
};

class IRLiteral : public IRNode {
protected:
    LiteralType literal_type{};
public:
    explicit IRLiteral() = default;

	~IRLiteral() override = default;

	void print() override {}
};

class LiteralInt : public IRLiteral {
    int64_t value = 0;
public:
    explicit LiteralInt(const int64_t &value) {
        literal_type = LiteralType::Integer;
    	this->value = value;
    }

    [[nodiscard]] int64_t getValue() const { return value; }

    void setValue(const int64_t &value) { this->value = value; }

	void print() override {
	    std::cout << "i32 " << value;
    }
};

class LiteralBool : public IRLiteral {
    bool value = false;
public:
    explicit LiteralBool(const bool value) {
        this->value = value;
        literal_type = LiteralType::Boolean;
    }
};

class Nullptr : public IRLiteral {
public:
    explicit Nullptr() {
        literal_type = LiteralType::Nullptr;
    }
};
#endif //IRCONSTANT_H