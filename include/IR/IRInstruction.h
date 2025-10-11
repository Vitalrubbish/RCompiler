#ifndef IRINSTRUCTION_H
#define IRINSTRUCTION_H

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <utility>

#include "IRNode.h"

class IRVar;
class IRType;
class IRVisitor;

/**************** ENUM TYPES ****************/
enum class OpType {
    Ret, CondBr, UncondBr,
    Add, Sub, Mul, SDiv, Srem, Shl, AShr, And, Or, Xor,
    Alloca, Load, Store, GetElementPtr,
    ICmp, CallWithRet, CallWithoutRet, Phi, Select
};

/**************** BASE CLASS ****************/
class IRInstruction : public IRNode {
protected:
    OpType op_type{};
public:
    IRInstruction() = default;
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
    AddInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
        const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Add) {}
};

class SubInstruction : public BinaryOpInstruction {
public:
    SubInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Sub) {}
};

class MulInstruction : public BinaryOpInstruction {
public:
    MulInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Mul) {}
};

class SDivInstruction : public BinaryOpInstruction {
public:
    SDivInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::SDiv) {}
};

class SremInstruction : public BinaryOpInstruction {
public:
    SremInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Srem) {}
};

class ShlInstruction : public BinaryOpInstruction {
public:
    ShlInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Shl) {}
};

class AShrInstruction : public BinaryOpInstruction {
public:
    AShrInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::AShr) {}
};

class AndInstruction : public BinaryOpInstruction {
public:
    AndInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::And) {}
};

class OrInstruction : public BinaryOpInstruction {
public:
    OrInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Or) {}
};

class XorInstruction : public BinaryOpInstruction {
public:
    XorInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                   const std::shared_ptr<IRVar> &op1, const std::shared_ptr<IRVar> &op2)
            : BinaryOpInstruction(result, type, op1, op2, OpType::Xor) {}
};

class RetInstruction : public ControlInstruction {
public:
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> var;

    RetInstruction(const std::shared_ptr<IRType> &type, const std::shared_ptr<IRVar> &var)
            : ControlInstruction(OpType::Ret), type(type), var(var) {}
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
};

class UnconditionalBrInstruction : public ControlInstruction {
public:
    std::string label;

    explicit UnconditionalBrInstruction(std::string label)
            : ControlInstruction(OpType::UncondBr), label(std::move(label)) {}
};

class AllocaInstruction : public MemoryInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;

    AllocaInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type)
            : MemoryInstruction(OpType::Alloca), result(result), type(type) {}
};

class LoadInstruction : public MemoryInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> ptr;

    LoadInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                    const std::shared_ptr<IRVar> &ptr)
            : MemoryInstruction(OpType::Load), result(result), type(type), ptr(ptr) {}
};

class StoreInstruction : public MemoryInstruction {
public:
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> var;
    std::shared_ptr<IRVar> ptr;

    StoreInstruction(const std::shared_ptr<IRType> &type, const std::shared_ptr<IRVar> &var,
                     const std::shared_ptr<IRVar> &ptr)
            : MemoryInstruction(OpType::Store), type(type), var(var), ptr(ptr) {}
};

class GetElementPtrInstruction : public MemoryInstruction {
public:
    std::shared_ptr<IRVar> result;
    std::shared_ptr<IRType> type;
    std::shared_ptr<IRVar> ptr;
    std::vector<std::shared_ptr<IRType>> types;
    std::vector<std::shared_ptr<IRVar>> indexes;

    GetElementPtrInstruction(const std::shared_ptr<IRVar> &result, const std::shared_ptr<IRType> &type,
                             const std::shared_ptr<IRVar> &ptr,
                             const std::vector<std::shared_ptr<IRType>> &types,
                             const std::vector<std::shared_ptr<IRVar>> &indexes)
            : MemoryInstruction(OpType::GetElementPtr), result(result), type(type), ptr(ptr),
              types(types), indexes(indexes) {}
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
};
#endif //IRINSTRUCTION_H
