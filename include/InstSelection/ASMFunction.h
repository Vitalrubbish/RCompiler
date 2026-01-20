#ifndef RCOMPILER_ASMFUNCTION_H
#define RCOMPILER_ASMFUNCTION_H

#include "ASMBlock.h"
#include <vector>

class ASMFunction {
public:
	std::string name;
	std::vector<std::shared_ptr<ASMBlock>> blocks;

	explicit ASMFunction(const std::string &func_name) : name(func_name) {}

	void AddBlock(const std::shared_ptr<ASMBlock> &block) {
		blocks.push_back(block);
	}

	void print() {
		for (const auto &block : blocks) {
			block->print();
		}
	}
};

#endif //RCOMPILER_ASMFUNCTION_H