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
   bufferIDEX IDEX;
   Instruction instru;
   reg.PC = instruction[0];
   reg.SP = data[0];
   reg.IF = instruction[2];
   printf("cycle 0\n");
	reg.show();
	control.show();
	IDEX.instr_decode(instruction[2], control, reg,instru);
}
