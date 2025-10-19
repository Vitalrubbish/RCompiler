#ifndef IRVAR_H
#define IRVAR_H
#include <string>
#include "IRNode.h"

enum class VarType {
    Global, Local, Constant
};

class IRVar : public IRNode {
protected:
    std::string name;
    VarType var_type;
public:
    IRVar(const std::string& name_, VarType type_) {
        name = name_;
        var_type = type_;
    }
};

class GlobalVar : public IRVar {
public:
    explicit GlobalVar(const std::string& name) : IRVar(name, VarType::Global) {}
};

class LocalVar : public IRVar {
public:
    explicit LocalVar(const std::string& name) : IRVar(name, VarType::Local) {}
};

class ConstVar : public IRVar {
public:
    explicit ConstVar(const std::string& name) : IRVar(name, VarType::Constant) {}
};
#endif //IRVAR_H
