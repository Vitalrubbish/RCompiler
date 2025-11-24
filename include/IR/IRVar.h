#ifndef IRVAR_H
#define IRVAR_H
#include <string>
#include <iostream>
#include "IRNode.h"
#include "IRManager.h"

class IRManager;
class IRBuilder;

extern IRManager ir_manager;
extern ScopeManager scope_manager;

enum class VarType {
    Global, Local, Constant
};

class IRVar : public IRNode {
public:
	std::string name;
	std::string scoped_name;
	std::string true_name;
	std::shared_ptr<IRType> type;
	VarType var_type;

	IRVar(const std::string& name_, VarType var_type_, const std::shared_ptr<IRType> &type_, bool change_name = true) {
		name = name_;
		if (change_name) {
			scoped_name = name + "." + std::to_string(scope_manager.current_scope->scope_index);
			if (ir_manager.variable_use_count.find(scoped_name) == ir_manager.variable_use_count.end()) {
				scope_manager.ir_declare(name);
				ir_manager.variable_use_count[scoped_name] = 0;
			} else {
				ir_manager.variable_use_count[scoped_name]++;
			}
		} else {
			uint32_t sym = scope_manager.ir_lookup(name);
			scoped_name = name + "." + std::to_string(sym);
		}
		true_name = scoped_name + '.' + std::to_string(ir_manager.variable_use_count[scoped_name]);
		var_type = var_type_;
		type = type_;
	}

	void print() override {}
};

class GlobalVar : public IRVar {
public:
    explicit GlobalVar(const std::string& name, const std::shared_ptr<IRType>& type) : IRVar(name, VarType::Global, type) {}
};

class LocalVar : public IRVar {
public:
    explicit LocalVar(const std::string& name, const std::shared_ptr<IRType>& type, bool change_name = true) :
		IRVar(name, VarType::Local, type, change_name) {}

	void print() override {
    	std::cout << "%" << true_name;
    }
};

class ConstVar : public LocalVar {
public:
    explicit ConstVar(const std::string& name, const std::shared_ptr<IRType>& type, bool change_name = true) :
		LocalVar(name, type, change_name) {}

	void print() override {
    	std::cout << "@" << true_name;
    }
};
#endif //IRVAR_H
