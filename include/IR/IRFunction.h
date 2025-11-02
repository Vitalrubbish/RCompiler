#ifndef IRFUNCTION_H
#define IRFUNCTION_H
#include <utility>

#include "IRBlock.h"
#include "IRType.h"
#include "IRManager.h"

class IRBasicBlock;

class IRFunctionParam : public IRNode {
public:
    std::shared_ptr<IRType> type;
    std::string name;

	IRFunctionParam(const std::shared_ptr<IRType>& type, std::string  name) : type(type), name(std::move(name)) {}

	void print() override {
		type->print();
		std::cout << " " << name;
	}
};

class IRFunction : public IRNode {
public:
    std::string name;
    std::shared_ptr<IRType> return_type;
    std::vector<IRFunctionParam> function_params;
    std::vector<std::shared_ptr<IRBasicBlock>> blocks;

    IRFunction(const std::string& name, const std::shared_ptr<IRType>& return_type,
               const std::vector<IRFunctionParam>& function_params) {
        this->name = name;
        this->return_type = return_type;
        this->function_params = function_params;
    }

	void print() override {
	    std::cout << "define ";
    	return_type->print();
    	std::cout << " @" << name << "(";
    	for (int32_t i = 0; i < function_params.size(); i++) {
    		function_params[i].print();
    		if (i != function_params.size() - 1) {
    			std::cout << ", ";
    		}
    	}
    	std::cout << ")\n";
    	std::cout << "{\n";
    	for (auto& it: blocks) {
    		it->print();
    	}
    	std::cout << "}\n";
    }
};
#endif //IRFUNCTION_H
