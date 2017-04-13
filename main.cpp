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
   reg.PC = instruction[0];
   reg.SP = data[0];
   reg.IF = instruction[2];
   int cnt = 0;
   while (reg.check_end() != false) {
	   printf("cycle %d\n",cnt);
	   cnt++;
	   //reg.show();	   
	  // reg.show();
	   WBstage.writeback(DMWB, reg);
	   DMstage.deal_memory(EXDM, DMWB, data, reg);
	   EXstage.calculate(IDEX, EXDM, reg,alucontrol);
	   IDstage.instr_decode(IFID, IDEX, control, reg, instru);
	   IFstage.instr_fetch(IFID, instruction, reg);
	   reg.show();
   }
}
