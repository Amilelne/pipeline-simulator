#include<stdio.h>
#include<fstream>
#pragma once
using namespace std;
class regfile {
public:
	int reg[34];
	int copyreg[34];
	//detect HI and LO
	bool overwrite;
	int IniPC;
	int PC;
	int IF;
	int SP;
	int rt_num;
	char  *ID;
	char *EX;
	char *DM;
	char  *WB;
	bool BranchStall;
	bool LoadUseStall;
	int EX_ID_forward;
	int EX_EX_forward;
	int DM_EX_forward;
	int for_rt_num;
	int wb_num;
	int wb_data;
	bool writeback;
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
		overwrite = false;
		BranchStall = false;
		LoadUseStall = false;
		writeback = false;
		EX_ID_forward = EX_EX_forward = DM_EX_forward = 0;
		for_rt_num = 0;
	}
	void showall(FILE* &snapshot) {
		for (int i = 0; i < 32; i++) {
			printf("$%02d: 0x%08X\n", i, reg[i]);
			fprintf(snapshot,"$%02d: 0x%08X\n", i, reg[i]);
		}
		printf("$HI: 0x%08X\n", reg[32]);
		fprintf(snapshot,"$HI: 0x%08X\n", reg[32]);
		printf("$LO: 0x%08X\n", reg[33]);
		printf("$PC: 0x%08X\n", PC-4);
		printf("$IF: 0x%08X\n", IF);
		printf("$ID: %s\n", ID);
		printf("$EX: %s\n", EX);
		printf("$DM: %s\n", DM);
		printf("$WB: %s\n", WB);
		for (int i = 0; i < 34; i++)
			copyreg[i] = reg[i];
	}
	void show(int num, FILE* &snapshot) {
		for (int i = 0; i < 32; i++)
			if (i == num&&reg[i]!=copyreg[i]) {
				printf("$%02d: 0x%08X\n", i, reg[i]);
			}
		if(num==32 && reg[32]!=copyreg[32])
			printf("$HI: 0x%08X\n", reg[32]);
		if (num == 33 && reg[33] != copyreg[33])
			printf("$LO: 0x%08X\n", reg[33]);
		printf("$PC: 0x%08X\n", PC-4);
		if (BranchStall||LoadUseStall) {
			printf("$IF: 0x%08X to_be_stalled\n", IF);
			printf("$ID: %s to_be_stalled\n", ID);
			ID = "NOP";
			PC = PC - 4;
		}
		else if (EX_ID_forward) {
			switch (EX_ID_forward)
			{
			case 1:
				printf("$IF: 0x%08X\n", IF);
				printf("$ID: %s fwd_EX-DM_rs_$%d\n", ID,for_rt_num);
				break;
			case 2:
				printf("$IF: 0x%08X\n", IF);
				printf("$ID: %s fwd_EX-DM_rt_$%d\n", ID, for_rt_num);
				break;
			case 3:
				printf("$IF: 0x%08X\n", IF);
				printf("$ID: %s fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d\n", for_rt_num, for_rt_num);
				break;
			default:
				break;
			}
		}
		else {
			printf("$IF: 0x%08X\n", IF);
			printf("$ID: %s\n", ID);
		}
		printf("$EX: %s\n", EX);
		printf("$DM: %s\n", DM);
		printf("$WB: %s\n", WB);
		for (int i = 0; i < 34; i++)
			copyreg[i] = reg[i];
		if (writeback)
			reg[wb_num] = wb_data;
	}
	void IFOP(int rs,int rt, int &rsdata, int &rtdata);
	void WBOP(int rt,int data);
	bool check_end() {
		if (ID == "HALT"&&EX == "HALT"&&DM == "HALT"&&WB == "HALT")
			return false;
		else
			return true;
	}
};
