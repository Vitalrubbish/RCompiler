#ifndef ASMINSTRUCTION_H
#define ASMINSTRUCTION_H

#include <iostream>
#include <memory>
#include "ASMOperand.h"

enum class ASMOpcode {
	ADD, SUB,

	MUL, DIV, REM,

	AND, OR, XOR,

	SRL, SRA, SLL, SLT, SLTU,

	ADDI, XORI, ANDI, ORI, SLLI, SRLI, SRAI, SLTI, SLTIU,

	LW, SW,

	JAL, JALR,

	LUI, AUIPC,

	BEQ, BNE, BLT, BGE, BLTU, BGEU,
};

class ASMInstruction {
public:
	ASMOpcode opcode;

	explicit ASMInstruction(ASMOpcode opcode) : opcode(opcode) {}

	virtual void print() = 0;

	virtual ~ASMInstruction() = default;
};

class ASMAddInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMAddInstruction(const std::shared_ptr<ASMOperand>& rd,
	               const std::shared_ptr<ASMOperand>& rs1,
	               const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::ADD), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tadd ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMSubInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMSubInstruction(const std::shared_ptr<ASMOperand>& rd,
				   const std::shared_ptr<ASMOperand>& rs1,
				   const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::SUB), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tsub ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMMulInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMMulInstruction(const std::shared_ptr<ASMOperand>& rd,
				   const std::shared_ptr<ASMOperand>& rs1,
				   const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::MUL), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tmul ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMDivInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMDivInstruction(const std::shared_ptr<ASMOperand>& rd,
				   const std::shared_ptr<ASMOperand>& rs1,
				   const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::DIV), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tdiv ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMRemInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMRemInstruction(const std::shared_ptr<ASMOperand>& rd,
				   const std::shared_ptr<ASMOperand>& rs1,
				   const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::REM), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\trem ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMAndInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMAndInstruction(const std::shared_ptr<ASMOperand>& rd,
	               const std::shared_ptr<ASMOperand>& rs1,
	               const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::AND), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tand ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMOrInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMOrInstruction(const std::shared_ptr<ASMOperand>& rd,
	              const std::shared_ptr<ASMOperand>& rs1,
	              const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::OR), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tor ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMXorInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMXorInstruction(const std::shared_ptr<ASMOperand>& rd,
	               const std::shared_ptr<ASMOperand>& rs1,
	               const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::XOR), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\txor ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMSrlInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMSrlInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::SRL), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tsrl ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMSraInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMSraInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::SRA), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tsra ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMSllInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMSllInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::SLL), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tsll ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMSltInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMSltInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::SLT), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tslt ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMSltuInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMSltuInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::SLTU), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tsltu ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
	}
};

class ASMAddiInstruction final: public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMAddiInstruction(const std::shared_ptr<ASMOperand>& rd,
				   const std::shared_ptr<ASMOperand>& rs1,
				   const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::ADDI), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\taddi ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMAndiInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMAndiInstruction(const std::shared_ptr<ASMOperand>& rd,
	               const std::shared_ptr<ASMOperand>& rs1,
	               const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::ANDI), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tandi ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMOriInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMOriInstruction(const std::shared_ptr<ASMOperand>& rd,
	              const std::shared_ptr<ASMOperand>& rs1,
	              const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::ORI), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tori ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMXoriInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMXoriInstruction(const std::shared_ptr<ASMOperand>& rd,
	               const std::shared_ptr<ASMOperand>& rs1,
	               const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::XORI), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\txori ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
	}
};

class ASMSlliInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMSlliInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::SLLI), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tslli ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMSrliInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMSrliInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::SRLI), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tsrli ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMSraiInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMSraiInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::SRAI), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tsrai ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMSltiInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMSltiInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::SLTI), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tslti ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMSltiuInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMSltiuInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::SLTIU), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tsltiu ";
		rd->print();
		std::cout << ", ";
		rs1->print();
		std::cout << ", ";
		imm->print();
	}
};

#endif //ASMINSTRUCTION_H
