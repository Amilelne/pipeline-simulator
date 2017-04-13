#pragma once
#include "regfile.h"
#include "Instruction.h"
class WB {
public:
	bool RegWrite;
	bool MemtoReg;
	int rt_num;
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
		printf("control:Regdst=%d,Jump=%d,Branch=%d,Memread=%d,MemtoReg%d,ALUOp=%d,MemWrite=%d,ALUSrc=%d,RegWrite=%d\n", RegDst, Jump, Branch, MemRead, MemtoReg, ALUOp, MemWrite, ALUSrc, RegWrite);
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
class ALUcontrol {
public:
	int ALUOp;
	int funct;
	int ALU_control;
	ALUcontrol() {
		ALUOp = 0x11;
	}
	void get_ALU_control() {
		if (ALUOp == 0x00)
			ALU_control = 0x0010;
		else if (ALUOp == 0x01)
			ALU_control = 0x0110;
		else if (ALUOp == 0x10) {
			switch (funct) {
			case 0x100000:
				ALU_control = 0x0010;
				break;
			case 0x100010:
				ALU_control = 0x0110;
				break;
			case 0x100100:
				ALU_control = 0x0000;
				break;
			case 0x100101:
				ALU_control = 0x0001;
				break;
			case 0x101010:
				ALU_control = 0x0111;
				break;
			}
		}
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
	void calculate(bufferIDEX bufIDEX,bufferEXDM &bufEXDM,regfile &reg, ALUcontrol &ALU_c);
};
class DMstage {
public:
	void deal_memory(bufferEXDM bufEXDM,bufferDMWB &bufDMWB,int data[],regfile &reg);
	int read_memory(bufferEXDM bufEXDM,bufferDMWB &bufDMWB, int data[]);
	int write_memory(bufferEXDM bufEXDM,int data[]);
};
class WBstage {
public:
	void writeback(bufferDMWB bufDMWB,regfile &reg);
};

