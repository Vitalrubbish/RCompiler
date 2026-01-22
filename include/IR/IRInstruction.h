#ifndef IRINSTRUCTION_H
#define IRINSTRUCTION_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "IRLiteral.h"
#include "IRNode.h"
#include "IRType.h"
#include "IRVar.h"
#include "IRVisitor.h"

class IRVar;
class IRType;
class IRVisitor;
class IRLiteral;

/**************** ENUM TYPES ****************/
enum class OpType {
    Ret, CondBr, UncondBr,
    Add, Sub, Mul, SDiv, Srem, Shl, AShr, And, Or, Xor,
    Alloca, Load, Store, GetElementPtr,
    ICmp, CallWithRet, CallWithoutRet, Phi, Select, UDiv, Urem
};

/**************** BASE CLASS ****************/
class IRInstruction : public IRNode {
protected:
    OpType op_type{};
public:
    IRInstruction() = default;

	void print() override {}
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class UnreachableInstruction: public IRInstruction {
	void print() override {
		std::cout << "\tunreachable";
	}
	public:
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class BinaryOpInstruction : public IRInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> op1;
    std::shared_ptr<IRVar> op2;

    BinaryOpInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
        const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2, OpType op_type)
            : result(result), type(type), op1(op1), op2(op2) {
        this->op_type = op_type;
    }
};

class ControlInstruction : public IRInstruction {
public:
    explicit ControlInstruction(OpType op_type) {
        this->op_type = op_type;
    }
};

class MemoryInstruction : public IRInstruction {
public:
    explicit MemoryInstruction(OpType op_type) {
        this->op_type = op_type;
    }
};

/**************** DERIVED CLASS ****************/
class AddInstruction : public BinaryOpInstruction {
public:
	int imm1 = 0;
	int imm2 = 0;

    AddInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
        const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Add) {}

	AddInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
			const int &imm1_, const int &imm2_)
				: BinaryOpInstruction(result, type, nullptr, nullptr, OpType::Add) {
	    imm1 = imm1_;
    	imm2 = imm2_;
    }

	void print() override {
    	std::cout << '\t';
	    result->print();
    	std::cout << " = add ";
    	type->print();
    	std::cout << ' ';
    	if (op1) {
    		op1->print();
    	} else {
    		std::cout << imm1;
    	}
    	std::cout << ", ";
    	if (op2) {
    		op2->print();
    	} else {
    		std::cout << imm2;
    	}
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class SubInstruction : public BinaryOpInstruction {
public:
    SubInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Sub) {}

	SubInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
			const std::shared_ptr<IRVar> &op)
				: BinaryOpInstruction(result, type, nullptr, op, OpType::Sub) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = sub ";
    	type->print();
    	std::cout << ' ';
    	if (op1) {
    		op1->print();
    	} else {
    		std::cout << 0;
    	}
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class MulInstruction : public BinaryOpInstruction {
public:
    MulInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Mul) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = mul ";
    	type->print();
    	std::cout << ' ';
    	op1->print();
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class SDivInstruction : public BinaryOpInstruction {
public:
    SDivInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::SDiv) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = sdiv ";
    	type->print();
    	std::cout << ' ';
    	op1->print();
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class UDivInstruction : public BinaryOpInstruction {
public:
	UDivInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
				   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
			: BinaryOpInstruction(result, type, op1, op2, OpType::UDiv) {}

	void print() override {
		std::cout << '\t';
		result->print();
		std::cout << " = udiv ";
		type->print();
		std::cout << ' ';
		op1->print();
		std::cout << ", ";
		op2->print();
	}
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};


