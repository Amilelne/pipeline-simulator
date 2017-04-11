#include<stdio.h>
#pragma once
class regfile {
public:
	int reg[34];
	//detect HI and LO
	bool overwrite;
	int IniPC;
	int PC;
	int IF;
	int SP;
	char  *ID;
	char *EX;
	char *DM;
	char  *WB;
public:
	regfile() {
		reg[34] = { 0 };
		//reg[32]=HI,reg[33]=LO
		PC = 0;
		IniPC = 0;
		ID = "NOP";
		EX = "NOP";
		DM = "NOP";
		WB = "NOP";
		bool overwrite = false;
	}
	void show() {
		for (int i = 0; i < 32; i++)
			printf("$%02d: 0x%08X\n", i + 1, reg[i]);
		printf("$HI: 0x%08X\n", reg[32]);
		printf("$LO: 0x%08X\n", reg[33]);
		printf("$PC: 0x%08X\n", PC);
		printf("$IF: 0x%08X\n", IF);
		printf("$ID: %s\n", ID);
		printf("$EX: %s\n", EX);
		printf("$DM: %s\n", DM);
		printf("$WB: %s\n", WB);
	}
	void IFOP(int rs,int rt, int &rsdata, int &rtdata);
	void WBOP(int rt,int data);
};
