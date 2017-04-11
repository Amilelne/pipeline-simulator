#include "regfile.h"
class bufferDMWB {
private:
	int data;
	int ALU_result;
	int rt;
	bool WB;
public:
	bufferDMWB();
	bufferDMWB(int d, int A, int r, bool c);
};
class bufferEXDM {
private:
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
private:
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
	bool equal;
	bool EX;
	bool DM;
	bool WB;
public:
	bufferIDEX() {
		equal = EX = DM = WB = false;
		rs_num = rt_num = rd_num = opcode = shamt = func = 0;
	}
	void instr_decode(int instru);
};
class bufferIFID {
private:
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
