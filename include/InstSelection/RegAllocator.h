#ifndef RCOMPILER_REGALLOCATOR_H
#define RCOMPILER_REGALLOCATOR_H

#include "ASMModule.h"
#include <memory>
#include <vector>
#include <string>

class RegAllocator {
public:
    RegAllocator() = default;
    ~RegAllocator() = default;

    void run(std::shared_ptr<ASMModule> module) {
        // allocate registers, now we just consider using t0, t1 and t2.
        for (auto& func : module->functions) {
            for (auto& block : func->blocks) {
                allocateRegisters(block, func);
            }
        }
    }

    void allocateRegisters(std::shared_ptr<ASMBlock> block, std::shared_ptr<ASMFunction> func) {
        std::vector<std::shared_ptr<ASMInstruction>> new_insts;
        
        auto t0 = std::make_shared<Register>(5, true); // t0
        auto t1 = std::make_shared<Register>(6, true); // t1
        auto t2 = std::make_shared<Register>(7, true); // t2
        auto sp = std::make_shared<Register>(2, true); // sp

        for (auto& inst: block->instructions) {
            // Handle StackObjects in operands (mainly for ADDI used in alloca)
            // Using dynamic casts for commonly used instructions with immediates that might be StackObjects
            if(auto addi = std::dynamic_pointer_cast<ASMAddiInstruction>(inst)) {
                 if (addi->imm->type == OperandType::STACK_OBJECT) {
                    auto stack_obj = std::dynamic_pointer_cast<StackObject>(addi->imm);
                    addi->imm = std::make_shared<Immediate>(stack_obj->offset);
                 }
            } else if (auto lw = std::dynamic_pointer_cast<ASMLwInstruction>(inst)) {
                 if (lw->imm->type == OperandType::STACK_OBJECT) {
                    auto stack_obj = std::dynamic_pointer_cast<StackObject>(lw->imm);
                    lw->imm = std::make_shared<Immediate>(stack_obj->offset);
                 }
            } else if (auto sw = std::dynamic_pointer_cast<ASMSwInstruction>(inst)) {
                 if (sw->imm->type == OperandType::STACK_OBJECT) {
                    auto stack_obj = std::dynamic_pointer_cast<StackObject>(sw->imm);
                    sw->imm = std::make_shared<Immediate>(stack_obj->offset);
                 }
            }

            // Spill-All Logic
            if (inst->has_rs1()) {
                auto& rs1 = inst->get_rs1();
                if (rs1->type == OperandType::VIRTUAL_REGISTER) {
                    auto vreg = std::dynamic_pointer_cast<Register>(rs1);
                    std::string vreg_name = "v" + std::to_string(vreg->index);
                    // If not found, it might be a temp vreg? Assume map has it.
                    if (func->virtual_register_offset_map.count(vreg_name)) {
                        int offset = func->virtual_register_offset_map[vreg_name];
                        auto offset_imm = std::make_shared<Immediate>(offset);
                        // lw t0, offset(sp)
                        auto load_inst = std::make_shared<ASMLwInstruction>(t0, sp, offset_imm);
                        new_insts.push_back(load_inst);
                        rs1 = t0; // Replace rs1 with t0
                    }
                }
            }

            if (inst->has_rs2()) {
                auto& rs2 = inst->get_rs2();
                if (rs2->type == OperandType::VIRTUAL_REGISTER) {
                    auto vreg = std::dynamic_pointer_cast<Register>(rs2);
                    std::string vreg_name = "v" + std::to_string(vreg->index);
                    if (func->virtual_register_offset_map.count(vreg_name)) {
                        int offset = func->virtual_register_offset_map[vreg_name];
                        auto offset_imm = std::make_shared<Immediate>(offset);
                        // lw t1, offset(sp)
                        auto load_inst = std::make_shared<ASMLwInstruction>(t1, sp, offset_imm);
                        new_insts.push_back(load_inst);
                        rs2 = t1; // Replace rs2 with t1
                    }
                }
            }

            new_insts.push_back(inst);

            if (inst->has_rd()) {
                auto& rd = inst->get_rd();
                if (rd->type == OperandType::VIRTUAL_REGISTER) {
                    auto vreg = std::dynamic_pointer_cast<Register>(rd);
                    std::string vreg_name = "v" + std::to_string(vreg->index);
                    if (func->virtual_register_offset_map.count(vreg_name)) {
                        int offset = func->virtual_register_offset_map[vreg_name];
                        auto offset_imm = std::make_shared<Immediate>(offset);
                        
                        // Replace rd with t0 (reuse t0)
                        rd = t0;
                        
                        // sw t0, offset(sp)
                        auto store_inst = std::make_shared<ASMSwInstruction>(t0, sp, offset_imm);
                        new_insts.push_back(store_inst);
                    }
                }
            }
        }
        
        block->instructions = new_insts;
    }
};

#endif //RCOMPILER_REGALLOCATOR_H