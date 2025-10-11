#ifndef IRBASICBLOCK_H
#define IRBASICBLOCK_H
#include <vector>
#include <string>
#include <memory>

#include "IRNode.h"


class IRInstruction;

class IRBasicBlock : public IRNode {
public:
    std::string label;
    std::vector<std::shared_ptr<IRInstruction>> instructions;

    IRBasicBlock(const std::string &label, const std::vector<std::shared_ptr<IRInstruction>> &instructions)
        : label(label), instructions(instructions) {}
};
#endif //IRBASICBLOCK_H
