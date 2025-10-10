#ifndef IRVAR_H
#define IRVAR_H
#include <string>
#include "IRNode.h"

enum class VarType {
    Global, Local
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
    explicit GlobalVar(const std::string& name) : IRVar(name, VarType::Global) {}
};

class LocalVar : public IRVar {
    explicit LocalVar(const std::string& name) : IRVar(name, VarType::Local) {}
};
#endif //IRVAR_H
