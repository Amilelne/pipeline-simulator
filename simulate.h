#pragma once
#include "regfile.h"
#include "Instruction.h"
class Control {
public:
	int opcode;
	bool RegDst;
	bool Jump;
	bool Branch;
	bool MemRead;
	bool MemtoReg;
	bool ALUOp;
	bool MemWrite;
	bool ALUSrc;
	bool RegWrite;
	Control() {
		RegDst = Jump = Branch = MemRead = MemtoReg = ALUOp = MemWrite = ALUSrc = RegWrite = false;
	}
	void show() {
		printf("%d,%d,%d,%d,%d,%d,%d,%d,%d\n", RegDst, Jump, Branch, MemRead, MemtoReg, ALUOp, MemWrite, ALUSrc, RegWrite);
	}
};
class bufferDMWB {
public:
	int data;
	int ALU_result;
	int rt;
	bool WB;
public:
	bufferDMWB();
	bufferDMWB(int d, int A, int r, bool c);
};
class bufferEXDM {
public:
	int ALU_result;
	int address;
	int rt;
	bool WB;
	bool DM;
public:
	bufferEXDM();
	bufferEXDM(int A, int a, int r, bool W, bool D);
};
class bufferIDEX {
public:
	int rsdata;
	int rtdata;
	int PC;
	int sign_extend;
	int rs_num;
	int rt_num;
	int rd_num;
	int opcode;
	int shamt;
	int func;
	int immediate;
	bool equal;
	bool EX;
	bool DM;
	bool WB;
public:
	bufferIDEX() {
		equal = EX = DM = WB = false;
		rs_num = rt_num = rd_num = opcode = shamt = func = 0;
	}
	void instr_decode(int instru, Control &control, regfile &reg, Instruction &instruction);
};
class bufferIFID {
public:
	int PC;
	int instr;
	bool hazard;
	bool flash;
	bool jump;
public:
	bufferIFID() {
		hazard = flash = jump = false;
	}
	void instr_fetch(int IniPC,int PC,int instr[], regfile &reg);
};
