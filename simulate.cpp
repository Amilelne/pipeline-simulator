#include "simulate.h"
#include "regfile.h"
#include "Instruction.h"
#include<iostream>
using namespace std;
bufferDMWB::bufferDMWB() {
	data = 0;
	ALU_result = 0;
	rt = 0;
	WB = false;
}
bufferDMWB::bufferDMWB(int d, int A, int r, bool c) {
	data = d;
	ALU_result = A;
	rt = r;
	WB = c;
}
bufferEXDM::bufferEXDM() {
	ALU_result = 0;
	address = 0;
	rt = 0;
	WB = false;
	DM = false;
}
bufferEXDM::bufferEXDM(int A, int a, int r, bool W, bool D) {
	ALU_result = A;
	address = a;
	rt = r;
	WB = W;
	DM = D;
}
void bufferIFID::instr_fetch(int IniPC,int PC,int instr[],regfile &reg) {
	int tmp = PC - IniPC;
	if (tmp % 4 != 0)
		exit(0);
	reg.IF = instr[tmp / 4 + 2];
	reg.PC = PC + 4;
}
void bufferIDEX::instr_decode(int instr,Control &control,regfile &reg,Instruction &instruction) {
	opcode = (0xfc000000 & instr) >> 26;
	func = (0x0000003f & instr);
	rs_num = (0x03e00000 & instr) >> 21;
	rt_num = (0x001f0000 & instr) >> 16;
	rd_num = (0x0000f800 & instr) >> 11;
	immediate = (0x0000ffff & instr);
	rsdata = reg.reg[rs_num];
	rtdata = reg.reg[rt_num];
	control.opcode = opcode;
	instruction.getname(opcode,func);
	instruction.show();
	printf("%x,%d,%d,%d,%x,%x,%x", opcode, rs_num, rt_num, rd_num, immediate, rsdata, rtdata);
}
