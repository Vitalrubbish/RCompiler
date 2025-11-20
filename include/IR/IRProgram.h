#ifndef RCOMPILER_IRPROGRAM_H
#define RCOMPILER_IRPROGRAM_H
#include <vector>
#include <memory>

#include "IRFunction.h"
#include "IRInstruction.h"
#include "IR/IRNode.h"

inline void PrintBuiltIn();

inline void GetInt();

inline void PrintInt();

inline void PrintlnInt();

inline void Exit();

class IRProgram : public IRNode {
public:
    std::vector<std::shared_ptr<StructDefInstruction>> structs;
    std::vector<std::shared_ptr<ConstVarDefInstruction>> constants;
    std::vector<std::shared_ptr<GlobalVarDefInstruction>> globals;
    std::vector<std::shared_ptr<IRFunction>> functions;

    IRProgram() = default;

	void print() override {
		PrintBuiltIn();

	    for (auto& it: constants) {
		    it->print();
	    	std::cout << '\n';
	    }

		std::cout << '\n';

    	for (auto& it: structs) {
    		it->print();
    		std::cout << '\n';
    	}

		std::cout << '\n';

    	for (auto& it: functions) {
    		it->print();
    		std::cout << '\n';
    	}
    }
};


inline void PrintBuiltIn() {
	std::cout << "declare i32 @printf(i8*, ...)\n"; // scanf function linked to libc
	std::cout << "declare i32 @scanf(i8*, ...)\n"; // printf function linked to libc
	std::cout << R"(@.str.d = private unnamed_addr constant [3 x i8] c"%d\00")" << '\n';
	std::cout << R"(@.str.d_ln = private unnamed_addr constant [4 x i8] c"%d\0A\00")" << "\n";

	PrintInt();
	PrintlnInt();
	GetInt();
	Exit();
}

inline void PrintInt() {
	std::cout << "define void @printInt(i32 %value) {\n";
	std::cout << "entry:\n";
	std::cout << "\t%fmt = getelementptr inbounds [4 x i8], [4 x i8]* @.str.d, i32 0, i32 0\n";
	std::cout << "\tcall i32 (i8*, ...) @printf(i8* %fmt, i32 %value)\n";
	std::cout << "\tret void\n";
	std::cout << "}\n\n";
}

inline void GetInt() {
	std::cout << "define i32 @getInt() {\n";
	std::cout << "entry:\n";
	std::cout << "\t%fmt = getelementptr inbounds [3 x i8], [3 x i8]* @.str.d, i32 0, i32 0\n";
	std::cout << "\t%ptr = alloca i32\n";
	std::cout << "\tcall i32 (i8*, ...) @scanf(i8* %fmt, i32* %ptr)\n";
	std::cout << "\t%input_value = load i32, ptr %ptr\n";
	std::cout << "\tret i32 %input_value\n";
	std::cout << "}\n\n";
}

inline void PrintlnInt() {
	std::cout << "define void @printlnInt(i32 %value) {\n";
	std::cout << "entry:\n";
	std::cout << "\t%fmt = getelementptr inbounds [4 x i8], [4 x i8]* @.str.d_ln, i32 0, i32 0\n";
	std::cout << "\tcall i32 (i8*, ...) @printf(i8* %fmt, i32 %value)\n";
	std::cout << "\tret void\n";
	std::cout << "}\n\n";
}

inline void Exit() {
	std::cout << "define void @exit(i32 %code) {\n";
	std::cout << "entry:\n";
	std::cout << "\tret void\n";
	std::cout << "}\n\n";
}

#endif //RCOMPILER_IRPROGRAM_H