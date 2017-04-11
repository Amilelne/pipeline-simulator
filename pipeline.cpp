/*
#include<iostream>
#include<string>
#include<fstream>
#include<stdio.h>
#include<iomanip>
#include<stdint.h>
const int32_t N = 258;
using namespace std;
int8_t opcode, rs, rt, rd, shamt, funct;
int16_t immediate;
int32_t address;
int32_t cycle[32] = { 0 };
int32_t IFIDbuf[3] = { 0 };
int32_t IDEXbuf[10] = { 0 };
int32_t EXDMbuf[10] = { 0 };
int32_t DMWBbuf[10] = { 0 };
uint32_t HI = 0;
uint32_t LO = 0;
uint32_t IF = 0;
char * ID = "NOP";
char * EX = "NOP";
char * DM = "NOP";
char * WB = "NOP";
uint32_t PC = IF;
uint32_t iniPC = PC;//save the initial value of PC
int32_t cycle_num = 0;//count how many cycle have run
int32_t stack[100] = { 0 };
int32_t rd_value = 0;
int32_t rt_value = 0;
int32_t cycle31_value = 0;
int32_t address_value = 0;
int64_t product = 0;
uint32_t HI_value = 0;
uint32_t LO_value = 0;
bool halt_flag = false;
bool b_flag = true;
FILE* snapshot = fopen_s("snapshot.rpt", "w+");
FILE* error_file = fopen_s("error_dump.rpt", "w+");
void instr_fetch(int32_t iniPC, int32_t PC, uint32_t instruction[]) {
	IF = instruction[(PC - iniPC) / 4 + 2];
	PC = PC + 4;
}
void instr_decode(uint32_t IF) {
	opcode = (0xfc000000 & IF) >> 26;
	if (opcode == 0x00) {
		funct = (0x0000003f & IF);
		rs = (0x03e00000 & IF) >> 21;
		rt = (0x001f0000 & IF) >> 16;
		rd = (0x0000f800 & IF) >> 11;
		switch (funct) {
		case 0x20:
			ID = "ADD";
			break;
		case 0x21:
			ID = "ADDU";
			break;
		case 0x22:
			ID = "SUB";
			break;
		case 0x24:
			ID = "AND";
			break;
		case 0x25:
			ID = "OR";
			break;
		case 0x26:
			ID = "XOR";
			break;
		case 0x27:
			ID = "NOR";
			break;
		case 0x28:
			ID = "NAND";
			break;
		case 0x2A:
			ID = "SLT";
			break;
		case 0x00:
			ID = "SLL";
			break;
		case 0x02:
			ID = "SRL";
			break;
		case 0x03:
			ID = "SRA";
			break;
		case 0x08:
			ID = "JR";
			break;
		case 0x18:
			ID = "MULT";
			break;
		case 0x19:
			ID = "MULTU";
			break;
		case 0x10:
			ID = "MFHI";
			break;
		case 0x12:
			ID = "MFLO";
			break;
		}
	}
	else if (opcode == 0x02 || opcode == 0x03) {
		address = (0x03ffffff & IF);
		if (opcode == 0x02)
			ID = "J";
		else
			ID = "JAL";
	}
	else if (opcode == 0x3F) {

	}
	else if ((0x04 <= opcode&&opcode <= 0x0F && opcode != 0x06 && opcode != 0x0B) || (0x20 <= opcode&&opcode <= 0x2B && opcode != 0x22 && opcode != 0x26 && opcode != 0x27 && opcode != 0x2A)) {
		rs = (0x03e00000 & IF) >> 21;
		rt = (0x001f0000 & IF) >> 16;
		immediate = (0x0000ffff & IF);
		if (opcode == 0x04)
			b_flag = (cycle[rs] == cycle[rt]);
		else if (opcode == 0x05)
			b_flag = (cycle[rs] != cycle[rt]);
		else if (opcode == 0x07)
			b_flag = (cycle[rs] > 0);
	}
	else {
		printf("illegal instruction!!!");
		exit(0);
	}
}
void execu() {
	if (opcode == 0x00) {
		switch (funct)
		{
		case 0x20:
			rd_value = cycle[rs] + cycle[rt];
			if ((cycle[rs] > 0 && cycle[rt] > 0 && rd_value <= 0) || (cycle[rs] < 0 && cycle[rt] < 0 && rd_value >= 0)) {
				fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
			}
			break;
		case 0x21:
			rd_value = cycle[rs] + cycle[rt];
			break;
		case 0x22:
			int32_t minu = -cycle[rt];
			rd_value = cycle[rs] + minu;
			if ((cycle[rs] < 0 && minu< 0 && rd_value >= 0) || (cycle[rs]>0 && minu>0 && rd_value <= 0)) {
				fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
			}
			break;
		case 0x24:
			rd_value = cycle[rs] & cycle[rt];
			break;
		case 0x25:
			rd_value = cycle[rs] | cycle[rt];
			break;
		case 0x26:
			rd_value = cycle[rs] ^ cycle[rt];
			break;
		case 0x27:
			rd_value = ~(cycle[rs] | cycle[rt]);
			break;
		case 0x28:
			rd_value = ~(cycle[rs] & cycle[rt]);
			break;
		case 0x2A:
			rd_value = (cycle[rs] < cycle[rt]);
			break;
		case 0x00:
			rd_value = cycle[rt] << shamt;
			break;
		case 0x02:
			uint32_t tmp = (uint32_t)cycle[rt];
			rd_value = tmp >> shamt;
			break;
		case 0x03:
			rd_value = cycle[rt] >> shamt;
			break;
		case 0x08:
			PC = cycle[rs];//check whether in this stage
			break;
		case 0x18:
			product = (int64_t)(int32_t)cycle[rs] * (int64_t)(int32_t)cycle[rt];
			HI_value = (uint32_t)((product >> 32) & 0xffffffff);
			LO_value = (uint32_t)(0xffffffff & product);
			break;
		case 0x19:
			int64_t tmp = 0x00000000ffffffff;
			uint64_t rsval = (uint64_t)(cycle[rs] & tmp);
			uint64_t rtval = (uint64_t)(cycle[rt] & tmp);
			uint64_t temp = (uint64_t)rsval*(uint64_t)rtval;
			HI_value = (uint32_t)((temp >> 32) & 0xffffffff);
			LO_value = (uint32_t)(0xffffffff & temp);
			break;
		case 0x10:
			rd_value = HI;
			break;
		case 0x12:
			rd_value = LO;
			break;
		default:
			break;
		}
	}
	else if (opcode == 0x08) {
		rt_value = cycle[rs] + immediate;
		if ((cycle[rs] > 0 && immediate > 0 && rt_value <= 0) || (cycle[rs] < 0 && immediate < 0 && rt_value >= 0)) {
			fprintf(error_file, "In cycle %d: Number Overflow\n", cycle_num);
		}
	}
	else if (opcode == 0x09) {
		rt_value = cycle[rs] + immediate;
	}
	else if (opcode == 0x23) {
		address_value = immediate + cycle[rs];
		if (address_value > 1020) {
			fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
			halt_flag = true;
		}
	}
	else if (opcode == 0x21 || opcode == 0x25) {
		address_value = immediate + cycle[rs];
		if (address_value > 1022) {
			fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
			halt_flag = true;
		}
	}
	else if (opcode == 0x20 || opcode == 0x24) {
		address_value = immediate + cycle[rs];
		if (address_value > 1023) {
			fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
			halt_flag = true;
		}
	}
	else if (opcode == 0x2B) {
		address_value = immediate + cycle[rs];
		if (address_value > 1020) {
			fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
			halt_flag = true;
		}
		rt_value = cycle[rt] & 0x0000FFFF;
	}
	else if (opcode == 0x29) {
		address_value = immediate + cycle[rs];
		if (address_value > 1022) {
			fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
			halt_flag = true;
		}
		rt_value = cycle[rt] & 0x0000FFFF;
	}
	else if (opcode == 0x28) {
		address_value = immediate + cycle[rs];
		if (address_value > 1023) {
			fprintf(error_file, "In cycle %d: Address Overflow\n", cycle_num);
			halt_flag = true;
		}
		rt_value = cycle[rt] & 0x0000FFFF;
	}
	else if (opcode == 0x0F) {
		rt_value = ((uint32_t)((uint16_t)immediate)) << 16;
	}
	else if (opcode == 0x0C) {
		rt_value = cycle[rs] & (uint16_t)immediate;
	}
	else if (opcode == 0x0D) {
		rt_value = cycle[rs] | (uint16_t)immediate;
	}
	else if (opcode == 0x0E) {
		rt_value = ~(cycle[rs] | (uint16_t)immediate);
	}
	else if (opcode == 0x0A) {
		rt_value = (cycle[rs] <(int16_t)immediate);
	}
	else if (opcode == 0x04) {

	}
	else if (opcode == 0x05) {

	}
	else if (opcode == 0x07) {

	}
	else if (opcode == 0x02) {
		PC = ((PC >> 28) << 28) + 4 * address;
	}
	else if (opcode == 0x03) {
		cycle31_value = PC;
		PC = ((PC >> 28) << 28) + 4 * address;
	}
}
void datamem(uint32_t data[]) {
	if (opcode == 0x23) {
		rt_value = data[2 + (immediate + cycle[rs]) / 4];
	}
	else if (opcode == 0x21) {
		int pos = (immediate + cycle[rs]) / 4 + 2;
		int pos1 = immediate + cycle[rs];
		if (pos1 % 4 == 0) {
			rt_value = (int32_t)((int16_t)((data[pos] & 0xffff0000) >> 16));
		}
		else if (pos1 % 4 == 2) {
			rt_value = (int32_t)((int16_t)((data[pos] & 0x0000ffff)));
		}
	}
	else if (opcode == 0x25) {
		int pos = immediate / 4 + 2;
		int pos1 = immediate + cycle[rs];
		if (pos1 % 4 == 0) {
			rt_value = (uint32_t)((uint16_t)((data[(immediate + cycle[rs]) / 4 + 2] & 0xffff0000) >> 16));
		}
		else if (pos1 % 4 == 2) {
			rt_value = (uint32_t)((uint16_t)((data[(immediate + cycle[rs]) / 4 + 2] & 0x0000ffff)));
		}
	}
	else if (opcode == 0x20) {
		int pos = (immediate + cycle[rs]) / 4 + 2;
		int pos1 = immediate + cycle[rs];
		if (pos1 % 4 == 0)
			rt_value = (int64_t)((int8_t)(data[pos] >> 24));
		else if (pos1 % 4 == 1)
			rt_value = (int64_t)(int8_t)(((data[pos] & 0x00ff0000) >> 16));
		else if (pos1 % 4 == 2)
			rt_value = (int64_t)((int8_t)((data[pos] & 0x0000ff00) >> 8));
		else
			rt_value = (int64_t)((int8_t)(data[pos] & 0x000000ff));
	}
	else if (opcode == 0x24) {
		int pos = (immediate + cycle[rs]) / 4 + 2;
		int pos1 = immediate + cycle[rs];
		if (pos1 % 4 == 0)
			rt_value = (unsigned int)((data[pos] & 0xff000000) >> 24);
		else if (pos1 % 4 == 1)
			rt_value = (unsigned int)((data[pos] & 0x00ff0000) >> 16);
		else if (pos1 % 4 == 2)
			rt_value = (unsigned int)((data[pos] & 0x0000ff00) >> 8);
		else
			rt_value = (unsigned int)(data[pos] & 0x000000ff);
	}
	else if (opcode == 0x2B) {
		data[(immediate + cycle[rs]) / 4 + 2] = rt_value;
	}
	else if (opcode == 0x29) {
		int pos = (immediate + cycle[rs]) / 4 + 2;
		int pos1 = immediate + cycle[rs];
		if (pos1 % 4 == 0) {
			data[pos] = (data[pos] & 0x0000ffff) | ((rt_value & 0xffff) << 16);
		}
		else if (pos1 % 4 == 2) {
			data[pos] = (data[pos] & 0xffff0000) | (rt_value & 0xffff);
		}
	}
	else if (opcode == 0x28) {
		int32_t pos = (immediate + cycle[rs]) / 4 + 2;
		int pos1 = immediate + cycle[rs];
		if (pos1 % 4 == 0)
			data[pos] = (data[pos] & 0xffffff) | ((rt_value & 0xff) << 24);
		else if (pos1 % 4 == 1)
			data[pos] = (data[pos] & 0xff00ffff) | ((rt_value & 0xff) << 16);
		else if (pos1 % 4 == 2)
			data[pos] = (data[pos] & 0xffff00ff) | ((rt_value & 0xff) << 8);
		else
			data[pos] = (data[pos] & 0xffffff00) | (rt_value & 0xff);
	}
}
void writeback() {

}
int main()
{
	char* buffer;
	char* databuf;
	char iimage[] = "iimage.bin";
	char dimage[] = "dimage.bin";
	long size;
	uint32_t instruction[N] = { 0 };
	uint32_t data[N] = { 0 };
	int instru_num = 0;
	int data_num = 0;
	ifstream file(iimage, ios::in | ios::binary | ios::ate);
	size = file.tellg();
	instru_num = size / 4;
	file.seekg(0, ios::beg);
	buffer = new char[size];
	file.read(buffer, size);
	file.close();
	ifstream datafile(dimage, ios::in | ios::binary | ios::ate);
	long datasize = datafile.tellg();
	datafile.seekg(0, ios::beg);
	databuf = new char[datasize];
	datafile.read(databuf, datasize);
	datafile.close();
	int i = 0; int j = 0;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			instruction[i] = instruction[i] * 16 * 16 + (unsigned int)((unsigned char)buffer[4 * i + j]);
		}
	}
	for (i = 0; i < instruction[1] + 2; i++) {
		for (j = 0; j < 4; j++) {
			instruction[i] = instruction[i] * 16 * 16 + (unsigned int)((unsigned char)buffer[4 * i + j]);
		}
	}
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			data[i] = data[i] * 16 * 16 + (unsigned int)((unsigned char)(databuf[4 * i + j]));
		}
	}
	for (i = 2; i < data[1] + 2; i++) {
		for (j = 0; j < 4; j++) {
			data[i] = data[i] * 16 * 16 + (unsigned int)((unsigned char)(databuf[4 * i + j]));
		}
	}
	//number of words to be loadede into I memory
	unsigned int exec_instru = (unsigned int)instruction[1];
	//number of datas to be loaded into D memory
	unsigned int load_data = (unsigned int)data[1];
	while (WB != "halt") {
		writeback();
		datamem(data);
		execu();
		instr_decode(IF);
		instr_fetch(iniPC, PC, instruction);
	}
	return 0;
}
*/