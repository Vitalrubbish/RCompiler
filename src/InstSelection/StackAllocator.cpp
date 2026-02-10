#include "InstSelection/StackAllocator.h"
#include <iostream>

StackAllocator::StackAllocator(std::shared_ptr<ASMFunction> func) 
    : func(std::move(func)) {}

void StackAllocator::allocate_virtual_register(const std::string& reg_name, uint32_t size) {
    if (func->virtual_register_offset_map.find(reg_name) == func->virtual_register_offset_map.end()) {
        spilled_nodes[reg_name] = size;
    }
}

void StackAllocator::run() {
    uint32_t current_offset = 0; 

    // 1. Allocate for explicit StackObjects (from alloca)
    for (auto& obj : func->stack_objects) {
        // Align to 4 bytes
        if (current_offset % 4 != 0) {
            current_offset += (4 - (current_offset % 4));
        }
        obj->offset = current_offset;
        current_offset += obj->size;
    }

    // 2. Allocate for Spilled Virtual Registers
    for (const auto& [name, size] : spilled_nodes) {
        if (current_offset % 4 != 0) {
            current_offset += (4 - (current_offset % 4));
        }
        func->virtual_register_offset_map[name] = current_offset;
        current_offset += size;
    }

    // 3. Add the saved registers area (8 bytes for ra, s0)
    current_offset += 8; 

    // 4. Align total frame size to 16 bytes (RISC-V convention)
    if (current_offset % 16 != 0) {
        current_offset += (16 - (current_offset % 16));
    }

    func->stack_size = current_offset;
} 
