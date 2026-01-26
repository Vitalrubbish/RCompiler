#ifndef RCOMPILER_ASMFUNCTION_H
#define RCOMPILER_ASMFUNCTION_H

#include "ASMBlock.h"
#include <vector>

class ASMFunction {
public:
	std::string name;
	std::vector<std::shared_ptr<ASMBlock>> blocks;
	std::vector<std::shared_ptr<StackObject>> stack_objects;

	explicit ASMFunction(const std::string &func_name) : name(func_name) {}

	void AddBlock(const std::shared_ptr<ASMBlock> &block) {
		blocks.push_back(block);
	}

	void AddStackObject(uint32_t size) {
		uint32_t stack_object_id = stack_objects.size();
		auto stack_obj = std::make_shared<StackObject>(stack_object_id ,size);
		stack_objects.push_back(stack_obj);
	}

	void print() {
		for (const auto &block : blocks) {
			block->print();
		}
	}
};

#endif //RCOMPILER_ASMFUNCTION_H