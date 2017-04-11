#include "simulate.h"
#include "regfile.h"
#include "Instruction.h"
#include<iostream>
#include "arithmetic.h"
using namespace std;
ALU alu;
bufferDMWB::bufferDMWB() {
	data = 0;
	rt = 0;
}
bufferEXDM::bufferEXDM() {
	ALU_result = 0;
	rtdata = 0;
}
bufferEXDM::bufferEXDM(int A, int a, int r) {
	ALU_result = A;
	rtdata = r;
}
void IFstage::instr_fetch(bufferIFID &buf,int instr[],regfile &reg) {
	int tmp = reg.PC+4 - reg.IniPC;
	if (tmp % 4 != 0)
		exit(0);
	reg.IF = instr[tmp / 4 + 2];
	buf.PC = tmp;
}
void IDstage::instr_decode(bufferIFID bufIFID,bufferIDEX &bufIDEX,Control &control,regfile &reg,Instruction &instruction) {
	bufIDEX.opcode = (0xfc000000 & bufIFID.instr) >> 26;
	bufIDEX.func = (0x0000003f & bufIFID.instr);
	bufIDEX.rs_num = (0x03e00000 & bufIFID.instr) >> 21;
	bufIDEX.rt_num = (0x001f0000 & bufIFID.instr) >> 16;
	bufIDEX.rd_num = (0x0000f800 & bufIFID.instr) >> 11;
	bufIDEX.immediate = (0x0000ffff & bufIFID.instr);
	bufIDEX.rsdata = reg.reg[bufIDEX.rs_num];
	bufIDEX.rtdata = reg.reg[bufIDEX.rt_num];
	instruction.getname(bufIDEX.opcode, bufIDEX.func);
	instruction.show();
	control.decode_instr(bufIDEX.opcode);
	control.show();
	bufIDEX.ex.ALUOp = control.ALUOp;
	bufIDEX.ex.RegDst = control.RegDst;
	bufIDEX.ex.ALUSrc = control.ALUSrc;
	bufIDEX.m.MemRead = control.MemRead;
	bufIDEX.wb.MemtoReg = control.MemtoReg;
	bufIDEX.m.MemWrite = control.MemWrite;
	bufIDEX.wb.RegWrite = control.RegWrite;
	printf("%x,%d,%d,%d,%x,%x,%x", bufIDEX.opcode, bufIDEX.rs_num, bufIDEX.rt_num, bufIDEX.rd_num, bufIDEX.immediate, bufIDEX.rsdata, bufIDEX.rtdata);
}
void Control::decode_instr(int opcode) {
	if (opcode == 0) {
		RegDst = true;
		Branch = false;
		MemRead = false;
		MemtoReg = false;
		ALUOp = 0x10;
		MemWrite = false;
		ALUSrc = false;
		RegWrite = true;
	}
	else if (opcode == 0x08 || opcode == 0x09) {
		RegDst = 1;
		Branch = 0;
		MemRead = 0;
		MemtoReg = 0;
		//ALUOp = 0x11;
		MemWrite = 0;
		ALUSrc = 1;
		RegWrite = 1;
	}
	else if (opcode >= 0x20 && opcode <= 0x25&&opcode!=0x22) {
		RegDst = 0;
		Branch = 0;
		MemRead = 1;
		MemWrite = 0;
		ALUSrc = 1;
		RegWrite = 1;
		ALUOp = 0x00;
	}
	else if (opcode == 0x28 || opcode == 0x29 || opcode == 0x2B) {
		RegDst = 0;
		Branch = 0;
		MemRead = 0;
		MemWrite = 1;
		ALUSrc = 1;
		RegWrite = 0;
		ALUOp = 0x00;
	}
}
/*
void Control::trans_to_IDEX(bufferIDEX &buf) {
	buf.ex.ALUOp = ALUOp;
	buf.ex.RegDst = RegDst;
	buf.ex.ALUSrc = ALUSrc;
	buf.m.MemRead = MemRead;
	buf.wb.MemtoReg = MemtoReg;
	buf.m.MemWrite = MemWrite;
	buf.wb.RegWrite = RegWrite;
}*/
void EXstage::calculate(bufferIDEX bufIDEX, bufferEXDM &bufEXDM,regfile &reg) {
	int second_data = 0;
	int result;
	if (bufIDEX.ex.ALUSrc == 0)
		second_data = bufIDEX.rtdata;
	else
		second_data = bufIDEX.immediate;
	switch (bufIDEX.ex.ALUOp) {
	case 0x0010:
		result = alu.add(bufIDEX.rsdata, second_data);
		break;
	case 0x0110:
		result = alu.subtract(bufIDEX.rsdata, second_data);
		break;
	case 0x0000:
		result = alu. and (bufIDEX.rsdata, second_data);
		break;
	case 0x0001:
		result = alu. or (bufIDEX.rsdata, second_data);
		break;
	case 0x0111:
		break;
	}
	bufEXDM.ALU_result = result;
	bufEXDM.rtdata = bufIDEX.rtdata;
	bufEXDM.wb = bufIDEX.wb;
	bufEXDM.m = bufIDEX.m;
	bufEXDM.opcode = bufIDEX.opcode;
	reg.EX = "EX";
}
void DMstage::deal_memory(bufferEXDM bufEXDM, bufferDMWB &bufDMWB,int data[],regfile &reg) {
	int readdata = 0;
	if (bufEXDM.m.MemRead) {
		bufDMWB.data = read_memory(bufEXDM, data);
	}
	else if (bufEXDM.m.MemRead) {
		bufDMWB.data = write_memory(bufEXDM, data);
	}
	bufDMWB.wb = bufEXDM.wb;
	bufDMWB.ALU_result = bufEXDM.ALU_result;
	reg.DM = "DM";
}
int DMstage::read_memory(bufferEXDM bufEXDM,int data[]) {
	int opcode = bufEXDM.opcode;
	int rt_value = 0;
	if (opcode == 0x23) {
		if (bufEXDM.ALU_result > 1020) {
			printf( "In cycle : Address Overflow\n");
			//halt_flag = true;
		}
		else {
			rt_value = data[2 + bufEXDM.ALU_result / 4];
		}
	}
	else if (opcode == 0x21) {

	}
	else if (opcode == 0x25) {

	}
	else if (opcode == 0x20) {

	}
	else if (opcode == 0x24) {

	}
	return 0;
}
int DMstage::write_memory(bufferEXDM bufEXDM, int data[]) {
	int opcode = bufEXDM.opcode;
	if (opcode == 0x2B) {

	}
	else if (opcode == 0x29) {

	}
	else if (opcode == 0x28) {

	}
	return 0;
}
void WBstage::writeback(bufferDMWB bufDMWB,regfile &reg) {
	if (bufDMWB.wb.RegWrite)
		if (bufDMWB.wb.MemtoReg) 
			reg.reg[bufDMWB.rt] = bufDMWB.data;
		else
			reg.reg[bufDMWB.rt] = bufDMWB.ALU_result;
	else
		reg.WB = "WB";
}
