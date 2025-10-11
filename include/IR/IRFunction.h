#ifndef IRFUNCTION_H
#define IRFUNCTION_H
#include "IRType.h"
#include "IRVar.h"

class IRBasicBlock;

class IRFunctionParam : public IRNode {
    std::shared_ptr<IRType> type;
    std::string name;
};

class IRFunction : public IRNode {
public:
    std::string name;
    std::shared_ptr<IRType> return_type;
    std::vector<IRFunctionParam> function_params;
    std::vector<std::shared_ptr<IRBasicBlock>> blocks;

    IRFunction(const std::string& name, const std::shared_ptr<IRType>& return_type,
               const std::vector<IRFunctionParam>& function_params,
               const std::vector<std::shared_ptr<IRBasicBlock>>& blocks) {
        this->name = name;
        this->return_type = return_type;
        this->function_params = function_params;
        this->blocks = blocks;
    }
};
#endif //IRFUNCTION_H
