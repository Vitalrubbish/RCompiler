#ifndef IRTYPE_H
#define IRTYPE_H
#include <string>
#include <memory>
#include <vector>
enum class TypeID {
    voidType, integerType, pointerType,
    arrayType, structType, functionType
};

class IRType {
    TypeID id;

public:
    explicit IRType(const TypeID id) { this->id = id; }

    virtual ~IRType() = default;

    [[nodiscard]] virtual std::string toString() const = 0;

    [[nodiscard]] TypeID GetTypeId() const { return id; }

    [[nodiscard]] bool isVoidType() const { return id == TypeID::voidType; }

    [[nodiscard]] bool isIntegerType() const { return id == TypeID::integerType; }

    [[nodiscard]] bool isPointerType() const { return id == TypeID::pointerType; }

    [[nodiscard]] bool isArrayType() const { return id == TypeID::arrayType; }

    [[nodiscard]] bool isStructType() const { return id == TypeID::structType; }

    [[nodiscard]] bool isFunctionType() const { return id == TypeID::functionType; }
};

class VoidType: public IRType {
public:
    VoidType() : IRType(TypeID::voidType) {}

    [[nodiscard]] std::string toString() const override {
       return "void";
    }
};

class IntegerType: public IRType {
    uint32_t length = 0;
public:
    explicit IntegerType(const uint32_t &length) : IRType(TypeID::integerType) {
        this->length = length;
    }

    [[nodiscard]] std::string toString() const override {
        return "integer";
    }
};

class PointerType: public IRType {
    std::shared_ptr<IRType> baseType;
public:
    explicit PointerType(const std::shared_ptr<IRType> &base) : IRType(TypeID::pointerType) {
        baseType = base;
    }

    [[nodiscard]] std::string toString() const override {
        return baseType->toString() + "*";
    }
};

class ArrayType: public IRType {
    std::shared_ptr<IRType> baseType;
    uint32_t length = 0;
public:
    ArrayType(const std::shared_ptr<IRType> &base, const uint32_t& len) :
    IRType(TypeID::pointerType) {
        baseType = base;
        length = len;
    }

    [[nodiscard]] std::string toString() const override {
        return "[" + baseType->toString() + ";" + std::to_string(length) + "]";
    }
};

class StructType: public IRType {
    std::string name;
    std::vector<std::shared_ptr<IRType>> members;
public:
    StructType(const std::string& name, const std::vector<std::shared_ptr<IRType>>& members):
        IRType(TypeID::structType) {
        this->name = name;
        this->members = members;
    }

    [[nodiscard]] std::string toString() const override {
        return name;
    }
};

class FunctionType: public IRType {
    std::string name;
    std::shared_ptr<IRType> returnType;
    std::vector<std::shared_ptr<IRType>> parameters;
public:
    FunctionType(const std::string& name, const std::shared_ptr<IRType>& returnType,
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