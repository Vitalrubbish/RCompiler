#ifndef IRTYPE_H
#define IRTYPE_H
#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include "IRNode.h"

enum class TypeID {
    voidType, integerType, pointerType,
    arrayType, structType, functionType
};

class IRType : public IRNode {
    TypeID id;

public:
    explicit IRType(const TypeID id) { this->id = id; }

    ~IRType() override = default;

	void print() override {};

    [[nodiscard]] virtual std::string toString() const = 0;

    [[nodiscard]] TypeID GetTypeId() const { return id; }

    [[nodiscard]] bool isVoidType() const { return id == TypeID::voidType; }

    [[nodiscard]] bool isIntegerType() const { return id == TypeID::integerType; }

    [[nodiscard]] bool isPointerType() const { return id == TypeID::pointerType; }

    [[nodiscard]] bool isArrayType() const { return id == TypeID::arrayType; }

    [[nodiscard]] bool isStructType() const { return id == TypeID::structType; }

    [[nodiscard]] bool isFunctionType() const { return id == TypeID::functionType; }
};

class IRVoidType: public IRType {
public:
    IRVoidType() : IRType(TypeID::voidType) {}

    [[nodiscard]] std::string toString() const override {
       return "void";
    }

	void print() override {
	    std::cout << "void";
    }
};

class IRIntegerType: public IRType {
    uint32_t length = 0;
public:
    explicit IRIntegerType(const uint32_t &length) : IRType(TypeID::integerType) {
        this->length = length;
    }

    [[nodiscard]] std::string toString() const override {
        return "integer";
    }

	void print() override {
	    std::cout << "i" << length;
    }
};

class IRPointerType: public IRType {
    std::shared_ptr<IRType> baseType;
public:
    explicit IRPointerType(const std::shared_ptr<IRType> &base) : IRType(TypeID::pointerType) {
        baseType = base;
    }

    [[nodiscard]] std::string toString() const override {
        return baseType->toString() + "*";
    }
};

class IRArrayType: public IRType {
    std::shared_ptr<IRType> baseType;
    uint32_t length = 0;
public:
    IRArrayType(const std::shared_ptr<IRType> &base, const uint32_t& len) :
    IRType(TypeID::arrayType) {
        baseType = base;
        length = len;
    }

    [[nodiscard]] std::string toString() const override {
        return "[" + baseType->toString() + ";" + std::to_string(length) + "]";
    }
};

class IRStructType: public IRType {
public:
	std::string name;
	std::vector<std::shared_ptr<IRType>> members;

    IRStructType(const std::string& name, const std::vector<std::shared_ptr<IRType>>& members):
        IRType(TypeID::structType) {
        this->name = name;
        this->members = members;
    }

    void print() override {
	    std::cout << "%struct." << name;
    }

    [[nodiscard]] std::string toString() const override {
        return "%struct." + name;
    }
};


class IRFunctionType: public IRType {
    std::string name;
    std::shared_ptr<IRType> returnType;
    std::vector<std::shared_ptr<IRType>> parameters;
public:
    IRFunctionType(const std::string& name, const std::shared_ptr<IRType>& returnType,
        const std::vector<std::shared_ptr<IRType>>& parameters): IRType(TypeID::functionType) {
        this->name = name;
        this->returnType = returnType;
        this->parameters = parameters;
    }

    [[nodiscard]] std::string toString() const override {
        return name;
    }
};
#endif //IRTYPE_H