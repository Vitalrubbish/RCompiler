#ifndef IRFUNCTION_H
#define IRFUNCTION_H
#include "IRType.h"
#include "IRVar.h"

class IRBlock;

class IRFunctionParam : public IRNode {
    std::shared_ptr<IRType> type;
    std::string name;
};

class IRFunction : public IRNode {
public:
    std::string name;
    std::shared_ptr<IRType> return_type;
    std::vector<IRFunctionParam> function_params;
    std::shared_ptr<IRBlock> block;

    IRFunction(const std::string& name, const std::shared_ptr<IRType>& return_type,
               const std::vector<IRFunctionParam>& function_params) {
        this->name = name;
        this->return_type = return_type;
        this->function_params = function_params;
    }
};


#endif //IRFUNCTION_H
