#ifndef SCOPE_H
#define SCOPE_H
#include <string>
#include <unordered_map>
#include "Symbol.h"
#include "Error.h"

class Scope {
    std::unordered_map<std::string, Symbol> symbols_;

public:
    bool in_loop_ = false;

    Scope() = default;

    void declare(const Symbol& symbol) {
        if (symbols_.find(symbol.name_) != symbols_.end()) {
            throw SemanticError("Semantic Error: Variable MultiDeclaration", symbol.pos_);
        }
        symbols_[symbol.name_] = symbol;
    }

    Symbol lookup(const std::string &name) {
        if (symbols_.find(name) != symbols_.end()) {
            return symbols_[name];
        }
        return Symbol{};
    }

    void ModifyType(const std::string &name, const std::shared_ptr<Type> &type) {
        symbols_[name].type_ = type;
    }
};
#endif //SCOPE_H
