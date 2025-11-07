#ifndef SCOPE_H
#define SCOPE_H
#include <string>
#include <unordered_map>
#include <variant>
#include "Symbol.h"
#include "Error.h"

using ConstValue = std::variant<int64_t,std::string>;


class Scope {
    std::unordered_map<std::string, Symbol> symbols_;
    std::unordered_map<std::string, std::shared_ptr<Type> > types_;

public:
	uint32_t scope_index;
    std::vector<std::shared_ptr<Scope> > next_level_scopes_;
    std::shared_ptr<Scope> parent_scope_;
    std::unordered_map<std::string, ConstValue> value_map_;
    uint32_t index = 0;

    Scope() = default;

    void declare(const Symbol &symbol, bool multi_name_check = true) {
        if (symbols_.find(symbol.name_) != symbols_.end() && multi_name_check) {
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

    void AddNextLevelScope(const std::shared_ptr<Scope> &scope) {
        next_level_scopes_.push_back(scope);
    }
};
#endif //SCOPE_H
