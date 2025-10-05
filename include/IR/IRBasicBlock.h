#ifndef IRBASICBLOCK_H
#define IRBASICBLOCK_H
#include <vector>
#include <memory>

#include "IRUser.h"

class IRInstruction;

class IRBasicBlock : public IRUser {
public:
    std::vector<std::shared_ptr<IRInstruction>> instructions;


};
#endif //IRBASICBLOCK_H
