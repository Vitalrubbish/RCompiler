#ifndef RCOMPILER_ASMGLOBALVARIABLE_H
#define RCOMPILER_ASMGLOBALVARIABLE_H

#include <string>
#include <memory>
#include <iostream>
#include <utility>
#include "IR/IRType.h"
#include "IR/IRLiteral.h"

class ASMGlobalVariable {
public:
    std::string name;
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRLiteral> init_val;

    ASMGlobalVariable(std::string name, std::shared_ptr<IRType> type, std::shared_ptr<IRLiteral> init_val)
        : name(std::move(name)), type(std::move(type)), init_val(std::move(init_val)) {}

    void print() {
        std::cout << "\t.globl\t" << name << "\n";
        std::cout << "\t.data\n";
        std::cout << "\t.align\t2\n";
        std::cout << "\t.type\t" << name << ", @object\n";
        std::cout << "\t.size\t" << name << ", " << type->size << "\n";
        std::cout << name << ":\n";
        if (init_val) {
            if (auto val = std::dynamic_pointer_cast<LiteralInt>(init_val)) {
                std::cout << "\t.word\t" << val->getValue() << "\n";
            } else if (auto val = std::dynamic_pointer_cast<LiteralBool>(init_val)) {
                std::cout << "\t.byte\t" << (val->value ? 1 : 0) << "\n";
            } else {
                std::cout << "\t.zero\t" << type->size << "\n";
            }
        } else {
            std::cout << "\t.zero\t" << type->size << "\n";
        }
        std::cout << "\n";
    }
};

#endif //RCOMPILER_ASMGLOBALVARIABLE_H
