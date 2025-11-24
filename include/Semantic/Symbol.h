#ifndef SYMBOL_H
#define SYMBOL_H
#include <memory>
#include <string>
#include "Type.h"
#include "Position.h"

enum class SymbolType {
    Variable, Function, Type, Constant, Enumeration, Struct, None
};

struct Symbol {
    std::string name_;
    std::shared_ptr<Type> type_ = nullptr;
    SymbolType symbol_type_ = SymbolType::None;
    bool is_mutable_ = false;
    bool is_const_ = false;
    bool is_assigned_ = true;
    Position pos_;
	uint32_t scope_index_ = 0;

    Symbol() = default;

    Symbol(Position pos, const std::string &name, const std::shared_ptr<Type> &type, const SymbolType &symbol_type,
           const bool &is_mutable = false) {
        pos_ = pos;
        name_ = name;
        type_ = type;
        symbol_type_ = symbol_type;
        is_mutable_ = is_mutable;
    }

    void SetConst(bool is_const = false) {
        is_const_ = is_const;
    }

    void SetAssign(bool is_assigned = false) {
        is_assigned_ = is_assigned;
    }
};
#endif //SYMBOL_H
