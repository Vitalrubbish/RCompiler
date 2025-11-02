#ifndef IRBASICBLOCK_H
#define IRBASICBLOCK_H
#include <utility>
#include <vector>
#include <string>
#include <memory>

#include "IRInstruction.h"
#include "IRNode.h"


class IRInstruction;

class IRBasicBlock : public IRNode {
public:
    std::string label;
    std::vector<std::shared_ptr<IRInstruction>> instructions;

    explicit IRBasicBlock(std::string label)
        : label(std::move(label)){}

	void print() override {
    	std::cout << label << ":\n";
	    for (auto& inst: instructions) {
		    inst->print();
	    	std::cout << '\n';
	    }
    }
};
#endif //IRBASICBLOCK_H
