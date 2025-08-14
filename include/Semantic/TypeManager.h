#ifndef TYPEMANAGER_H
#define TYPEMANAGER_H
#include <unordered_map>
#include "Type.h"
#include "Error.h"

class TypeManager {
    std::unordered_map<std::string, Type*> primitive_map_;
    std::unordered_map<std::string, Type*> function_map_;
    std::unordered_map<std::string, Type*> struct_map_;
    std::unordered_map<std::string, Type*> tuple_map_;
    std::unordered_map<std::string, Type*> slice_map_;
public:
    TypeManager() {
        const Position pos(0);
        PutPrimitive(pos, "i32", new PrimitiveType("i32"));
        PutPrimitive(pos, "u32", new PrimitiveType("u32"));
        PutPrimitive(pos, "isize", new PrimitiveType("isize"));
        PutPrimitive(pos, "usize", new PrimitiveType("usize"));
        PutPrimitive(pos, "bool", new PrimitiveType("bool"));
        PutPrimitive(pos, "string", new PrimitiveType("string"));
        PutPrimitive(pos, "cstring", new PrimitiveType("cstring"));
        PutPrimitive(pos, "char", new PrimitiveType("char"));
        PutPrimitive(pos, "void", new PrimitiveType("void"));
        Type* i32_type = GetPrimitive(pos, "i32");
        Type* string_type = GetPrimitive(pos, "string");
        Type* void_type = GetPrimitive(pos, "void");
        Type* u32_type = GetPrimitive(pos, "i32");
        PutFunction(pos, "print", new FunctionType({string_type}, void_type));
        PutFunction(pos, "println", new FunctionType({string_type}, void_type));
        PutFunction(pos, "printInt", new FunctionType({i32_type}, void_type));
        PutFunction(pos, "printlnInt", new FunctionType({i32_type}, void_type));
        PutFunction(pos, "getInt", new FunctionType({}, i32_type));
        PutFunction(pos, "getString", new FunctionType({}, string_type));
        PutFunction(pos, "exit", new FunctionType({i32_type}, void_type));
        i32_type -> methods_.emplace_back(
            Method{"to_string", new FunctionType({}, string_type)});
        string_type -> methods_.emplace_back(
            Method{"len", new FunctionType({}, u32_type)});
    }

    Type* GetPrimitive(Position pos, const std::string& name) {
        if (primitive_map_.find(name) == primitive_map_.end()) {
            throw SemanticError("Semantic Error: Primitive Type Not Found: " + name, pos);
        }
        return primitive_map_[name];
    }

    void PutPrimitive(Position pos, const std::string& name, Type* type) {
        if (primitive_map_.find(name) == primitive_map_.end()) {
            throw SemanticError("Semantic Error: Primitive Type MultiDeclaration: " + name, pos);
        }
        primitive_map_[name] = type;
    }

    Type* GetFunction(Position pos, const std::string& name) {
        if (function_map_.find(name) == function_map_.end()) {
            throw SemanticError("Semantic Error: Function Type Not Found: " + name, pos);
        }
        return function_map_[name];
    }

    void PutFunction(Position pos, const std::string& name, Type* type) {
        if (function_map_.find(name) == function_map_.end()) {
            throw SemanticError("Semantic Error: Function Type MultiDeclaration: " + name, pos);
        }
        function_map_[name] = type;
    }

    Type* GetStruct(Position pos, const std::string& name) {
        if (struct_map_.find(name) == struct_map_.end()) {
            throw SemanticError("Semantic Error: Struct Type Not Found: " + name, pos);
        }
        return struct_map_[name];
    }

    void PutStruct(Position pos, const std::string& name, Type* type) {
        if (struct_map_.find(name) == struct_map_.end()) {
            throw SemanticError("Semantic Error: Struct Type MultiDeclaration: " + name, pos);
        }
        struct_map_[name] = type;
    }
};
#endif
