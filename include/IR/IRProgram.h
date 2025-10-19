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

    IRProgram(const std::vector<std::shared_ptr<StructDefInstruction>>& structs,
              const std::vector<std::shared_ptr<ConstVarDefInstruction>>& constants,
              const std::vector<std::shared_ptr<GlobalVarDefInstruction>>& globals,
              const std::vector<std::shared_ptr<IRFunction>>& functions) {
        this->structs = structs;
        this->constants = constants;
        this->globals = globals;
        this->functions = functions;
    }
};
#endif //RCOMPILER_IRPROGRAM_H