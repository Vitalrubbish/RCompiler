#ifndef RCOMPILER_ASMBLOCK_H
#define RCOMPILER_ASMBLOCK_H

#include <vector>
#include "ASMInstruction.h"

class ASMBlock {
public:
	std::string label;
	std::vector<std::shared_ptr<ASMInstruction>> instructions;

	explicit ASMBlock(const std::string &lbl) : label(lbl) {}

	void AddInstruction(const std::shared_ptr<ASMInstruction> &inst) {
		instructions.push_back(inst);
	}

	void print() {
		std::cout << label << ":\n";
		for (const auto &inst : instructions) {
			inst->print();
		}
	}
};

#endif //RCOMPILER_ASMBLOCK_H