class SremInstruction : public BinaryOpInstruction {
public:
    SremInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Srem) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = srem ";
    	type->print();
    	std::cout << ' ';
    	op1->print();
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class UremInstruction : public BinaryOpInstruction {
public:
	UremInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
				   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
			: BinaryOpInstruction(result, type, op1, op2, OpType::Urem) {}

	void print() override {
		std::cout << '\t';
		result->print();
		std::cout << " = urem ";
		type->print();
		std::cout << ' ';
		op1->print();
		std::cout << ", ";
		op2->print();
	}
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class ShlInstruction : public BinaryOpInstruction {
public:
    ShlInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Shl) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = shl ";
    	type->print();
    	std::cout << ' ';
    	op1->print();
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class AShrInstruction : public BinaryOpInstruction {
public:
    AShrInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::AShr) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = ashr ";
    	type->print();
    	std::cout << ' ';
    	op1->print();
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class AndInstruction : public BinaryOpInstruction {
public:
    AndInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::And) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = and ";
    	type->print();
    	std::cout << ' ';
    	op1->print();
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class OrInstruction : public BinaryOpInstruction {
public:
    OrInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Or) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = or ";
    	type->print();
    	std::cout << ' ';
    	op1->print();
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class XorInstruction : public BinaryOpInstruction {
public:
    XorInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Xor) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
    	std::cout << " = xor ";
    	type->print();
    	std::cout << ' ';
    	op1->print();
    	std::cout << ", ";
    	op2->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class RetInstruction : public ControlInstruction {
public:
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> var;

    RetInstruction(const std::shared_ptr<IRType> &type, const std::shared_ptr<IRVar> &var)
            : ControlInstruction(OpType::Ret), type(type), var(var) {}

	void print() override {
	    std::cout << "\tret ";
    	if (type) {
    		type->print();
    	}
    	std::cout << " ";
    	if (var) {
    		var->print();
    	}
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};


class ConditionalBrInstruction : public ControlInstruction {
public:
    std::shared_ptr<IRVar> condition;
    std::string if_true;
    std::string if_false;

    ConditionalBrInstruction(const std::shared_ptr<IRVar> &condition,
                             std::string if_true, std::string if_false)
            : ControlInstruction(OpType::CondBr), condition(condition),
              if_true(std::move(if_true)), if_false(std::move(if_false)) {}

	void print() override {
	    std::cout << "\tbr i1 ";
    	condition->print();
    	std::cout << ", label %" << if_true << ", label %" << if_false;
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class UnconditionalBrInstruction : public ControlInstruction {
public:
    std::string label;

    explicit UnconditionalBrInstruction(std::string label)
            : ControlInstruction(OpType::UncondBr), label(std::move(label)) {}

	void print() override {
	    std::cout << "\tbr label %" << label;
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class ZextInstruction : public IRInstruction {
public:
	std::shared_ptr<IRVar> result;
	std::shared_ptr<IRType> type1;
	std::shared_ptr<IRVar> op;
	std::shared_ptr<IRType> type2;

	ZextInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type1,
				   const std::shared_ptr<IRVar> &op, const std::shared_ptr<IRType> &type2) : IRInstruction() {
		this->result = result;
		this->type1 = type1;
		this->op = op;
		this->type2 = type2;
	}

	void print() override {
		std::cout << '\t';
		result->print();
		std::cout << " = zext ";
		type1->print();
		std::cout << ' ';
		op->print();
		std::cout << " to ";
		type2->print();
	}
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class AllocaInstruction : public MemoryInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;

    AllocaInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type)
            : MemoryInstruction(OpType::Alloca), result(result), type(type) {}

	void print() override {
    	std::cout << '\t';
	    result->print();
    	std::cout << " = alloca ";
    	if (type) {
    		type->print();
    	}
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class LoadInstruction : public MemoryInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> ptr;

    LoadInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                    const std::shared_ptr<IRVar> &ptr)
            : MemoryInstruction(OpType::Load), result(result), type(type), ptr(ptr) {}

	void print() override {
    	std::cout << '\t';
    	result->print();
	    std::cout << " = load ";
    	if (type) {
    		type->print();
    	}
    	std::cout << ", ptr ";
    	if (ptr) {
    		ptr->print();
    	}
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class StoreInstruction : public MemoryInstruction {
public:
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRLiteral> literal;
	std::shared_ptr<IRVar> var;
    std::shared_ptr<IRVar> ptr;

    StoreInstruction(const std::shared_ptr<IRType> &type, const std::shared_ptr<IRLiteral> &literal,
                     const std::shared_ptr<IRVar> &ptr)
            : MemoryInstruction(OpType::Store), type(type), literal(literal), ptr(ptr) {}

	StoreInstruction(const std::shared_ptr<IRType> &type, const std::shared_ptr<IRVar> &var,
					 const std::shared_ptr<IRVar>& ptr)
	        : MemoryInstruction(OpType::Store), type(type), var(var), ptr(ptr) {}

	void print() override {
	    std::cout << "\tstore ";
    	if (literal) {
    		literal->print();
    	}
    	if (var) {
    		if (type) {
    			type->print();
    		}
    		std::cout << " ";
    		var->print();
    	}
    	std::cout << ", ptr ";
    	ptr->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class GetElementPtrInstruction : public MemoryInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> ptr;
    std::vector<std::shared_ptr<IRType>> types;
    std::vector<std::shared_ptr<IRVar>> indexes;
	std::vector<std::shared_ptr<IRLiteral>> literals;

    GetElementPtrInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                             const std::shared_ptr<IRVar> &ptr,
                             const std::vector<std::shared_ptr<IRType>> &types,
                             const std::vector<std::shared_ptr<IRVar>> &indexes)
            : MemoryInstruction(OpType::GetElementPtr), result(result), type(type), ptr(ptr),
              types(types), indexes(indexes) {}

	GetElementPtrInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
							 const std::shared_ptr<IRVar> &ptr,
							 const std::vector<std::shared_ptr<IRType>> &types,
							 const std::vector<std::shared_ptr<IRLiteral>> &literals)
			: MemoryInstruction(OpType::GetElementPtr), result(result), type(type), ptr(ptr),
			  types(types), literals(literals) {}


	void print() override {
    	std::cout << '\t';
        result->print();
    	std::cout << " = getelementptr ";
    	if (type) {
    		type->print();
    	}
    	std::cout << ", ptr ";
    	ptr->print();
    	for (size_t i = 0; i < types.size(); i++) {
    		std::cout << ", ";
    		if (!indexes.empty()) {
    			types[i]->print();
    			std::cout << " ";
    			indexes[i]->print();
    		} else {
    			literals[i]->print();
    		}
    	}
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

enum class ConditionType {
    eq, ne, ugt, uge, ult, ule, sgt, sge, slt, sle
};

class ICmpInstruction : public IRInstruction {
public:
    ConditionType condition_type{};
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRVar> op1;
    std::shared_ptr<IRVar> op2;
    std::shared_ptr<IRType> type;

    ICmpInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2,
                   ConditionType condition_type)
            : result(result), type(type), op1(op1), op2(op2), condition_type(condition_type) {
        op_type = OpType::ICmp;
    }

	void print() override {
    	std::cout << "\t";
    	result->print();
	    std::cout << " = icmp ";
    	std::string condition;
    	if (condition_type == ConditionType::eq) {
    		condition = "eq";
    	} else if (condition_type == ConditionType::ne) {
    		condition = "ne";
    	} else if (condition_type == ConditionType::sge) {
    		condition = "sge";
    	} else if (condition_type == ConditionType::sgt) {
    		condition = "sgt";
    	} else if (condition_type == ConditionType::sle) {
    		condition = "sle";
    	} else if (condition_type == ConditionType::slt) {
    		condition = "slt";
    	}
    	std::cout << condition << " ";
    	if (type) {
    		type->print();
    	}
    	std::cout << " ";
    	op1->print();
    	std::cout << ", ";
    	if (op2) {
    		op2->print();
    	}

    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class CallExpression : public IRInstruction {
public:
    CallExpression() = default;
};

class CallWithRetInstruction : public CallExpression {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> return_type;
    std::string func_name;
    std::vector<std::shared_ptr<IRVar>> args;
    std::vector<std::shared_ptr<IRType>> arg_types;

    CallWithRetInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &return_type,
        std::string func_name, const std::vector<std::shared_ptr<IRVar>> &args,
        const std::vector<std::shared_ptr<IRType>> &arg_types)
            : result(result), return_type(return_type), func_name(std::move(func_name)),
              args(args), arg_types(arg_types) {
        op_type = OpType::CallWithRet;
    }

	void print() override {
    	std::cout << "\t";
	    result->print();
    	std::cout << " = call ";
    	if (return_type) {
    		return_type->print();
    	}
    	std::cout << " @" << func_name << "(";
    	for (uint32_t i = 0; i < args.size(); i++) {
    		if (arg_types[i]) {
    			arg_types[i]->print();
    		}
    		std::cout << " ";
    		if (args[i]) {
    			args[i]->print();
    		}
    		if (i != args.size() - 1) {
    			std::cout << ", ";
    		}
    	}
    	std::cout << ")";
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class CallWithoutRetInstruction : public CallExpression {
public:
    std::string func_name;
    std::vector<std::shared_ptr<IRVar>> args;
    std::vector<std::shared_ptr<IRType>> arg_types;

    CallWithoutRetInstruction(std::string func_name, const std::vector<std::shared_ptr<IRVar>> &args,
        const std::vector<std::shared_ptr<IRType>> &arg_types)
            : func_name(std::move(func_name)), args(args), arg_types(arg_types) {
        op_type = OpType::CallWithoutRet;
    }

	void print() override {
    	std::cout << "\t";
    	std::cout << "call void @" << func_name << "(";
    	for (uint32_t i = 0; i < args.size(); i++) {
    		arg_types[i]->print();
    		std::cout << " ";
    		args[i]->print();
    		if (i != args.size() - 1) {
    			std::cout << ", ";
    		}
    	}
    	std::cout << ")";
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class PhiInstruction : public IRInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;
    std::vector<std::shared_ptr<IRVar>> values;
    std::vector<std::string> labels;

    PhiInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::vector<std::shared_ptr<IRVar>> &values, const std::vector<std::string> &labels)
            : result(result), type(type), values(values), labels(labels) {
        op_type = OpType::Phi;
    }

	void print() override {
	    std::cout << "\t";
    	result->print();
    	std::cout << " = phi ";
    	type->print();
    	for (uint32_t i = 0; i < values.size(); i++) {
    		std::cout << "[";
    		values[i]->print();
    		std::cout << ", %" << labels[i] << "]";
    		if (i != values.size() - 1) {
    			std::cout << ", ";
    		}
    	}
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class SelectInstruction : public IRInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> condition;
    std::shared_ptr<IRType> var_type;
    std::shared_ptr<IRVar> true_value;
    std::shared_ptr<IRVar> false_value;

    SelectInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                      const std::shared_ptr<IRVar> &condition, const std::shared_ptr<IRType> &var_type,
                      const std::shared_ptr<IRVar> &true_value, const std::shared_ptr<IRVar> &false_value)
            : result(result), type(type), condition(condition), var_type(var_type),
              true_value(true_value), false_value(false_value) {
        op_type = OpType::Select;
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class StructDefInstruction : public IRInstruction {
public:
    std::shared_ptr<IRStructType> struct_type;
    std::vector<std::shared_ptr<IRType>> args;

    StructDefInstruction(const std::shared_ptr<IRStructType>& struct_type, const std::vector<std::shared_ptr<IRType>> &args) :
        struct_type(struct_type), args(args) {}

	void print() override {
	    struct_type->print();
    	std::cout << " = type {";
    	for (uint32_t i = 0; i < args.size(); i++) {
    		args[i]->print();
    		if (i != args.size() - 1) {
    			std::cout << ", ";
    		}
    	}
    	std::cout << "}";
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class GlobalVarDefInstruction : public IRInstruction {
public:
    std::shared_ptr<GlobalVar> global_var;
    std::shared_ptr<IRLiteral> value;

    GlobalVarDefInstruction(const std::shared_ptr<GlobalVar>& global_var, const std::shared_ptr<IRLiteral>& value) :
        global_var(global_var), value(value) {}
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};

class ConstVarDefInstruction : public IRInstruction {
public:
    std::shared_ptr<ConstVar> const_var;
    std::shared_ptr<IRLiteral> value;

    ConstVarDefInstruction(const std::shared_ptr<ConstVar>& const_var, const std::shared_ptr<IRLiteral>& value) :
        const_var(const_var), value(value) {}

	void print() override {
	    const_var->print();
    	std::cout << " = constant ";
    	value->print();
    }
	void accept(IRVisitor *visitor) override { visitor->visit(this); }
};
#endif //IRINSTRUCTION_H