#pragma once
#include "regfile.h"
#include "Instruction.h"
#include<string>
class WB {
public:
	bool RegWrite;
	bool MemtoReg;
	int rt_num;
	WB() {
		rt_num = 0;
		RegWrite = false;
		MemtoReg = false;
	}
	void show() {
		printf("------WB------\n");
		printf("RegWrite = %d,MemtoReg=%d,rt_num=%d\n", RegWrite, MemtoReg, rt_num);
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
	void show() {
		printf("-----M-----\n");
		printf("MemRead=%d,MemWrite=%d\n", MemRead, MemWrite);
	}
};

class EX {
public:
	int ALUOp;
	bool ALUSrc;
	bool RegDst;
	EX() {
		ALUOp = 0x11;
	}
	void show() {
		printf("----EX-----\n");
		printf("ALUOp=%06x,ALUSrc=%d,RegDst=%d\n", ALUOp, ALUSrc, RegDst);
	}
};
class NOP {
public:
	EX ex;
	M m;
	WB wb;
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
		RegDst = Jump = Branch = MemRead = MemtoReg  = MemWrite = ALUSrc = RegWrite = false;
		ALUOp = 0x11;
	}
	void show() {
		printf("------control-----\n");
		printf("control:Regdst=%d,Jump=%d,Branch=%d,Memread=%d,MemtoReg=%d,ALUOp=%d,MemWrite=%d,ALUSrc=%d,RegWrite=%d\n", RegDst, Jump, Branch, MemRead, MemtoReg, ALUOp, MemWrite, ALUSrc, RegWrite);
	}
	void decode_instr(int opcode,int funct);
	void decode_nop() {
		RegDst = Jump = Branch = MemRead = MemtoReg = MemWrite = ALUSrc = RegWrite = false;
		ALUOp = 0x11;
	}
	//void trans_to_IDEX(bufferIDEX &buf);
};
class bufferDMWB {
public:
	int ALU_result;
	int data;
	int rt;
	int opcode;
	WB wb;
	int write_data;
public:
	bufferDMWB();
	void show() {
		printf("------------------DMWB------------------------------\n");
		printf("ALU_result=%d,data=%x,write_data=%d,rt=%d,opcode=%x\n",ALU_result,data,write_data,rt,opcode);
		wb.show();
	}
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
	void show() {
		printf("------------------------EXDM-----------------------------\n");
		printf("ALU_result=%d,rtdata=%d,opcode=%06x\n", ALU_result, rtdata, opcode);
		wb.show();
		m.show();
	}
};
class bufferIDEX {
public:
	int rsdata;
	int rtdata;
	int rs_num;
	int rt_num;
	int rd_num;
	int opcode;
	int shamt;
	int func;
	int immediate;
	int jal_PC;
	int forward_from_DMWB;
	int address;
	EX ex;
	M m;
	WB wb;
public:
	bufferIDEX() {
		rsdata = rtdata = 0;
		rs_num = rt_num = rd_num = opcode = shamt = func = forward_from_DMWB = 0;
		jal_PC = 0;
		address = 0;
	}
	void show() {
		printf("---------------------------IDEX-----------------\n");
		printf("rsdata=%08x,rtdata=%08x,jal_pc=%08x\n", rsdata, rtdata,jal_PC);
		printf("rs_num=%d,rt_num=%d,rd_num=%d,opcode=%x,func=%x,shamt=%x,immediate=%x,address=%08x\n", rs_num, rt_num, rd_num, opcode, func, shamt, immediate,address);
		ex.show();
		m.show();
		wb.show();
	}
};
class bufferIFID {
public:
	int PC;
	int instr;
	bool hazard;
	bool flash;
	bool jump;
	int rt_num;
	int rs_num;
	int rd_num;
	int opcode;
	int func;
	int immediate;
	int shamt;
	int EX_data_forward;
public:
	bufferIFID() {
		hazard = flash = jump = false;
		EX_data_forward = 0;
		rs_num = rt_num = rd_num = 0;
		opcode = immediate = shamt = EX_data_forward = 0;
	}
	void show() {
		printf("---------IFID-------------\n");
		printf("PC=%d,instr=%08x,flash=%d,jump=%d,rs_num=%d,rt_num=%d\n", PC, instr, flash, jump,rs_num,rt_num);
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
			case 0x20:
				ALU_control = 0x0010;
				break;
			case 0x21:
				ALU_control = 0x0010;
				break;
			case 0x22:
				ALU_control = 0x0110;
				break;
			case 0x24:
				ALU_control = 0x0000;
				break;
			case 0x25:
				ALU_control = 0x0001;
				break;
			case 0x2A:
				ALU_control = 0x0111;
				break;
			case 0x10:
				ALU_control = 0x1111;
				break;
			case 0x12:
				ALU_control = 0x1111;
				break;
			default:
				ALU_control = 0x1111;
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
	void instr_decode(bufferIFID bufIFID, bufferIDEX &bufIDEX, bufferEXDM bufEXDM, Control &control, regfile &reg, Instruction &instruction);
};
class EXstage {
public:
	void calculate(bufferIFID &bufIFID, bufferIDEX &bufIDEX,bufferEXDM &bufEXDM, bufferDMWB bufDMWB,regfile &reg, ALUcontrol &ALU_c, FILE* &snapshot,NOP nop);
	
};
class DMstage {
public:
	void deal_memory(bufferIDEX &bufIDEX, bufferEXDM bufEXDM,bufferDMWB &bufDMWB,int data[],regfile &reg);
	int read_memory(bufferEXDM bufEXDM,bufferDMWB &bufDMWB, int data[], regfile &reg);
	int write_memory(bufferEXDM bufEXDM,int data[], regfile &reg);
};
class WBstage {
public:
	void writeback(bufferDMWB bufDMWB,regfile &reg);
};
class Hazard {
public:
	int ForwardA;
	int ForwardB;
	Hazard() {
		ForwardA = ForwardB = 0;
	}
	bool Branch_Hazard(bufferIFID bufIFID,bufferIDEX bufIDEX, bufferEXDM bufEXDM) {
		if (bufIFID.opcode == 0x04 || bufIFID.opcode == 0x05 || bufIFID.opcode == 0x07) {
			if (bufIDEX.wb.RegWrite && bufIDEX.wb.rt_num!=0 && ((bufIDEX.wb.rt_num == bufIFID.rs_num) || (bufIDEX.wb.rt_num == bufIFID.rt_num)))
				return true;
			else if (bufEXDM.wb.MemtoReg && bufEXDM.wb.rt_num != 0 && ((bufEXDM.wb.rt_num == bufIFID.rs_num) || (bufEXDM.wb.rt_num == bufIFID.rt_num)))
				return true;
			else
				return false;
		}
		if (bufIDEX.m.MemRead &&  bufIDEX.wb.rt_num != 0 && ((bufIDEX.wb.rt_num == bufIFID.rs_num) || (bufIDEX.wb.rt_num == bufIFID.rt_num)))
			return true;
		return false;
	}
	bool Load_Use_Hazard(bufferIDEX bufIDEX, bufferEXDM bufEXDM) {
		if (bufEXDM.m.MemRead && (bufEXDM.wb.rt_num != 0)&& ((bufEXDM.wb.rt_num == bufIDEX.rs_num) || (bufEXDM.wb.rt_num == bufIDEX.rt_num)))
			return true;
	}
	int EX_hazard(bufferEXDM bufEXDM, bufferIDEX bufIDEX,regfile &reg) {
		if (bufEXDM.wb.RegWrite && (bufEXDM.wb.rt_num != 0) && (bufEXDM.wb.rt_num == bufIDEX.rs_num)) {
			ForwardA = 1;
		}
		else
			ForwardA = 0;
		if (bufEXDM.wb.RegWrite && (bufEXDM.wb.rt_num != 0) && (bufEXDM.wb.rt_num == bufIDEX.rt_num)) {
			ForwardB = 2;
		}
		else
			ForwardB = 0;
		return (ForwardA+ForwardB);
	}
	int MEM_hazard(bufferDMWB bufDMWB, bufferIDEX bufIDEX, bufferEXDM bufEXDM,regfile &reg) {
		if ((bufIDEX.opcode!=0x04&& bufIDEX.opcode != 0x05&& bufIDEX.opcode != 0x06)&&bufDMWB.wb.RegWrite && (bufDMWB.wb.rt_num != 0) && !(bufEXDM.wb.RegWrite && (bufEXDM.wb.rt_num != 0) && (bufEXDM.wb.rt_num == bufIDEX.rs_num)) && (bufDMWB.wb.rt_num == bufIDEX.rs_num))
			ForwardA = 4;
		else
			ForwardA = 0;
		if ((bufIDEX.opcode != 0x04 && bufIDEX.opcode != 0x05 && bufIDEX.opcode != 0x06)&&bufDMWB.wb.RegWrite && (bufDMWB.wb.rt_num != 0) && !(bufEXDM.wb.RegWrite && (bufEXDM.wb.rt_num != 0) && (bufEXDM.wb.rt_num == bufIDEX.rt_num)) && (bufDMWB.wb.rt_num == bufIDEX.rt_num))
			ForwardB = 7;
		else
			ForwardB = 0;
		return (ForwardA + ForwardB);
	}
	int EX_ID_hazard(bufferIFID bufIFID,bufferIDEX &IDEX,bufferEXDM &bufEXDM,regfile &reg) {
		if (bufIFID.opcode == 0x04 || bufIFID.opcode == 0x05 || bufIFID.opcode == 0x07) {
			printf("@@@@@@@@@@EXDMrt_num=%d\n",  bufEXDM.wb.rt_num);
			if (bufEXDM.wb.RegWrite && (bufEXDM.wb.rt_num != 0) && (bufEXDM.wb.rt_num == bufIFID.rs_num)) {
				ForwardA = 1;
				//printf("@@@@@@@@@@bufIFID.rs_num=%d,rt_num=%d\n", bufEXDM.wb.RegWrite, bufEXDM.wb.rt_num,bufIFID.rt_num);
			}
			else
				ForwardA = 0;
			if (bufEXDM.wb.RegWrite && (bufEXDM.wb.rt_num != 0) && (bufEXDM.wb.rt_num == bufIFID.rt_num)) {
				ForwardB = 2;
			}
			else
				ForwardB = 0;
		}
		else {
			ForwardA = ForwardB = 0;
		}
		return (ForwardA + ForwardB);
	}
};

