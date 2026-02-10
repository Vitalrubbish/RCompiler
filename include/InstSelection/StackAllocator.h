#ifndef RCOMPILER_STACKALLOCATOR_H
#define RCOMPILER_STACKALLOCATOR_H

#include "ASMFunction.h"
#include <map>
#include <string>
#include <memory>

class StackAllocator {
public:
    explicit StackAllocator(std::shared_ptr<ASMFunction> func);

    // Call this to allocate a slot for a virtual register (e.g. spilling)
    void allocate_virtual_register(const std::string& reg_name, uint32_t size = 4);

    // Perform the layout calculation. 
    // Assigns offsets to all StackObjects and VirtualRegisters registered so far.
    // Updates func->stack_size.
    void run();

private:
    std::shared_ptr<ASMFunction> func;
    
    // We might need to store sizes of virtual registers if not standard
    std::map<std::string, uint32_t> spilled_nodes; 
};

#endif //RCOMPILER_STACKALLOCATOR_H
