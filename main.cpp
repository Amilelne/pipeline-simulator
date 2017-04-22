#include<iostream>
#include<fstream>
#include<stdio.h>
#include<iomanip>
#include<stdint.h>
#include "simulate.h"
#include "regfile.h"
#include "Instruction.h"
const int32_t N = 258;
using namespace std;
int main()
{
	char* buffer;
	char* databuf;
	char iimage[] = "iimage.bin";
	char dimage[] = "dimage.bin";
	FILE* snapshot = fopen("snapshot.rpt", "w+");
	FILE* error_file = fopen("error_dump.rpt", "w+");
	long size;
	int instruction[N] = { 0 };
	int data[N] = { 0 };
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
	for (i = 0; i < 2; i++) 
		for (j = 0; j < 4; j++) 
			instruction[i] = instruction[i] * 16 * 16 + (unsigned int)((unsigned char)buffer[4 * i + j]);
	for (i = 0; i < instruction[1] + 2; i++) 
		for (j = 0; j < 4; j++) 
			instruction[i] = instruction[i] * 16 * 16 + (unsigned int)((unsigned char)buffer[4 * i + j]);
	for (i = 0; i < 2; i++) 
		for (j = 0; j < 4; j++) 
			data[i] = data[i] * 16 * 16 + (unsigned int)((unsigned char)(databuf[4 * i + j]));
	for (i = 2; i < data[1] + 2; i++) 
		for (j = 0; j < 4; j++) 
			data[i] = data[i] * 16 * 16 + (unsigned int)((unsigned char)(databuf[4 * i + j]));
	//initialize regfile
   regfile reg;
   Control control;
   ALUcontrol alucontrol;
   Hazard hazard;
   bufferIFID IFID;
   bufferIDEX IDEX;
   bufferEXDM EXDM;
   bufferDMWB DMWB;
   Instruction instru;
   IFstage IFstage;
   IDstage IDstage;
   EXstage EXstage;
   DMstage DMstage;
   WBstage WBstage;
   NOP nop;
   reg.IniPC = instruction[0];
   reg.PC = instruction[0];
   reg.SP = data[0];
   reg.IF = instruction[2];
   int cnt = 0;
   int rt_num = -1;
   while (reg.check_end() != false) {
	   printf("cycle %d\n",cnt);
	   fprintf(snapshot,"cycle %d\n", cnt);
	   if (reg.writeback) {
		   if (reg.write0)
			   fprintf(error_file, "In cycle %d: Write $0 Error\n", cnt);
		   else
			   for (int i = 1; i < 32; i++)
				   if (i == rt_num&&reg.reg[i] != reg.wb_data) {
					   printf("$%02d: 0x%08X\n", i, reg.reg[i]);
					   fprintf(snapshot, "$%02d: 0x%08X\n", i, reg.reg[i]);
				   }  
	   }
	   if (reg.mult) {
		   if (reg.HI_data != reg.reg[32]) {
			   printf("$HI: 0x%08X\n", reg.reg[32]);
			   fprintf(snapshot, "$HI: 0x%08X\n", reg.reg[32]);
		   }
		   if (reg.LO_data != reg.reg[33]) {
			   printf("$LO: 0x%08X\n", reg.reg[33]);
			   fprintf(snapshot, "$LO: 0x%08X\n", reg.reg[33]);
		   }
	   }
	   //reg.show();	   
	  // reg.show();
	   reg.BranchStall = hazard.Branch_Hazard(IFID,IDEX,EXDM);
	   if (!reg.BranchStall) {
		   reg.EX_ID_forward = hazard.EX_ID_hazard(IFID, IDEX, EXDM, reg);
		   reg.DM_EX_forward = hazard.MEM_hazard(DMWB, IDEX, EXDM, reg);
		   reg.EX_EX_forward = hazard.EX_hazard(EXDM,IDEX,reg);
		   if(reg.EX_ID_forward)
			   reg.EX_ID_rt_num = EXDM.wb.rt_num;
		   if (reg.DM_EX_forward)
			   reg.DM_EX_rt_num = DMWB.wb.rt_num;
		   if (reg.EX_EX_forward)
			   reg.EX_EX_rt_num = EXDM.wb.rt_num;
	   }
	   WBstage.writeback(DMWB, reg);
	   rt_num = DMWB.wb.rt_num;
	   DMstage.deal_memory(IDEX,EXDM, DMWB, data, reg);
	   DMWB.show();
	   EXstage.calculate(IDEX, EXDM,DMWB, reg,alucontrol,snapshot,nop);
	   EXDM.show();
	   if (reg.overwrite > 1)
		   fprintf(error_file, "In cycle %d: Overwrite HI-LO registers\n", cnt+1);
	   IDstage.instr_decode(IFID, IDEX,EXDM, control, reg, instru);
	   IDEX.show();
	   IFstage.instr_fetch(IFID, instruction, reg);
	   IFID.show();
	   //printf("rt_num=%d\n", DMWB.wb.rt_num);
	   if (cnt == 0)
		   reg.showall(snapshot);
	   else
		 reg.show(snapshot);
	   printf("#####$$$$$$reg.overwrite=%d\n\n", reg.overwrite);
	   cnt++;
   }
   fclose(snapshot);
   fclose(error_file);
}
