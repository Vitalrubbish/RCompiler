#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <stdexcept>

class Type;
class FunctionType;

struct Method {
    std::string name_;
    std::shared_ptr<Type> type_;
};

enum class TypeKind {
    Primitive, Function, Struct, Enumeration, Tuple,
    Slice, Inferred, Array
};

class Type {
public:
    std::vector<Method> methods_;

    Type() = default;

    virtual ~Type() = default;

    virtual Type &operator=(const Type &other) {
        methods_ = other.methods_;
        return *this;
    }

    [[nodiscard]] virtual TypeKind getKind() const = 0;

    [[nodiscard]] virtual std::string toString() const = 0;

    [[nodiscard]] virtual bool equal(const std::shared_ptr<Type> &other) const = 0;
};

class PrimitiveType : public Type {
public:
    std::string name_;

    explicit PrimitiveType(std::string name) : name_(std::move(name)) {
    }

    PrimitiveType &operator=(const Type &other) override {
        methods_ = other.methods_;
        if (const auto *ptr = dynamic_cast<const PrimitiveType *>(&other)) {
            name_ = ptr->name_;
        } else {
            throw std::runtime_error("Cannot assign non-PrimitiveType to PrimitiveType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override { return TypeKind::Primitive; }
    [[nodiscard]] std::string toString() const override { return name_; }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other->getKind() != TypeKind::Primitive) return false;
        auto ptr = std::static_pointer_cast<PrimitiveType>(other);
        return name_ == ptr->name_;
    }
};

struct StructMember {
    std::string name_;
    std::shared_ptr<Type> type_;
};

class StructType : public Type {
public:
    std::string name_;
    std::vector<StructMember> members_;

    StructType(std::string name, std::vector<StructMember> members)
        : name_(std::move(name)), members_(std::move(members)) {
    }

    StructType &operator=(const Type &other) override {
        methods_ = other.methods_;
        if (const auto *ptr = dynamic_cast<const StructType *>(&other)) {
            name_ = ptr->name_;
            members_ = ptr->members_;
        } else {
            throw std::runtime_error("Cannot assign non-StructType to StructType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override { return TypeKind::Struct; }
    [[nodiscard]] std::string toString() const override { return name_; }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other->getKind() != TypeKind::Struct) return false;
        auto ptr = std::static_pointer_cast<StructType>(other);
        if (name_ != ptr->name_ || members_.size() != ptr->members_.size()) return false;
        for (size_t i = 0; i < members_.size(); i++) {
            if (members_[i].name_ != ptr->members_[i].name_ ||
                !members_[i].type_->equal(ptr->members_[i].type_)) {
                return false;
            }
        }
        return true;
    }
};

class EnumerationType : public Type {
public:
    std::string name_;
    std::vector<std::string> variants_;

    EnumerationType(std::string name, std::vector<std::string> variants)
        : name_(std::move(name)), variants_(std::move(variants)) {
    }

    EnumerationType &operator=(const Type &other) override {
        methods_ = other.methods_;
        if (const auto *ptr = dynamic_cast<const EnumerationType *>(&other)) {
            name_ = ptr->name_;
            variants_ = ptr->variants_;
        } else {
            throw std::runtime_error("Cannot assign non-EnumerationType to EnumerationType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override { return TypeKind::Enumeration; }
    [[nodiscard]] std::string toString() const override { return name_; }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other->getKind() != TypeKind::Enumeration) return false;
        auto ptr = std::static_pointer_cast<EnumerationType>(other);
        return name_ == ptr->name_ && variants_ == ptr->variants_;
    }
};

class FunctionType : public Type {
public:
    std::vector<std::shared_ptr<Type> > params_;
    std::shared_ptr<Type> ret_;

    FunctionType(std::vector<std::shared_ptr<Type> > params, std::shared_ptr<Type> ret)
        : params_(std::move(params)), ret_(std::move(ret)) {
    }

