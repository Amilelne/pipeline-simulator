#pragma once
#include "regfile.h"
#include "Instruction.h"
class WB {
public:
	bool RegWrite;
	bool MemtoReg;
	WB() {
		RegWrite = false;
		MemtoReg = false;
	}
};
class M {
public:
	bool MemRead;
	bool MemWrite;
	M() {
		MemRead = false;
		MemWrite = false;
	}
};
class EX {
public:
	int ALUOp;
	bool ALUSrc;
	bool RegDst;
	EX() {
		ALUOp = 0x00;
	}
};
class Control {
public:
	bool RegDst;
	bool Jump;
	bool Branch;
	bool MemRead;
	bool MemtoReg;
	int ALUOp;
	bool MemWrite;
	bool ALUSrc;
	bool RegWrite;
	Control() {
		RegDst = Jump = Branch = MemRead = MemtoReg = ALUOp = MemWrite = ALUSrc = RegWrite = false;
	}
	void show() {
		printf("control:%d,%d,%d,%d,%d,%d,%d,%d,%d\n", RegDst, Jump, Branch, MemRead, MemtoReg, ALUOp, MemWrite, ALUSrc, RegWrite);
	}
	void decode_instr(int opcode);
	//void trans_to_IDEX(bufferIDEX &buf);
};
class bufferDMWB {
public:
	int ALU_result;
	int data;
	int rt;
	int opcode;
	WB wb;
public:
	bufferDMWB();
};
class bufferEXDM {
public:
	int ALU_result;
	int rtdata;
	int opcode;
	WB wb;
	M m;
public:
	bufferEXDM();
	bufferEXDM(int A, int a, int r);
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
	EX ex;
	M m;
	WB wb;
public:
	bufferIDEX() {
		rs_num = rt_num = rd_num = opcode = shamt = func = 0;
	}
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
};
class IFstage {
public:
	void instr_fetch(bufferIFID &buf, int instr[], regfile &reg);
};
class IDstage {
public:
	void instr_decode(bufferIFID bufIFID, bufferIDEX &bufIDEX, Control &control, regfile &reg, Instruction &instruction);
};
class EXstage {
public:
	void calculate(bufferIDEX bufIDEX,bufferEXDM &bufEXDM,regfile &reg);
};
class DMstage {
public:
	void deal_memory(bufferEXDM bufEXDM,bufferDMWB &bufDMWB,int data[],regfile &reg);
	int read_memory(bufferEXDM bufEXDM,int data[]);
	int write_memory(bufferEXDM bufEXDM,int data[]);
};
class WBstage {
public:
	void writeback(bufferDMWB bufDMWB,regfile &reg);
};
