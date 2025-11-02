#include "Semantic/ScopeManager.h"
#include "Semantic/ASTNode.h"

std::shared_ptr<Type> ScopeManager::lookupType(const std::shared_ptr<TypeNode> &type) {
    std::shared_ptr<Type> member_type;
    auto array_type = std::dynamic_pointer_cast<ArrayTypeNode>(type);
    auto ref_type = std::dynamic_pointer_cast<ReferenceTypeNode>(type);
    if (array_type) {
        return lookupArray(array_type);
    }
    if (ref_type) {
        return lookupRef(ref_type);
    }
    Symbol symbol = lookup(type->toString());
    return symbol.type_;
}


std::shared_ptr<ArrayType> ScopeManager::lookupArray(const std::shared_ptr<ArrayTypeNode> &type) {
    auto base = type -> type_;
    auto value = type -> expression_node_ -> value;
    auto* tmp = std::get_if<int64_t>(&value);
    int64_t size = 0;
    if (tmp) {
        size = *tmp;
    }
    std::shared_ptr<Type> base_ = lookupType(base);
    return std::make_shared<ArrayType>(base_, size);
}


std::shared_ptr<ReferenceType> ScopeManager::lookupRef(const std::shared_ptr<ReferenceTypeNode> &type) {
    auto base = type -> type_node_;
    std::shared_ptr<Type> base_ = lookupType(base);
    return std::make_shared<ReferenceType>(base_, type->is_mut_);
}
