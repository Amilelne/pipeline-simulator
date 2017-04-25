#include "simulate.h"
#include "regfile.h"
#include "Instruction.h"
#include<iostream>
#include "arithmetic.h"
#include<string>
#include<stdlib.h>
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
	int tmp = reg.PC - reg.IniPC;
	if (tmp % 4 != 0)
		exit(0);
	reg.IF = instr[tmp / 4 + 2];
	//printf("########reg.IF=%8x,tmp=%d\n******", reg.IF,tmp);
	if (!reg.BranchStall) {
		if (reg.flush) {
			buf.instr = 0;
		}	
		else {
			buf.instr = instr[tmp / 4 + 2];
		}
		reg.PC = reg.PC + 4;
		buf.PC = reg.PC;
		buf.opcode = (0xfc000000 & buf.instr) >> 26;
		buf.func = (0x0000003f & buf.instr);
		buf.rs_num = (0x03e00000 & buf.instr) >> 21;
		buf.rt_num = (0x001f0000 & buf.instr) >> 16;
		buf.rd_num = (0x0000f800 & buf.instr) >> 11;
		buf.shamt = (0x000007c0 & buf.instr) >> 6;
		buf.immediate = (0x0000ffff & buf.instr);
	}
	else {
		reg.PC = reg.PC + 4;
	}
	//printf("instr=%08x,address = %d\n", instr[tmp / 4 + 2], tmp/4+2);
}
void IDstage::instr_decode(bufferIFID bufIFID,bufferIDEX &bufIDEX,bufferEXDM bufEXDM,Control &control,regfile &reg,Instruction &instruction) {
	int real_instr = bufIFID.instr;
	if (real_instr == 0)
		control.decode_nop();
	else
		control.decode_instr((0xfc000000 & real_instr) >> 26, (0x0000003f & real_instr));

	bufIDEX.immediate = (int32_t)((int16_t)(0x0000ffff & real_instr));
	bufIDEX.address = (0x03ffffff & real_instr);
	if (reg.BranchStall) {
		instruction.getname(bufIFID.opcode, bufIFID.func);
		reg.ID = instruction.ID;
		real_instr = 0;
		control.decode_nop();
		bufIDEX.immediate = (int32_t)((int16_t)(0x0000ffff & real_instr));
	}
	else {
		//deal with forward
		if (reg.EX_ID_forward == 0) {
			bufIDEX.rsdata = reg.reg[bufIFID.rs_num];
			bufIDEX.rtdata = reg.reg[bufIFID.rt_num];
		}
		else if (reg.EX_ID_forward == 1) {
			bufIDEX.rsdata = bufIFID.EX_data_forward;
			bufIDEX.rtdata = reg.reg[bufIFID.rt_num];
		}
		else if (reg.EX_ID_forward == 2) {
			bufIDEX.rsdata = reg.reg[bufIFID.rs_num];
			bufIDEX.rtdata = bufIFID.EX_data_forward;
		}
		else if (reg.EX_ID_forward == 3) {
			bufIDEX.rsdata = bufIFID.EX_data_forward;
			bufIDEX.rtdata = bufIFID.EX_data_forward;
		}
		//deal with branch
		if (control.Branch) {
			switch (bufIFID.opcode) {
			case 0x04:
				if (bufIDEX.rsdata == bufIDEX.rtdata) {
					reg.flush = true;
					reg.branchPC += bufIDEX.immediate << 2;
				}
				else {
					reg.flush = false;
					reg.branchPC = 0;
					reg.jumpflush = false;
				}	
				break;
			case 0x05:
				if (bufIDEX.rsdata != bufIDEX.rtdata) {
					printf("@@@@bufIDEX.rsdata=%d,bufIDEX.rtdata=%d,rt_num=%d,reg[0]=%d\n", bufIDEX.rsdata, bufIDEX.rtdata,bufIDEX.rt_num,reg.reg[0]);
					reg.branchPC += bufIDEX.immediate << 2;
					reg.flush = true;
				}
				else {
					reg.flush = false;
					reg.jumpflush = false;
					reg.branchPC = 0;
				}
				break;
			case 0x07:
				if (bufIDEX.rsdata > 0) {
					reg.branchPC += bufIDEX.immediate << 2;
					reg.flush = true;
				}
				else {
					reg.flush = false;
					reg.jumpflush = false;
					reg.branchPC = 0;
				}
				break;
			}
		}
		else if (control.Jump) {//deal with jump
			if (bufIFID.opcode == 0x02) {
				reg.flush = true;
				reg.jumpflush = true;
				reg.branchPC = ((reg.PC >> 28) << 28)+bufIDEX.address << 2;
			}
			else if (bufIFID.opcode == 0x03) {
				reg.flush = true;
				reg.jumpflush = true;
				bufIDEX.wb.rt_num = 31;
				bufIDEX.jal_PC = reg.PC;
				reg.branchPC = ((reg.PC >> 28) << 28)+bufIDEX.address << 2;
			}
			else if (bufIFID.func == 0x08) {
				reg.flush = true;
				reg.jumpflush = true;
				bufIDEX.jal_PC = reg.PC;
				reg.branchPC = bufIDEX.rsdata;
			}
			else {
				reg.flush = false;
				reg.jumpflush = false;
				reg.branchPC = 0;
			}
		}
		else {
			reg.flush = false;
			reg.jumpflush = false;
			reg.branchPC = 0;
		}	
	}
	int secondreg_num = -1;
	if (control.RegDst)
		secondreg_num = (0x0000f800 & real_instr) >> 11;
	else
		secondreg_num = (0x001f0000 & real_instr) >> 16;
	bufIDEX.opcode = (0xfc000000 & real_instr) >> 26;
	bufIDEX.func = (0x0000003f & real_instr);
	bufIDEX.rs_num = (0x03e00000 & real_instr) >> 21;
	bufIDEX.rt_num = (0x001f0000 & real_instr) >> 16;
	bufIDEX.rd_num = (0x0000f800 & real_instr) >> 11;
	bufIDEX.shamt = (0x000007c0 & real_instr) >> 6;
	bufIDEX.immediate = (int32_t)((int16_t)(0x0000ffff & real_instr));
	bufIDEX.address = (0x03ffffff & real_instr);
	if (real_instr == 0)
		instruction.ID = "NOP";
	else
		instruction.getname(bufIDEX.opcode, bufIDEX.func);
	bufIDEX.ex.ALUOp = control.ALUOp;
	bufIDEX.ex.RegDst = control.RegDst;
	bufIDEX.ex.ALUSrc = control.ALUSrc;
	bufIDEX.m.MemRead = control.MemRead;
	bufIDEX.wb.MemtoReg = control.MemtoReg;
	if (bufIFID.opcode != 0x03)
		bufIDEX.wb.rt_num = secondreg_num;
	bufIDEX.m.MemWrite = control.MemWrite;
	bufIDEX.wb.RegWrite = control.RegWrite;
	if(!reg.BranchStall)
		reg.ID = instruction.ID;
	instruction.show();
	if (bufIDEX.opcode == 0x00 && (bufIDEX.func == 0x10 || bufIDEX.func == 0x12))
		reg.overwrite = 0;
}
void Control::decode_instr(int opcode,int funct) {
	if (opcode == 0 && funct == 0x08) {
		RegDst = 0;
		Jump = 1;
		Branch = 0;
		MemRead = 0;
		MemWrite = 0;
		ALUOp = 0x11;
		MemtoReg = 0;
		RegWrite = 0;
	}
	else if (opcode == 0) {
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
		RegDst = 0;
		Branch = 0;
		MemRead = 0;
		MemtoReg = 0;
		ALUOp = 0x00;
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
		MemtoReg = 1;
	}
	else if (opcode == 0x28 || opcode == 0x29 || opcode == 0x2B) {
		RegDst = 0;
		Branch = 0;
		MemRead = 0;
		MemWrite = 1;
		ALUSrc = 1;
		RegWrite = 0;
		ALUOp = 0x00;
		MemtoReg = 0;
	}
	else if (opcode == 0x0C || opcode == 0x0A || opcode == 0x0D || opcode == 0x0E) {
		RegDst = 0;
		Branch = 0;
		MemRead = 0;
		MemtoReg = 0;
		ALUOp = 0x11;
		MemWrite = 0;
		ALUSrc = 0;
		RegWrite = 1;
	}
	else if (opcode == 0x0F) {
		RegDst = 0;
		Branch = 0;
		MemRead = 0;
		MemtoReg = 0;
		ALUOp = 0x11;
		MemWrite = 0;
		ALUSrc = 1;
		RegWrite = 1;
	}
	else if (opcode == 0x04 || opcode == 0x05 || opcode == 0x07) {
		RegDst = 0;
		Branch = 1;
		MemRead = 0;
		MemtoReg = 0;
		ALUOp = 0x11;
		MemWrite = 0;
		ALUSrc = 0;
		RegWrite = 0;
		Jump = 0;
	}
	else if (opcode == 0x02) {
		RegDst = 0;
		Jump = 1;
		Branch = 0;
		MemRead = 0;
		MemWrite = 0;
		ALUOp = 0x11;
		MemtoReg = 0;
		RegWrite = 0;
	}
	else if (opcode == 0x03) {
		RegDst = 0;
		Jump = 1;
		Branch = 0;
		MemRead = 0;
		MemWrite = 0;
		ALUOp = 0x11;
		MemtoReg = 0;
		RegWrite = 1;
	}
	else if (opcode == 0x3f) {
		RegDst = 0;
		Jump = 0;
		Branch = 0;
		MemRead = 0;
		MemWrite = 0;
		ALUOp = 0x11;
		MemtoReg = 0;
		RegWrite = 0;
	}
	else {
		RegWrite = 0;
		MemtoReg = 0;
	}
}
void EXstage::calculate(bufferIFID &bufIFID,bufferIDEX &bufIDEX, bufferEXDM &bufEXDM,bufferDMWB bufDMWB,regfile &reg,ALUcontrol &ALU_c,FILE* &snapshot,NOP nop) {
	bufIFID.EX_data_forward = bufEXDM.ALU_result;
	if (reg.DM_EX_forward || reg.EX_EX_forward) {
		int forward = reg.DM_EX_forward + reg.EX_EX_forward;
		switch (forward) {
		case 1:
			bufIDEX.rsdata = bufEXDM.ALU_result;
			break;
		case 2:
			bufIDEX.rtdata = bufEXDM.ALU_result;
			break;
		case 3:
			bufIDEX.rsdata = bufEXDM.ALU_result;
			bufIDEX.rtdata = bufEXDM.ALU_result;
			break;
		case 4:
			bufIDEX.rsdata = bufIDEX.forward_from_DMWB;
			break;
		case 7:
			bufIDEX.rtdata = bufIDEX.forward_from_DMWB;
			break;
		case 11:
			bufIDEX.rsdata = bufIDEX.forward_from_DMWB;
			bufIDEX.rtdata = bufIDEX.forward_from_DMWB;
			break;
		case 6:
			bufIDEX.rsdata = bufIDEX.forward_from_DMWB;
			bufIDEX.rtdata = bufEXDM.ALU_result;
			break;
		case 8:
			bufIDEX.rsdata = bufEXDM.ALU_result;
			bufIDEX.rtdata = bufIDEX.forward_from_DMWB;
			break;
		}
	}
	if (bufIDEX.opcode == 0x00 && (bufIDEX.func == 0x18 || bufIDEX.func == 0x19)) {
		reg.overwrite += 1;
		reg.mult = true;
		reg.HI_data = reg.reg[32];
		reg.LO_data = reg.reg[33];
		if (bufIDEX.func == 0x18) {
			int64_t product = 0;
			product = (int64_t)(int32_t)bufIDEX.rsdata * (int64_t)(int32_t)bufIDEX.rtdata;
			reg.reg[32] = (uint32_t)((product >> 32) & 0xffffffff);
			reg.reg[33] = (uint32_t)(0xffffffff & product);
		}
		else {
			int64_t tmp = 0x00000000ffffffff;
			uint64_t rsval = (uint64_t)(bufIDEX.rsdata & tmp);
			uint64_t rtval = (uint64_t)(bufIDEX.rtdata & tmp);
			uint64_t temp = (uint64_t)rsval*(uint64_t)rtval;
			reg.reg[32] = (uint32_t)((temp >> 32) & 0xffffffff);
			reg.reg[33] = (uint32_t)(0xffffffff & temp);
		}
		bufEXDM.ALU_result = 0;
		bufEXDM.rtdata = 0;
		bufEXDM.wb = nop.wb;
		bufEXDM.m = nop.m;
		bufEXDM.opcode = bufIDEX.opcode;
		reg.EX = reg.ID;
	}
	else if (bufIDEX.opcode == 0x03 || bufIDEX.opcode == 0x08 || bufIDEX.opcode == 0x09 || bufIDEX.opcode == 0x0F || bufIDEX.opcode == 0x0A || bufIDEX.opcode == 0x0C || bufIDEX.opcode == 0x0D || bufIDEX.opcode == 0x0E) {
		int32_t imme = (int32_t)((int16_t)bufIDEX.immediate);
		switch (bufIDEX.opcode)
		{
		case 0x03:
			bufEXDM.ALU_result = bufIDEX.jal_PC;
			break;
		case 0x08:
			bufEXDM.ALU_result = bufIDEX.rsdata + imme;
			if ((bufIDEX.rsdata > 0 && imme > 0 && bufEXDM.ALU_result <= 0) || (bufIDEX.rsdata < 0 && imme < 0 && bufEXDM.ALU_result >= 0))
				reg.number_overflow = true;
			else
				reg.number_overflow = false;
			break;
		case 0x09:
			bufEXDM.ALU_result = bufIDEX.rsdata + bufIDEX.immediate;
			break;
		case 0x0F:
			bufEXDM.ALU_result = bufIDEX.immediate << 16;
			break;
		case 0x0A:
			bufEXDM.ALU_result = (bufIDEX.rsdata < bufIDEX.immediate);
			break;
		case 0x0C:
			bufEXDM.ALU_result = bufIDEX.rsdata &(0x0000ffff & bufIDEX.immediate);
			break;
		case 0x0D:
			bufEXDM.ALU_result = bufIDEX.rsdata | (0xffff & bufIDEX.immediate);
			break;
		case 0x0E:
			bufEXDM.ALU_result = ~(bufIDEX.rsdata | (0xffff & bufIDEX.immediate));
			break;
		default:
			break;
		}
		bufEXDM.wb = bufIDEX.wb;
		bufEXDM.m = bufIDEX.m;
		bufEXDM.opcode = bufIDEX.opcode;
		reg.EX = reg.ID;
	}
	else if (bufIDEX.opcode == 0x00 && (bufIDEX.func == 0x00||bufIDEX.func == 0x02|| bufIDEX.func == 0x03|| bufIDEX.func == 0x08|| bufIDEX.func == 0x10 || bufIDEX.func == 0x12|| bufIDEX.func == 0x20 || bufIDEX.func == 0x21 || bufIDEX.func == 0x22 || bufIDEX.func == 0x24 || bufIDEX.func == 0x25 || bufIDEX.func == 0x26|| bufIDEX.func == 0x27|| bufIDEX.func == 0x28)) {
		int32_t rt = bufIDEX.rtdata;
		switch (bufIDEX.func) {
		case 0x00:
			bufEXDM.ALU_result = bufIDEX.rtdata << bufIDEX.shamt;
			break;
		case 0x02:
			bufEXDM.ALU_result = (uint32_t)bufIDEX.rtdata >> bufIDEX.shamt;
			break;
		case 0x03:
			bufEXDM.ALU_result = bufIDEX.rtdata >> bufIDEX.shamt;
			break;
		case 0x08:
			bufEXDM.ALU_result = bufIDEX.rsdata;
			break;
		case 0x10:
			bufEXDM.ALU_result = reg.reg[32];
			break;
		case 0x12:
			bufEXDM.ALU_result = reg.reg[33];
			break;
		case 0x20:
			bufEXDM.ALU_result = bufIDEX.rsdata + bufIDEX.rtdata;
			if ((bufIDEX.rsdata > 0 && bufIDEX.rtdata > 0 && bufEXDM.ALU_result <= 0) || (bufIDEX.rsdata < 0 && bufIDEX.rtdata < 0 && bufEXDM.ALU_result >= 0))
				reg.number_overflow = true;
			else
				reg.number_overflow = false;
			break;
		case 0x21:
			bufEXDM.ALU_result = bufIDEX.rsdata + bufIDEX.rtdata;
			break;
		case 0x22:
			bufEXDM.ALU_result = bufIDEX.rsdata + rt;
			if ((bufIDEX.rsdata > 0 && rt > 0 && bufEXDM.ALU_result <= 0) || (bufIDEX.rsdata < 0 && rt < 0 && bufEXDM.ALU_result >= 0))
				reg.number_overflow = true;
			else
				reg.number_overflow = false;
			break;
		case 0x24:
			bufEXDM.ALU_result = bufIDEX.rsdata & bufIDEX.rtdata;
			break;
		case 0x25:
			bufEXDM.ALU_result = bufIDEX.rsdata | bufIDEX.rtdata;
			break;
		case 0x26:
			bufEXDM.ALU_result = bufIDEX.rsdata ^ bufIDEX.rtdata;
			break;
		case 0x27:
			bufEXDM.ALU_result = ~(bufIDEX.rsdata | bufIDEX.rtdata);
			break;
		case 0x28:
			bufEXDM.ALU_result = ~(bufIDEX.rsdata & bufIDEX.rtdata);
			break;
		}
		bufEXDM.wb = bufIDEX.wb;
		bufEXDM.m = bufIDEX.m;
		bufEXDM.opcode = bufIDEX.opcode;
		reg.EX = reg.ID;
	}
	else {
		reg.mult = false;
		int second_data = 0;
		ALU_c.funct = bufIDEX.func;
		ALU_c.ALUOp = bufIDEX.ex.ALUOp;
		ALU_c.get_ALU_control();
		alu.ALU_control = ALU_c.ALU_control;
		if (bufIDEX.ex.ALUSrc == 0)
			second_data = bufIDEX.rtdata;
		else
			second_data = bufIDEX.immediate;
		bufEXDM.ALU_result = alu.alu(bufIDEX.rsdata, second_data);
		bufEXDM.rtdata = bufIDEX.rtdata;
		bufEXDM.wb = bufIDEX.wb;
		bufEXDM.m = bufIDEX.m;
		bufEXDM.opcode = bufIDEX.opcode;
		reg.EX = reg.ID;
	}
}

