#ifndef SCOPE_H
#define SCOPE_H
#include <string>
#include <unordered_map>
#include "Symbol.h"
#include "Error.h"

class Scope {
    std::unordered_map<std::string, Symbol> symbols_;
    std::unordered_map<std::string, std::shared_ptr<Type>> types_;

public:
    bool in_loop_ = false;

    Scope() = default;

    explicit Scope(bool in_loop) : in_loop_(in_loop) {}

    void declare(const Symbol& symbol) {
        if (symbols_.find(symbol.name_) != symbols_.end()) {
            throw SemanticError("Semantic Error: Variable MultiDeclaration", symbol.pos_);
        }
        symbols_[symbol.name_] = symbol;
        types_[symbol.name_] = symbol.type_;
    }

    Symbol lookup(const std::string &name) {
        if (symbols_.find(name) != symbols_.end()) {
            return symbols_[name];
        }
        return Symbol{};
    }

    void ModifyType(const std::string &name, const std::shared_ptr<Type> &type) {
        *types_[name] = *type;
    }
};
#endif //SCOPE_H
