#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <stdexcept>
#include <variant>
#include <unordered_map>

class Type;
class FunctionType;

struct Method {
    std::string name_;
    std::shared_ptr<Type> type_;
};

enum class TypeKind {
    Primitive, Function, Struct, Enumeration, Tuple,
    Slice, Inferred, Array, Unit, Reference
};

using ConstValue = std::variant<int64_t,std::string>;

class Type {
public:
    std::unordered_map<std::string, bool> name_set;
    std::unordered_map<std::string, ConstValue> value_map;
    std::vector<Method> methods_{};
    std::vector<Method> inline_functions_{};
    std::vector<Method> constants_{};

    Type() = default;

    virtual ~Type() = default;

    virtual Type &operator=(const Type &other) = default;

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
        value_map = other.value_map;
        name_set = other.name_set;
        methods_ = other.methods_;
        inline_functions_ = other.inline_functions_;
        constants_ = other.constants_;
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
        auto ptr = std::dynamic_pointer_cast<PrimitiveType>(other);
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
        value_map = other.value_map;
        name_set = other.name_set;
        methods_ = other.methods_;
        inline_functions_ = other.inline_functions_;
        constants_ = other.constants_;
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
        auto ptr = std::dynamic_pointer_cast<StructType>(other);
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
        value_map = other.value_map;
        name_set = other.name_set;
        methods_ = other.methods_;
        inline_functions_ = other.inline_functions_;
        constants_ = other.constants_;
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
        auto ptr = std::dynamic_pointer_cast<EnumerationType>(other);
        return name_ == ptr->name_ && variants_ == ptr->variants_;
    }
};

class FunctionType : public Type {
public:
    bool have_self_ = false;
    bool is_mut_ = false;
    bool have_and_ = false;

    std::vector<std::shared_ptr<Type> > params_;
    std::shared_ptr<Type> ret_;

    FunctionType(std::vector<std::shared_ptr<Type> > params, std::shared_ptr<Type> ret)
        : params_(std::move(params)), ret_(std::move(ret)) {
    }

    void SetParam(bool have_self, bool have_and, bool is_mut) {
        have_self_ = have_self;
        have_and_ = have_and;
        is_mut_ = is_mut;
    }

    FunctionType &operator=(const Type &other) override {
        value_map = other.value_map;
        name_set = other.name_set;
        methods_ = other.methods_;
        inline_functions_ = other.inline_functions_;
        constants_ = other.constants_;
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
        auto ptr = std::dynamic_pointer_cast<FunctionType>(other);
        if (!ret_->equal(ptr->ret_) || params_.size() != ptr->params_.size()) return false;
        for (size_t i = 0; i < params_.size(); i++) {
            if (!params_[i]->equal(ptr->params_[i])) return false;
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
        value_map = other.value_map;
        name_set = other.name_set;
        methods_ = other.methods_;
        inline_functions_ = other.inline_functions_;
        constants_ = other.constants_;
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
        auto ptr = std::dynamic_pointer_cast<SliceType>(other);
        return type_->equal(ptr->type_);
    }
};

class IRArrayType : public Type {
public:
    std::shared_ptr<Type> base_;
    uint32_t length_;

    IRArrayType(const std::shared_ptr<Type> &base, uint32_t length);

    IRArrayType &operator=(const Type &other) override {
        value_map = other.value_map;
        name_set = other.name_set;
        methods_ = other.methods_;
        inline_functions_ = other.inline_functions_;
        constants_ = other.constants_;
        if (const auto *ptr = dynamic_cast<const IRArrayType *>(&other)) {
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
        auto ptr = std::dynamic_pointer_cast<IRArrayType>(other);
        return length_ == ptr->length_ && base_->equal(ptr->base_);
    }
};

class UnitType: public Type {
public:
    UnitType() = default;

    UnitType &operator=(const Type& other) override {
        value_map = other.value_map;
        name_set = other.name_set;
        methods_ = other.methods_;
        inline_functions_ = other.inline_functions_;
        constants_ = other.constants_;
        const auto *ptr = dynamic_cast<const UnitType *>(&other);
        if (!ptr) {
            throw std::runtime_error("Cannot assign non-UnitType to ArrayType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Unit;
    }

    [[nodiscard]] std::string toString() const override {
        return "()";
    }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other -> getKind() != TypeKind::Unit) {
            return false;
        }
        return true;
    }
};

class ReferenceType: public Type {
public:
    std::shared_ptr<Type> type_;
    bool is_mut_ = false;

    explicit ReferenceType(const std::shared_ptr<Type> &type, bool is_mut = false) {
        type_ = type;
        is_mut_ = is_mut;
    }

    ReferenceType &operator= (const Type& other) override {
        value_map = other.value_map;
        name_set = other.name_set;
        methods_ = other.methods_;
        inline_functions_ = other.inline_functions_;
        constants_ = other.constants_;
        if (const auto *ptr = dynamic_cast<const ReferenceType *>(&other)) {
            type_ = ptr -> type_;
            is_mut_ = ptr -> is_mut_;
        } else {
            throw std::runtime_error("Cannot assign non-ReferenceType to ReferenceType");
        }
        return *this;
    }

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Reference;
    }

    [[nodiscard]] std::string toString() const override {
        return "&" + type_ -> toString();
    }

    [[nodiscard]] bool equal(const std::shared_ptr<Type> &other) const override {
        if (!other || other -> getKind() != TypeKind::Reference) {
            return false;
        }
        auto ptr = std::dynamic_pointer_cast<ReferenceType>(other);
        if (is_mut_ != ptr->is_mut_ && is_mut_) { return false; }
        return type_->equal(ptr->type_);
    }
};



#endif //TYPE_H