void DMstage::deal_memory(bufferIDEX &bufIDEX,bufferEXDM bufEXDM, bufferDMWB &bufDMWB,int data[],regfile &reg) {
	int readdata = 0;
	bufIDEX.forward_from_DMWB = bufDMWB.write_data;
	if (bufEXDM.m.MemRead) {
		bufDMWB.data = read_memory(bufEXDM,bufDMWB, data,reg);
	}
	else if (bufEXDM.m.MemWrite) {
		bufDMWB.data = write_memory(bufEXDM, data,reg);
	}
	bufDMWB.wb = bufEXDM.wb;
	bufDMWB.ALU_result = bufEXDM.ALU_result;
	reg.DM = reg.EX;
	if (bufEXDM.wb.MemtoReg)
		bufDMWB.write_data = bufDMWB.data;
	else
		bufDMWB.write_data = bufDMWB.ALU_result;
}
int DMstage::read_memory(bufferEXDM bufEXDM,bufferDMWB &bufDMWB,int data[],regfile &reg) {
	int opcode = bufEXDM.opcode;
	int rt_value = 0;
	if (opcode == 0x23) {
		if (bufEXDM.ALU_result > 1020) 
			reg.address_overflow = true;
		else {
			reg.address_overflow = false;
			rt_value = data[2 + bufEXDM.ALU_result / 4];
			printf("address=%d,rt_value=%08x\n", 2 + bufEXDM.ALU_result / 4);
		}
	}
	else if (opcode == 0x21) {
		if (bufEXDM.ALU_result > 1022)
			reg.address_overflow = true;
		else {
			reg.address_overflow = false;
			int pos = 2 + bufEXDM.ALU_result / 4;
			int pos1 = bufEXDM.ALU_result;
			if (pos1 % 4 == 0)
				rt_value = (int32_t)((int16_t)((data[pos] & 0xffff0000) >> 16));
			else if (pos1 % 4 == 2) 
				rt_value = (int32_t)((int16_t)((data[pos] & 0x0000ffff)));
		}
	}
	else if (opcode == 0x25) {
		if (bufEXDM.ALU_result > 1022)
			reg.address_overflow = true;
		else {
			reg.address_overflow = false;
			int pos = 2 + bufEXDM.ALU_result / 4;
			int pos1 = bufEXDM.ALU_result;
			if (pos1 % 4 == 0)
				rt_value = (uint32_t)((uint16_t)((data[pos] & 0xffff0000) >> 16));
			else if (pos1 % 4 == 2)
				rt_value = (uint32_t)((uint16_t)((data[pos] & 0x0000ffff)));
		}
	}
	else if (opcode == 0x20) {
		if (bufEXDM.ALU_result > 1023)
			reg.address_overflow = true;
		else {
			int pos = (bufEXDM.ALU_result) / 4 + 2;
			int pos1 = bufEXDM.ALU_result;
			if (pos1 % 4 == 0)
				rt_value = (int64_t)((int8_t)(data[pos] >> 24));
			else if (pos1 % 4 == 1)
				rt_value = (int64_t)(int8_t)(((data[pos] & 0x00ff0000) >> 16));
			else if (pos1 % 4 == 2)
				rt_value = (int64_t)((int8_t)((data[pos] & 0x0000ff00) >> 8));
			else
				rt_value = (int64_t)((int8_t)(data[pos] & 0x000000ff));
		}
	}
	else if (opcode == 0x24) {
		if (bufEXDM.ALU_result > 1023)
			reg.address_overflow = true;
		else {
			int pos = (bufEXDM.ALU_result) / 4 + 2;
			int pos1 = bufEXDM.ALU_result;
			if (pos1 % 4 == 0)
				rt_value = (uint64_t)((uint8_t)(data[pos] >> 24));
			else if (pos1 % 4 == 1)
				rt_value = (uint64_t)(uint8_t)(((data[pos] & 0x00ff0000) >> 16));
			else if (pos1 % 4 == 2)
				rt_value = (uint64_t)((uint8_t)((data[pos] & 0x0000ff00) >> 8));
			else
				rt_value = (uint64_t)((uint8_t)(data[pos] & 0x000000ff));
		}
	}
	return rt_value;
}
int DMstage::write_memory(bufferEXDM bufEXDM, int data[],regfile &reg) {
	int opcode = bufEXDM.opcode;
	if (opcode == 0x2B) {
		if (bufEXDM.ALU_result > 1020)
			reg.address_overflow = true;
		else {
			reg.address_overflow = false;
			data[bufEXDM.ALU_result / 4 + 2] = bufEXDM.rtdata;
			printf("@@@@@@data[256]=%08x@@@@@@@@@@@@@\n@@@@@@@\n", data[bufEXDM.ALU_result / 4 + 2]);
		}
	}
	else if (opcode == 0x29) {
		if (bufEXDM.ALU_result > 1022)
			reg.address_overflow = true;
		else {
			reg.address_overflow = false;
			data[bufEXDM.ALU_result / 4 + 2] = reg.reg[bufEXDM.wb.rt_num];
		}
	}
	else if (opcode == 0x28) {
		if (bufEXDM.ALU_result > 1023)
			reg.address_overflow = true;
		else {
			reg.address_overflow = false;
			data[bufEXDM.ALU_result / 4 + 2] = reg.reg[bufEXDM.wb.rt_num];
		}
	}
	return 0;
}
void WBstage::writeback(bufferDMWB bufDMWB,regfile &reg) {
	reg.WB = reg.DM;
	if (bufDMWB.wb.RegWrite) {
		reg.writeback = true;
		if (bufDMWB.wb.rt_num == 0) {
			reg.write0 = true;
		}
		else {
			reg.write0 = false;
			if (bufDMWB.wb.MemtoReg) {
				reg.wb_data = reg.reg[bufDMWB.wb.rt_num];
				reg.reg[bufDMWB.wb.rt_num] = bufDMWB.data;
				//reg.wb_num = bufDMWB.wb.rt_num;
			}
			else {
				reg.wb_data = reg.reg[bufDMWB.wb.rt_num];
				reg.reg[bufDMWB.wb.rt_num] = bufDMWB.ALU_result;
				//reg.wb_num = bufDMWB.wb.rt_num;
				//reg.wb_data = bufDMWB.ALU_result;
			}
		}
	}
	else {
		reg.writeback = false;
		reg.write0 = false;
	}
		
}
