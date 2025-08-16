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
    Position pos_;

    Symbol() = default;

    Symbol(Position pos, const std::string &name, const std::shared_ptr<Type> &type, const SymbolType &symbol_type,
           const bool &is_mutable = false) {
        pos_ = pos;
        name_ = name;
        type_ = type;
        symbol_type_ = symbol_type;
        is_mutable_ = is_mutable;
    }
};
#endif //SYMBOL_H
