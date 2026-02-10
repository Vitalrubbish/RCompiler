#ifndef RCOMPILER_ASMMODULE_H
#define RCOMPILER_ASMMODULE_H

#include <vector>
#include <memory>
#include "ASMNode.h"
#include "ASMFunction.h"
#include "ASMGlobalVariable.h"

class ASMModule : public ASMNode {
public:
	std::vector<std::shared_ptr<ASMFunction>> functions;
	std::vector<std::shared_ptr<ASMGlobalVariable>> global_vars;

	ASMModule() = default;

	void print() override {
		for (auto &global_var: global_vars) {
			global_var->print();
			std::cout << '\n';
		}
		for (auto &function: functions) {
			function->print();
			std::cout << '\n';
		}
	}
};

#endif //RCOMPILER_ASMMODULE_H