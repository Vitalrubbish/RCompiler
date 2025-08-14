#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include "Type.h"
#include "Position.h"

enum class SymbolType {
    Variable, Function, Type, Constant, Enumeration, Struct
};

struct Symbol {
    std::string name_;
    Type *type_;
    SymbolType symbol_type_;
    bool is_mutable_ = false;
    Position pos_;

    Symbol(Position pos, const std::string &name, Type *type, const SymbolType &symbol_type,
           const bool &is_mutable) {
        pos_ = pos;
        name_ = name;
        type_ = type;
        symbol_type_ = symbol_type;
        is_mutable_ = is_mutable;
    }
};
#endif //SYMBOL_H
