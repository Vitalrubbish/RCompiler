#ifndef SCOPEMANAGER_H
#define SCOPEMANAGER_H
#include <vector>
#include "Scope.h"

class TypeNode;
class ReferenceTypeNode;
class ArrayTypeNode;
using ConstValue = std::variant<int64_t, std::string>;

class ScopeManager {
public:
    std::shared_ptr<Scope> root;
    std::shared_ptr<Scope> current_scope;

    ScopeManager() {
        Position pos(0);
        root = std::make_shared<Scope>();
        current_scope = root;
        std::shared_ptr<Type> i32_type = std::make_shared<PrimitiveType>("i32");
        std::shared_ptr<Type> u32_type = std::make_shared<PrimitiveType>("u32");
        std::shared_ptr<Type> isize_type = std::make_shared<PrimitiveType>("isize");
        std::shared_ptr<Type> usize_type = std::make_shared<PrimitiveType>("usize");
        std::shared_ptr<Type> bool_type = std::make_shared<PrimitiveType>("bool");
        std::shared_ptr<Type> char_type = std::make_shared<PrimitiveType>("char");
        std::shared_ptr<Type> str_type = std::make_shared<PrimitiveType>("str");
        std::shared_ptr<Type> andStr_type = std::make_shared<ReferenceType>(str_type);
        std::shared_ptr<Type> string_type = std::make_shared<PrimitiveType>("String");
        std::shared_ptr<Type> cstring_type = std::make_shared<PrimitiveType>("cstring");
        std::shared_ptr<Type> never_type = std::make_shared<PrimitiveType>("never");
        std::shared_ptr<Type> void_type = std::make_shared<PrimitiveType>("void");
        Symbol i32(pos, "i32", i32_type, SymbolType::Type);
        Symbol u32(pos, "u32", u32_type, SymbolType::Type);
        Symbol isize(pos, "isize", isize_type, SymbolType::Type);
        Symbol usize(pos, "usize", usize_type, SymbolType::Type);
        Symbol bool_(pos, "bool", bool_type, SymbolType::Type);
        Symbol char_(pos, "char", char_type, SymbolType::Type);
        Symbol string_(pos, "str", str_type, SymbolType::Type);
        Symbol String_(pos, "String", string_type, SymbolType::Type);
        Symbol cstring_(pos, "cstring", cstring_type, SymbolType::Type);
        Symbol void_(pos, "void", void_type, SymbolType::Type);
        Symbol Void_(pos, "()", void_type, SymbolType::Type);
        Symbol never_(pos, "never", never_type, SymbolType::Type);
        declare(i32);
        declare(u32);
        declare(isize);
        declare(usize);
        declare(bool_);
        declare(char_);
        declare(string_);
        declare(String_);
        declare(cstring_);
        declare(void_);
        declare(Void_);
        declare(never_);

        std::shared_ptr<Type> print = std::make_shared<FunctionType>(
            std::vector{andStr_type},
            void_type
        );
        std::shared_ptr<Type> println = std::make_shared<FunctionType>(
            std::vector{andStr_type},
            void_type
        );
        std::shared_ptr<Type> printInt = std::make_shared<FunctionType>(
            std::vector{i32_type},
            void_type
        );
        std::shared_ptr<Type> printlnInt = std::make_shared<FunctionType>(
            std::vector{i32_type},
            void_type
        );
        std::shared_ptr<Type> getString = std::make_shared<FunctionType>(
            std::vector<std::shared_ptr<Type>>{},
            string_type
        );
        std::shared_ptr<Type> getInt = std::make_shared<FunctionType>(
            std::vector<std::shared_ptr<Type>>{},
            i32_type
        );
        std::shared_ptr<Type> exit = std::make_shared<FunctionType>(
            std::vector{i32_type},
            void_type
        );
        Symbol print_(pos, "print", print, SymbolType::Function);
        Symbol println_(pos, "println", println, SymbolType::Function);
        Symbol printInt_(pos, "printInt", printInt, SymbolType::Function);
        Symbol printlnInt_(pos, "printlnInt", printlnInt, SymbolType::Function);
        Symbol getString_(pos, "getString", getString, SymbolType::Function);
        Symbol getInt_(pos, "getInt", getInt, SymbolType::Function);
        Symbol exit_(pos, "exit", exit, SymbolType::Function);
        declare(print_);
        declare(println_);
        declare(printInt_);
        declare(printlnInt_);
        declare(getString_);
        declare(getInt_);
        declare(exit_);

        i32_type->methods_.emplace_back(
            Method{"to_string", std::make_shared<FunctionType>(
                std::vector<std::shared_ptr<Type>>{}, string_type)});
        string_type->methods_.emplace_back(
            Method{"len", std::make_shared<FunctionType>(
                std::vector<std::shared_ptr<Type>>{}, usize_type)});
    }

    ~ScopeManager() = default;

    void AddScope(){
        std::shared_ptr<Scope> new_scope = std::make_shared<Scope>();
        if (current_scope) {
            current_scope->AddNextLevelScope(new_scope);
            new_scope->parent_scope_ = current_scope;
            current_scope = new_scope;
        }
    }

    void PopScope() {
        if (current_scope->parent_scope_) {
            current_scope = current_scope->parent_scope_;
        } else {
            throw SemanticError("Semantic Error: Cannot pop root scope");
        }
    }

    void declare(const Symbol& symbol, bool multi_name_check = true) const {
        try {
            current_scope -> declare(symbol, multi_name_check);
        } catch (std::exception &) {
            throw;
        }
    }

    [[nodiscard]] Symbol lookup(const std::string& name) const {
        std::shared_ptr<Scope> cursor = current_scope;
        while (cursor != nullptr) {
            Symbol symbol = cursor -> lookup(name);
            if (symbol.symbol_type_ != SymbolType::None) {
                return symbol;
            }
            cursor = cursor -> parent_scope_;
        }
        throw SemanticError("Semantic Error: Symbol not found - " + name);
    }

    std::shared_ptr<Type> lookupType(const std::shared_ptr<TypeNode> &type);

    std::shared_ptr<IRArrayType> lookupArray(const std::shared_ptr<ArrayTypeNode> &type);

    std::shared_ptr<ReferenceType> lookupRef(const std::shared_ptr<ReferenceTypeNode> &type);

    void AddConstant(const std::string &name, const ConstValue& val) const {
        current_scope->value_map_[name] = val;
    }

    [[nodiscard]] ConstValue SearchValue(const std::string &name) const {
        std::shared_ptr<Scope> cursor = current_scope;
        while (cursor != nullptr) {
            Symbol symbol = cursor -> lookup(name);
            if (symbol.symbol_type_ != SymbolType::None) {
                return cursor ->value_map_[name];
            }
            cursor = cursor -> parent_scope_;
        }
        throw SemanticError("This Error should not be occurred, please check your code!!!");
    }

    void ModifyType(const std::string &name, const std::shared_ptr<Type>& type) const {
        current_scope -> ModifyType(name, type);
    }

    void RemoteModifyType(const std::string &name, const std::shared_ptr<Type>& type) const {
        std::shared_ptr<Scope> cursor = current_scope;
        while (cursor != nullptr) {
            Symbol symbol = cursor -> lookup(name);
            if (symbol.symbol_type_ != SymbolType::None) {
                cursor -> ModifyType(name, type);
                return;
            }
            cursor = cursor -> parent_scope_;
        }
        throw SemanticError("Semantic Error: Symbol not found - " + name);
    }
};
#endif //SCOPEMANAGER_H
