#ifndef TYPE_H
#define TYPE_H
#include <string>
#include <vector>
enum class TypeKind {
    Primitive, Function, Struct, Enumeration, ConstantItem, Tuple,
    Slice, Inferred
};
class Type {
public:
    Type() = default;
    virtual ~Type() = 0;
    [[nodiscard]] virtual TypeKind getKind() const = 0;
    [[nodiscard]] virtual std::string toString() const = 0;
    virtual bool equal(Type* other) const = 0;
};

class PrimitiveType: public Type {
    std::string name_;
public:
    explicit PrimitiveType(const std::string& name) {
        name_ = name;
    }

    ~PrimitiveType() override = default;

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Primitive;
    }

    [[nodiscard]] std::string toString() const override {
        return name_;
    }

    bool equal(Type* other) const override {
        if (!other || other -> getKind() != TypeKind::Primitive) {
            return false;
        }
        auto* tmp = dynamic_cast<PrimitiveType*>(other);
        if (name_ == tmp -> name_) {
            return true;
        }
        return false;
    }
};

class FunctionType: public Type {
    std::vector<Type*> params_;
    Type* ret_;
public:
    FunctionType(const std::vector<Type*>& params, Type* ret) {
        params_ = params;
        ret_ = ret;
    }

    ~FunctionType() override {
        delete ret_;
        for (auto& it: params_) {
            delete it;
        }
    }

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Function;
    }

    [[nodiscard]] std::string toString() const override {
        std::string str;
        str += "fn ( ";
        for (auto& it: params_) {
            str += it -> toString();
            str += ", ";
        }
        str += ") -> ";
        str += ret_ -> toString();
        return str;
    }

    bool equal(Type* other) const override {
        if (!other || other -> getKind() != TypeKind::Function) {
            return false;
        }
        auto* tmp = dynamic_cast<FunctionType*>(other);
        if (tmp -> ret_ != ret_) {
            return false;
        }
        if (tmp -> params_.size() != params_.size()) {
            return false;
        }
        for (uint32_t i = 0; i < params_.size(); i++) {
            if (!params_[i] -> equal(tmp -> params_[i])) {
                return false;
            }
        }
        return true;
    }
};

struct StructMember {
    std::string name_;
    Type* type_;
};

class StructType: public Type {
    std::vector<StructMember> members_;
public:
    explicit StructType(const std::vector<StructMember>& members) {
        members_ = members;
    }

    ~StructType() override {
        for (auto& it: members_) {
            delete it.type_;
        }
    }

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Struct;
    }

    [[nodiscard]] std::string toString() const override {
        std::string str;
        str += "struct { ";
        for (auto& it: members_) {
            str += it.name_;
            str += ":";
            str += it.type_ -> toString();
            str += ", ";
        }
        str += "}";
        return str;
    }

    bool equal(Type* other) const override {
        if (!other || other -> getKind() != TypeKind::Struct) {
            return false;
        }
        auto* tmp = dynamic_cast<StructType*>(other);
        if (tmp -> members_.size() != members_.size()) {
            return false;
        }
        for (uint32_t i = 0; i < members_.size(); i++) {
            if (members_[i].name_ != tmp -> members_[i].name_ ||
                !members_[i].type_ -> equal(tmp -> members_[i].type_)) {
                return false;
            }
        }
        return true;
    }
};

class TupleType: public Type {
    std::vector<Type*> types_;
public:
    explicit TupleType(const std::vector<Type*>& types) {
        types_ = types;
    }

    ~TupleType() override {
        for (auto& it: types_) {
            delete it;
        }
    }

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Tuple;
    }

    [[nodiscard]] std::string toString() const override {
        std::string str;
        str += "( ";
        for (auto& it: types_) {
            str += it -> toString();
            str += ", ";
        }
        str += ")";
        return str;
    }

    bool equal(Type *other) const override {
        if (!other || other -> getKind() != TypeKind::Tuple) {
            return false;
        }
        auto* tmp = dynamic_cast<TupleType*> (other);
        if (types_.size() != tmp -> types_.size()) {
            return false;
        }
        for (uint32_t i = 0; i < types_.size(); i++) {
            if (!types_[i] -> equal(tmp -> types_[i])) {
                return false;
            }
        }
        return true;
    }
};

class SliceType: public Type {
    Type* type_ = nullptr;
public:
    explicit SliceType(Type* type) {
        type_ = type;
    }

    ~SliceType() override {
        delete type_;
    }

    [[nodiscard]] TypeKind getKind() const override {
        return TypeKind::Slice;
    }

    [[nodiscard]] std::string toString() const override {
        std::string str;
        str += "[ ";
        str += type_ -> toString();
        str += " ]";
        return str;
    }

    bool equal(Type *other) const override {
        if (!other || other -> getKind() != TypeKind::Slice) {
            return false;
        }
        auto* tmp = dynamic_cast<SliceType*> (other);
        if (!type_ -> equal(tmp -> type_)) {
            return false;
        }
        return true;
    }
};

class EnumerationType: public Type {
    // TODO Hard......
};

class InferredType: public Type {
    // TODO Hard......
};
#endif //TYPE_H