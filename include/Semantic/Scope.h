#ifndef SCOPE_H
#define SCOPE_H
#include <string>
#include <unordered_map>
#include "Symbol.h"
#include "Error.h"
class Scope {
    std::unordered_map<std::string, Symbol*> symbols_;
public:
    Scope() = default;

    void declare(Symbol* symbol) {
        if (symbols_.find(symbol -> name_) != symbols_.end()) {
            throw SemanticError("Semantic Error: Variable MultiDeclaration", symbol -> pos_);
        }
        symbols_[symbol -> name_] = symbol;
    }

    Symbol* lookup(const std::string& name) {
        if (symbols_.find(name) != symbols_.end()) {
            return symbols_[name];
        }
        return nullptr;
    }
};
#endif //SCOPE_H
