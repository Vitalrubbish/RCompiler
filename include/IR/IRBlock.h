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
	std::string true_label;
    std::vector<std::shared_ptr<IRInstruction>> instructions;
	std::map<std::string, uint32_t> last_variable_index;

    explicit IRBasicBlock(const std::string& label)
        : label(label) {
	    if (ir_manager.label_count.find(label) == ir_manager.label_count.end()) {
		    ir_manager.label_count[label] = 0;
	    } else {
		    ir_manager.label_count[label]++;
	    }
    	true_label = label + "." + std::to_string(ir_manager.label_count[label]);
    }

	void print() override {
    	std::cout << true_label << ":\n";
	    for (auto& inst: instructions) {
		    inst->print();
	    	std::cout << '\n';
	    }
    }
};
#endif //IRBASICBLOCK_H
