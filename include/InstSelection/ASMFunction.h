#ifndef RCOMPILER_ASMFUNCTION_H
#define RCOMPILER_ASMFUNCTION_H

#include <utility>
#include <vector>
#include <map>

#include "ASMBlock.h"
#include "ASMNode.h"

class ASMFunction : public ASMNode {
public:
	std::string name;
	std::vector<std::shared_ptr<ASMBlock>> blocks;
	std::vector<std::shared_ptr<StackObject>> stack_objects;
	std::map<std::string, uint32_t> virtual_register_offset_map;
	uint32_t stack_size = 8; // Start with 8 bytes for ra and s0

	explicit ASMFunction(std::string func_name) : name(std::move(func_name)) {}

	void AddBlock(const std::shared_ptr<ASMBlock> &block) {
		blocks.push_back(block);
	}

	std::shared_ptr<StackObject> AddStackObject(uint32_t size) {
		uint32_t stack_object_id = stack_objects.size();
		auto stack_obj = std::make_shared<StackObject>(stack_object_id ,size);
		stack_objects.push_back(stack_obj);
		stack_size += size;
		return stack_obj;
	}

	void print() override {
		for (const auto &block : blocks) {
			block->print();
			std::cout << '\n';
		}
	}
};

#endif //RCOMPILER_ASMFUNCTION_H