    FunctionType &operator=(const Type &other) override {
        methods_ = other.methods_;
        if (const auto *ptr = dynamic_cast<const FunctionType *>(&other)) {
            params_ = ptr->params_;
            ret_ = ptr->ret_;
        } else {
            throw std::runtime_error("Cannot assign non-FunctionType to FunctionType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override { return TypeKind::Function; }

    [[nodiscard]] std::string toString() const override {
        std::string str = "fn(";
        for (const auto &param: params_) {
            str += param->toString();
            str += ",";
        }
        if (!params_.empty()) str.pop_back();
        str += ")->" + ret_->toString();
        return str;
    }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other->getKind() != TypeKind::Function) return false;
        auto ptr = std::static_pointer_cast<FunctionType>(other);
        if (!ret_->equal(ptr->ret_) || params_.size() != ptr->params_.size()) return false;
        for (size_t i = 0; i < params_.size(); i++) {
            if (!params_[i]->equal(ptr->params_[i])) return false;
        }
        return true;
    }
};

class TupleType : public Type {
public:
    std::vector<std::shared_ptr<Type> > types_;

    explicit TupleType(std::vector<std::shared_ptr<Type> > types)
        : types_(std::move(types)) {
    }

    TupleType &operator=(const Type &other) override {
        methods_ = other.methods_;
        if (const auto *ptr = dynamic_cast<const TupleType *>(&other)) {
            types_ = ptr->types_;
        } else {
            throw std::runtime_error("Cannot assign non-TupleType to TupleType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override { return TypeKind::Tuple; }

    [[nodiscard]] std::string toString() const override {
        std::string str = "(";
        for (const auto &type: types_) {
            str += type->toString();
            str += ", ";
        }
        str += ")";
        return str;
    }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other->getKind() != TypeKind::Tuple) return false;
        auto ptr = std::static_pointer_cast<TupleType>(other);
        if (types_.size() != ptr->types_.size()) return false;
        for (size_t i = 0; i < types_.size(); i++) {
            if (!types_[i]->equal(ptr->types_[i])) return false;
        }
        return true;
    }
};

class SliceType : public Type {
    std::shared_ptr<Type> type_;

public:
    explicit SliceType(std::shared_ptr<Type> type) : type_(std::move(type)) {
    }

    SliceType &operator=(const Type &other) override {
        methods_ = other.methods_;
        if (const auto *ptr = dynamic_cast<const SliceType *>(&other)) {
            type_ = ptr->type_;
        } else {
            throw std::runtime_error("Cannot assign non-SliceType to SliceType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override { return TypeKind::Slice; }

    [[nodiscard]] std::string toString() const override {
        return "[" + type_->toString() + "]";
    }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other->getKind() != TypeKind::Slice) return false;
        auto ptr = std::static_pointer_cast<SliceType>(other);
        return type_->equal(ptr->type_);
    }
};

class ArrayType : public Type {
public:
    std::shared_ptr<Type> base_;
    uint32_t length_;

    ArrayType(std::shared_ptr<Type> base, uint32_t length)
        : base_(std::move(base)), length_(length) {
    }

    ArrayType &operator=(const Type &other) override {
        methods_ = other.methods_;
        if (const auto *ptr = dynamic_cast<const ArrayType *>(&other)) {
            base_ = ptr->base_;
            length_ = ptr->length_;
        } else {
            throw std::runtime_error("Cannot assign non-ArrayType to ArrayType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override { return TypeKind::Array; }

    [[nodiscard]] std::string toString() const override {
        return "[" + base_->toString() + "; " + std::to_string(length_) + "]";
    }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other->getKind() != TypeKind::Array) return false;
        auto ptr = std::static_pointer_cast<ArrayType>(other);
        return length_ == ptr->length_ && base_->equal(ptr->base_);
    }
};

class InferredType : public Type {
    // TODO: Implementation
};

#endif //TYPE_H
