#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>
#include <cstdint>

class Type;
class FunctionType;

struct Method {
    std::string name_;
    Type *type_;
};

enum class TypeKind {
    Primitive, Function, Struct, Enumeration, Tuple,
    Slice, Inferred, Array
};

class Type {
public:
    std::vector<Method> methods_;

    Type() = default;

    virtual ~Type() {
        for (auto &method: methods_) {
            delete method.type_;
        }
    }

    [[nodiscard]] virtual TypeKind getKind() const = 0;

    [[nodiscard]] virtual std::string toString() const = 0;

    virtual bool equal(Type *other) const = 0;
};

class PrimitiveType : public Type {
    std::string name_;

public:
    explicit PrimitiveType(const std::string &name) {
        name_ = name;
    }

    ~PrimitiveType() override = default;

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Primitive;
    }

    [[nodiscard]] std::string toString() const override {
        return name_;
    }

    bool equal(Type *other) const override {
        return this == other;
    }
};

struct StructMember {
    std::string name_;
    Type *type_;
};

class StructType : public Type {
    std::string name_;
    std::vector<StructMember> members_;

public:
    explicit StructType(const std::string &name, const std::vector<StructMember> &members) {
        name_ = name;
        members_ = members;
    }

    ~StructType() override = default;

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Struct;
    }

    [[nodiscard]] std::string toString() const override {
        return name_;
    }

    bool equal(Type *other) const override {
        return this == other;
    }
};

class EnumerationType : public Type {
    std::string name_;
    std::vector<std::string> variant_;
public:
    explicit EnumerationType(const std::string &name,
        const std::vector<std::string> &variant) {
        variant_ = variant;
        name_ = name;
    }

    ~EnumerationType() override = default;

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Enumeration;
    }

    [[nodiscard]] std::string toString() const override {
        return name_;
    }

    bool equal(Type *other) const override {
        return this == other;
    }
};

class FunctionType : public Type {
    std::vector<Type *> params_;
    Type *ret_;

public:
    FunctionType(const std::vector<Type *> &params, Type *ret) {
        params_ = params;
        ret_ = ret;
    }

    ~FunctionType() override = default;

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Function;
    }

    [[nodiscard]] std::string toString() const override {
        std::string str = "fn ( ";
        for (auto param: params_) {
            str += param->toString();
            str += ", ";
        }
        str += ") -> " + ret_->toString();
        return str;
    }

    bool equal(Type *other) const override {
        if (!other || other->getKind() != TypeKind::Function) {
            return false;
        }
        auto *tmp = dynamic_cast<FunctionType *>(other);
        if (!ret_->equal(tmp->ret_)) {
            return false;
        }
        if (params_.size() != tmp->params_.size()) {
            return false;
        }
        for (uint32_t i = 0; i < params_.size(); i++) {
            if (!params_[i]->equal(tmp->params_[i])) {
                return false;
            }
        }
        return true;
    }
};

class TupleType : public Type {
    std::vector<Type *> types_;

public:
    explicit TupleType(const std::vector<Type *> &types) {
        types_ = types;
    }

    ~TupleType() override = default;

    [[nodiscard]] TypeKind getKind() const override { return TypeKind::Tuple; }

    [[nodiscard]] std::string toString() const override {
        std::string str = "( ";
        for (auto type: types_) {
            str += type->toString();
            str += ", ";
        }
        str += " )";
        return str;
    }

    bool equal(Type *other) const override {
        if (!other || other->getKind() != TypeKind::Tuple) {
            return false;
        }
        auto *tmp = dynamic_cast<TupleType *>(other);
        if (types_.size() != tmp->types_.size()) {
            return false;
        }
        for (uint32_t i = 0; i < types_.size(); i++) {
            if (!types_[i]->equal(tmp->types_[i])) {
                return false;
            }
        }
        return true;
    }
};

class SliceType : public Type {
    Type *type_ = nullptr;

public:
    explicit SliceType(Type *type) {
        type_ = type;
    }

    ~SliceType() override = default;

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Slice;
    }

    [[nodiscard]] std::string toString() const override {
        return "[" + type_->toString() + "]";
    }

    bool equal(Type *other) const override {
        if (!other || other->getKind() != TypeKind::Slice) {
            return false;
        }
        auto *tmp = dynamic_cast<SliceType *>(other);
        return type_->equal(tmp->type_);
    }
};

class ArrayType : public Type {
    Type* base_ = nullptr;
    uint32_t length_ = 0;
public:
    ArrayType(Type* base, const uint32_t& length) {
        base_ = base;
        length_ = length;
    }

    ~ArrayType() override = default;

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Array;
    }

    [[nodiscard]] std::string toString() const override {
        return "[" + base_ -> toString() + "; " + "]";
    }

    bool equal(Type *other) const override {
        return this == other;
    }
};

class InferredType : public Type {
    // TODO
};

#endif //TYPE_H
