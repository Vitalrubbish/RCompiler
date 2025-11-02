#ifndef RCOMPILER_IRBUIDER_H
#define RCOMPILER_IRBUIDER_H
#include <map>
#include <memory>
#include <vector>

#include "IR/IRType.h"
#include "Semantic/Type.h"

class IRVar;
class IRFunction;

class IRManager {
public:
	std::map<std::shared_ptr<Type>, std::shared_ptr<IRType>, std::owner_less<std::shared_ptr<Type>>> type_map_;
	std::map<std::string, uint32_t> variable_use_count;
	std::map<std::string, std::shared_ptr<IRFunction>> function_map_;

	std::shared_ptr<IRType> GetIRType(const std::shared_ptr<Type>& type) {
		for (auto& it: type_map_) {
			if (it.first -> equal(type)) {
				return it.second;
			}
		}
	  	return nullptr;
	}

	void AddType(const std::shared_ptr<Type>& type) {
		auto struct_type = std::dynamic_pointer_cast<StructType>(type);
		if (struct_type) {
			AddStructType(struct_type);
		}
		auto array_type = std::dynamic_pointer_cast<ArrayType>(type);
		if (array_type) {
			AddArrayType(array_type);
		}
		// TODO: handle other kinds of types (array/pointer/function/primitive) if needed
	}

	void AddStructType(const std::shared_ptr<StructType>& type) {
		std::vector<std::shared_ptr<IRType>> member_types;
		for (auto& it: type->members_) {
			auto ir_type = GetIRType(it.type_);
			if (!ir_type) {
				// add the member's type (not the struct itself) and then get it
				AddType(it.type_);
				ir_type = GetIRType(it.type_);
			}
			member_types.emplace_back(ir_type);
		}
		auto struct_type = std::make_shared<IRStructType>(type->name_, member_types);
		type_map_[type] = struct_type;
	}

	void AddArrayType(const std::shared_ptr<ArrayType>& type) {

	}

};
#endif //RCOMPILER_IRBUIDER_H