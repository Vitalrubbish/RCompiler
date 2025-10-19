#include "Semantic/Type.h"
#include "Semantic/ScopeManager.h"

extern ScopeManager scope_manager;

IRArrayType::IRArrayType(const std::shared_ptr<Type> &base, uint32_t length) {
    base_ = base;
    length_ = length;
    auto usize_type = scope_manager.lookup("usize").type_;
    auto len_ = std::make_shared<FunctionType>(std::vector<std::shared_ptr<Type>>(), usize_type);
    Method len{"len", len_};
    methods_.emplace_back(len);
}
