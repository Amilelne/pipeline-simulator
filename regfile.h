#include<stdio.h>
#include<fstream>
#pragma once
using namespace std;
class regfile {
public:
	int reg[34];
	//detect HI and LO
	int overwrite;
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
	int EX_ID_forward;
	int EX_EX_forward;
	int DM_EX_forward;
	int EX_ID_rt_num;
	int EX_EX_rt_num;
	int DM_EX_rt_num;
	int wb_num;
	int wb_data;
	int HI_data;
	int LO_data;
	bool writeback;
	bool mult;
	bool write0;
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
		overwrite = 0;
		BranchStall = false;
		writeback = false;
		mult = false;
		write0 = false;
		EX_ID_forward = EX_EX_forward = DM_EX_forward = 0;
		EX_ID_rt_num = EX_EX_rt_num= DM_EX_rt_num = 0;
		HI_data = LO_data = 0;
	}
	void showall(FILE* &snapshot) {
		for (int i = 0; i < 32; i++) {
			printf("$%02d: 0x%08X\n", i, reg[i]);
			fprintf(snapshot,"$%02d: 0x%08X\n", i, reg[i]);
		}
		printf("$HI: 0x%08X\n", reg[32]);
		fprintf(snapshot,"$HI: 0x%08X\n", reg[32]);
		printf("$LO: 0x%08X\n", reg[33]);
		fprintf(snapshot,"$LO: 0x%08X\n", reg[33]);
		printf("$PC: 0x%08X\n", PC-4);
		fprintf(snapshot,"$PC: 0x%08X\n", PC - 4);
		printf("$IF: 0x%08X\n", IF);
		fprintf(snapshot,"$IF: 0x%08X\n", IF);
		printf("$ID: %s\n", ID);
		fprintf(snapshot,"$ID: %s\n", ID);
		printf("$EX: %s\n", EX);
		fprintf(snapshot,"$EX: %s\n", EX);
		printf("$DM: %s\n", DM);
		fprintf(snapshot,"$DM: %s\n", DM);
		printf("$WB: %s\n", WB);
		fprintf(snapshot,"$WB: %s\n\n\n", WB);
	}
	void show(FILE* &snapshot) {
		/*
		for (int i = 0; i < 32; i++)
			if (i == num&&reg[i]!=copyreg[i]) {
				printf("$%02d: 0x%08X\n", i, reg[i]);
				fprintf(snapshot,"$%02d: 0x%08X\n", i, reg[i]);
			}*/
		printf("$PC: 0x%08X\n", PC-4);
		fprintf(snapshot,"$PC: 0x%08X\n", PC - 4);
		if (BranchStall) {
			printf("$IF: 0x%08X to_be_stalled\n", IF);
			fprintf(snapshot,"$IF: 0x%08X to_be_stalled\n", IF);
			printf("$ID: %s to_be_stalled\n", ID);
			fprintf(snapshot,"$ID: %s to_be_stalled\n", ID);
			ID = "NOP";
			PC = PC - 4;
		}
		else if (EX_ID_forward) {
			switch (EX_ID_forward)
			{
			case 1:
				printf("$IF: 0x%08X\n", IF);
				fprintf(snapshot,"$IF: 0x%08X\n", IF);
				printf("$ID: %s fwd_EX-DM_rs_$%d\n", ID,EX_ID_rt_num);
				fprintf(snapshot,"$ID: %s fwd_EX-DM_rs_$%d\n", ID, EX_ID_rt_num);
				break;
			case 2:
				printf("$IF: 0x%08X\n", IF);
				fprintf(snapshot,"$IF: 0x%08X\n", IF);
				printf("$ID: %s fwd_EX-DM_rt_$%d\n", ID, EX_ID_rt_num);
				fprintf(snapshot,"$ID: %s fwd_EX-DM_rt_$%d\n", ID, EX_ID_rt_num);
				break;
			case 3:
				printf("$IF: 0x%08X\n", IF);
				fprintf(snapshot,"$IF: 0x%08X\n", IF);
				printf("$ID: %s fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d\n", EX_ID_rt_num, EX_ID_rt_num);
				fprintf(snapshot,"$ID: %s fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d\n", EX_ID_rt_num, EX_ID_rt_num);
				break;
			default:
				break;
			}
		}
		else {
			printf("$IF: 0x%08X\n", IF);
			fprintf(snapshot,"$IF: 0x%08X\n", IF);
			printf("$ID: %s\n", ID);
			fprintf(snapshot,"$ID: %s\n", ID);
		}
		if (DM_EX_forward || EX_EX_forward) {
			int forward = DM_EX_forward + EX_EX_forward;
			switch (forward)
			{
			case 1:
				printf("$EX: %s fwd_EX-DM_rs_$%d\n", EX, EX_EX_rt_num);
				fprintf(snapshot, "$EX: %s fwd_EX-DM_rs_$%d\n", EX, EX_EX_rt_num);
				break;
			case 2:
				printf("$EX: %s fwd_EX-DM_rt_$%d\n", EX, EX_EX_rt_num);
				fprintf(snapshot, "$EX: %s fwd_EX-DM_rt_$%d\n", EX, EX_EX_rt_num);
				break;
			case 3:
				printf("$EX: %s fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d\n", EX, EX_EX_rt_num, EX_EX_rt_num);
				fprintf(snapshot, "$EX: %s fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d\n", EX, EX_EX_rt_num, EX_EX_rt_num);
				break;
			case 4:
				printf("$EX: %s fwd_DM-WB_rs_$%d\n", EX,DM_EX_rt_num);
				fprintf(snapshot, "$EX: %s fwd_DM-WB_rs_$%d\n", EX, DM_EX_rt_num);
				break;
			case 7:
				printf("$EX: %s fwd_DM-WB_rt_$%d\n", EX, DM_EX_rt_num);
				fprintf(snapshot, "$EX: %s fwd_DM-WB_rt_$%d\n", EX, DM_EX_rt_num);
				break;
			case 11:
				printf("$EX: %s fwd_DM-WB_rs_$%d fwd_DM-WB_rt_$%d\n", EX, DM_EX_rt_num, DM_EX_rt_num);
				fprintf(snapshot, "$EX: %s fwd_DM-WB_rs_$%d fwd_DM-WB_rt_$%d\n", EX, DM_EX_rt_num, DM_EX_rt_num);
				break;
			case 6:
				printf("$EX: %s fwd_DM-WB_rs_$%d fwd_EX-DM_rt_$%d\n", EX, DM_EX_rt_num, EX_EX_rt_num);
				fprintf(snapshot, "$EX: %s fwd_DM-WB_rs_$%d fwd_EX-DM_rt_$%d\n", EX, DM_EX_rt_num, EX_EX_rt_num);
				break;
			case 8:
				printf("$EX: %s fwd_EX-DM_rs_$%d fwd_DM-WB_rt_$%d\n", EX, EX_EX_rt_num, DM_EX_rt_num);
				fprintf(snapshot, "$EX: %s fwd_EX-DM_rs_$%d fwd_DM-WB_rt_$%d\n", EX, EX_EX_rt_num, DM_EX_rt_num);
				break;
			default:
				break;
			}
		}
		else {
			printf("$EX: %s\n", EX);
			fprintf(snapshot, "$EX: %s\n", EX);
		}
		printf("$DM: %s\n", DM);
		fprintf(snapshot,"$DM: %s\n", DM);
		printf("$WB: %s\n", WB);
		fprintf(snapshot,"$WB: %s\n\n\n", WB);
		if (writeback) {
			reg[wb_num] = wb_data;
			printf("%%%%####wb_num,wb_data=%d,%d\n", wb_num, wb_data);
		}		
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
