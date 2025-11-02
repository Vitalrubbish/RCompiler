#ifndef RCOMPILER_IRPROGRAM_H
#define RCOMPILER_IRPROGRAM_H
#include <vector>
#include <memory>

#include "IRFunction.h"
#include "IRInstruction.h"
#include "IR/IRNode.h"

class IRProgram : public IRNode {
public:
    std::vector<std::shared_ptr<StructDefInstruction>> structs;
    std::vector<std::shared_ptr<ConstVarDefInstruction>> constants;
    std::vector<std::shared_ptr<GlobalVarDefInstruction>> globals;
    std::vector<std::shared_ptr<IRFunction>> functions;

    IRProgram() = default;

	void print() override {
	    for (auto& it: constants) {
		    it->print();
	    	std::cout << '\n';
	    }

    	for (auto& it: structs) {
    		it->print();
    		std::cout << '\n';
    	}

    	for (auto& it: functions) {
    		it->print();
    		std::cout << '\n';
    	}
    }
};
#endif //RCOMPILER_IRPROGRAM_H