#ifndef SCOPEMANAGER_H
#define SCOPEMANAGER_H
#include <vector>
#include "Scope.h"


class ScopeManager {
    std::vector<Scope> scopes_;

public:
    ScopeManager() {
        Position pos(0);
        pushBack();
        Type* i32_type = new PrimitiveType("i32");
        Type* u32_type = new PrimitiveType("u32");
        Type* bool_type = new PrimitiveType("bool");
        Type* char_type = new PrimitiveType("char");
        Type* string_type = new PrimitiveType("string");
        Type* cstring_type = new PrimitiveType("cstring");
        Type* void_type = new PrimitiveType("void");
        Symbol i32(pos, "i32", i32_type, SymbolType::Type);
        Symbol u32(pos, "u32", u32_type, SymbolType::Type);
        Symbol bool_(pos, "bool", bool_type, SymbolType::Type);
        Symbol char_(pos, "char", char_type, SymbolType::Type);
        Symbol string_(pos, "string", string_type, SymbolType::Type);
        Symbol cstring_(pos, "cstring", cstring_type, SymbolType::Type);
        Symbol void_(pos, "void", void_type, SymbolType::Type);
        declare(i32);
        declare(u32);
        declare(bool_);
        declare(char_);
        declare(string_);
        declare(cstring_);
        declare(void_);

        Type* print = new FunctionType({string_type}, void_type);
        Type* println = new FunctionType({string_type}, void_type);
        Type* printInt = new FunctionType({i32_type}, void_type);
        Type* printlnInt = new FunctionType({i32_type}, void_type);
        Type* getString = new FunctionType({void_type}, string_type);
        Type* getInt = new FunctionType({void_type}, i32_type);
        Symbol print_(pos, "print", print, SymbolType::Function);
        Symbol println_(pos, "println", println, SymbolType::Function);
        Symbol printInt_(pos, "print_int", printInt, SymbolType::Function);
        Symbol printlnInt_(pos, "println_int", printlnInt, SymbolType::Function);
        Symbol getString_(pos, "get_string", getString, SymbolType::Function);
        Symbol getInt_(pos, "get_int", getInt, SymbolType::Function);
        declare(print_);
        declare(println_);
        declare(printInt_);
        declare(printlnInt_);
        declare(getString_);
        declare(getInt_);

        i32_type->methods_.emplace_back(
            Method{"to_string", new FunctionType({}, string_type)});
        string_type->methods_.emplace_back(
            Method{"len", new FunctionType({}, u32_type)});
    }

    void pushBack() {
        scopes_.emplace_back();
    }

    void popBack() {
        scopes_.pop_back();
    }

    void declare(const Symbol& symbol) {
        try {
            uint32_t len = scopes_.size();
            scopes_[len - 1].declare(symbol);
        } catch (std::exception &) {
            throw;
        }
    }

    Symbol lookup(const std::string &name) {
        auto len = static_cast<int32_t>(scopes_.size());
        for (int32_t i = len - 1; i >= 0; --i) {
            Symbol ret = scopes_[i].lookup(name);
            if (ret.symbol_type_ == SymbolType::None) {
                return ret;
            }
        }
        throw SemanticError("Semantic Error: Symbol not declared: " + name);
    }

    void ModifyType(const std::string &name, Type* type) {
        uint32_t len = scopes_.size();
        scopes_[len - 1].ModifyType(name, type);
    }
};
#endif //SCOPEMANAGER_H
