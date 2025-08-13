#ifndef TYPEMANAGER_H
#define TYPEMANAGER_H
#include <unordered_map>
#include "Type.h"
class TypeManager {
    std::unordered_map<std::string, Type*> primitive_map_;
    std::unordered_map<std::string, Type*> function_map_;
    std::unordered_map<std::string, Type*> struct_map_;
    std::unordered_map<std::string, Type*> tuple_map_;
    std::unordered_map<std::string, Type*> slice_map_;
};
#endif //TYPEMANAGER_H
