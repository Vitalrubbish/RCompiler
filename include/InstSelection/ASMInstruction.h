#ifndef ASMINSTRUCTION_H
#define ASMINSTRUCTION_H

#include <iostream>
#include <memory>
#include "ASMOperand.h"

enum class ASMOpcode {
	ADD, SUB,

	MUL, DIV, DIVU, REM, REMU,

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

class ASMDivuInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMDivuInstruction(const std::shared_ptr<ASMOperand>& rd,
				   const std::shared_ptr<ASMOperand>& rs1,
				   const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::DIVU), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tdivu ";
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

class ASMRemuInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;

	ASMRemuInstruction(const std::shared_ptr<ASMOperand>& rd,
					const std::shared_ptr<ASMOperand>& rs1,
					const std::shared_ptr<ASMOperand>& rs2)
		: ASMInstruction(ASMOpcode::REMU), rd(rd), rs1(rs1), rs2(rs2) {}

	void print() override {
		std::cout << "\tremu ";
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

class ASMLwInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMLwInstruction(const std::shared_ptr<ASMOperand>& rd,
	              const std::shared_ptr<ASMOperand>& rs1,
	              const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::LW), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tlw ";
		rd->print();
		std::cout << ", ";
		imm->print();
		std::cout << "(";
		rs1->print();
		std::cout << ")";
	}
};

class ASMSwInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rs2;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMSwInstruction(const std::shared_ptr<ASMOperand>& rs2,
	              const std::shared_ptr<ASMOperand>& rs1,
	              const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::SW), rs2(rs2), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tsw ";
		rs2->print();
		std::cout << ", ";
		imm->print();
		std::cout << "(";
		rs1->print();
		std::cout << ")";
	}
};

class ASMJalInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> label;

	ASMJalInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& label)
		: ASMInstruction(ASMOpcode::JAL), rd(rd), label(label) {}

	void print() override {
		std::cout << "\tjal ";
		rd->print();
		std::cout << ", ";
		label->print();
	}
};

class ASMJalrInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> imm;

	ASMJalrInstruction(const std::shared_ptr<ASMOperand>& rd,
	                 const std::shared_ptr<ASMOperand>& rs1,
	                 const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::JALR), rd(rd), rs1(rs1), imm(imm) {}

	void print() override {
		std::cout << "\tjalr ";
		rd->print();
		std::cout << ", ";
		imm->print();
		std::cout << "(";
		rs1->print();
		std::cout << ")";
	}
};

class ASMRetInstruction final : public ASMInstruction {
public:
	ASMRetInstruction() : ASMInstruction(ASMOpcode::JALR) {}

	void print() override {
		std::cout << "\tret";
	}
};

class ASMLuiInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> imm;

	ASMLuiInstruction(const std::shared_ptr<ASMOperand>& rd,
	              const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::LUI), rd(rd), imm(imm) {}

	void print() override {
		std::cout << "\tlui ";
		rd->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMAuipcInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> imm;

	ASMAuipcInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& imm)
		: ASMInstruction(ASMOpcode::AUIPC), rd(rd), imm(imm) {}

	void print() override {
		std::cout << "\tauipc ";
		rd->print();
		std::cout << ", ";
		imm->print();
	}
};

class ASMBeqInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;
	std::shared_ptr<ASMOperand> label;

	ASMBeqInstruction(const std::shared_ptr<ASMOperand>& rs1,
	                const std::shared_ptr<ASMOperand>& rs2,
	                const std::shared_ptr<ASMOperand>& label)
		: ASMInstruction(ASMOpcode::BEQ), rs1(rs1), rs2(rs2), label(label) {}

	void print() override {
		std::cout << "\tbeq ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
		std::cout << ", ";
		label->print();
	}
};

class ASMBneInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;
	std::shared_ptr<ASMOperand> label;

	ASMBneInstruction(const std::shared_ptr<ASMOperand>& rs1,
					const std::shared_ptr<ASMOperand>& rs2,
					const std::shared_ptr<ASMOperand>& label)
		: ASMInstruction(ASMOpcode::BNE), rs1(rs1), rs2(rs2), label(label) {}

	void print() override {
		std::cout << "\tbne ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
		std::cout << ", ";
		label->print();
	}
};

class ASMBltInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;
	std::shared_ptr<ASMOperand> label;

	ASMBltInstruction(const std::shared_ptr<ASMOperand>& rs1,
					const std::shared_ptr<ASMOperand>& rs2,
					const std::shared_ptr<ASMOperand>& label)
		: ASMInstruction(ASMOpcode::BLT), rs1(rs1), rs2(rs2), label(label) {}

	void print() override {
		std::cout << "\tblt ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
		std::cout << ", ";
		label->print();
	}
};

class ASMBgeInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;
	std::shared_ptr<ASMOperand> label;

	ASMBgeInstruction(const std::shared_ptr<ASMOperand>& rs1,
					const std::shared_ptr<ASMOperand>& rs2,
					const std::shared_ptr<ASMOperand>& label)
		: ASMInstruction(ASMOpcode::BGE), rs1(rs1), rs2(rs2), label(label) {}

	void print() override {
		std::cout << "\tbge ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
		std::cout << ", ";
		label->print();
	}
};

class ASMBltuInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;
	std::shared_ptr<ASMOperand> label;

	ASMBltuInstruction(const std::shared_ptr<ASMOperand>& rs1,
					const std::shared_ptr<ASMOperand>& rs2,
					const std::shared_ptr<ASMOperand>& label)
		: ASMInstruction(ASMOpcode::BLTU), rs1(rs1), rs2(rs2), label(label) {}

	void print() override {
		std::cout << "\tbltu ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
		std::cout << ", ";
		label->print();
	}
};

class ASMBgeuInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rs1;
	std::shared_ptr<ASMOperand> rs2;
	std::shared_ptr<ASMOperand> label;

	ASMBgeuInstruction(const std::shared_ptr<ASMOperand>& rs1,
					const std::shared_ptr<ASMOperand>& rs2,
					const std::shared_ptr<ASMOperand>& label)
		: ASMInstruction(ASMOpcode::BGEU), rs1(rs1), rs2(rs2), label(label) {}

	void print() override {
		std::cout << "\tbgeu ";
		rs1->print();
		std::cout << ", ";
		rs2->print();
		std::cout << ", ";
		label->print();
	}
};

class ASMSeqzInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs;

	ASMSeqzInstruction(const std::shared_ptr<ASMOperand>& rd,
	                const std::shared_ptr<ASMOperand>& rs)
		: ASMInstruction(ASMOpcode::SLTIU), rd(rd), rs(rs) {}

	void print() override {
		std::cout << "\tseqz ";
		rd->print();
		std::cout << ", ";
		rs->print();
	}
	// set rd = 1 iff rs == 0
};

class ASMSnezInstruction final : public ASMInstruction {
public:
	std::shared_ptr<ASMOperand> rd;
	std::shared_ptr<ASMOperand> rs;

	ASMSnezInstruction(const std::shared_ptr<ASMOperand>& rd,
					 const std::shared_ptr<ASMOperand>& rs)
		: ASMInstruction(ASMOpcode::SLTU), rd(rd), rs(rs) {}

	void print() override {
		std::cout << "\tsnez ";
		rd->print();
		std::cout << ", ";
		rs->print();
	}
	// set rd = 1 iff rs != 0
};

#endif //ASMINSTRUCTION_H
