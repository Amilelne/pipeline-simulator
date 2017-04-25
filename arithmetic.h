#pragma once
class ALU {
public:
	int ALU_control;
public:
	int add(int rsdata,int rtdata) {
		int tmp = rsdata + rtdata;
		return tmp;
	}
	int subtract(int rsdata, int rtdata) {
		int tmp1 = -rtdata;
		int tmp2 = rsdata - rtdata;
		return tmp2;
	}
	int and(int rsdata, int rtdata) {
		return (rsdata&rtdata);
	}
	int or (int rsdata, int rtdata) {
		return (rsdata | rtdata);
	}
	int set_on_less_than(int rsdata,int rtdata) {
		return (rsdata < rtdata);
	}
	int alu(int rsdata, int rtdata) {
		if (ALU_control == 0x0010) {
			int reu = add(rsdata, rtdata);
			return reu;
		}
		else if (ALU_control == 0x0110) {
			int reu = subtract(rsdata, rtdata);
			return reu;
		}
		else if (ALU_control == 0x0000) {
			int reu = and (rsdata, rtdata);
			return reu;
		}
		else if (ALU_control == 0x0001) {
			int reu = or (rsdata, rtdata);
			return reu;
		}
		else if (ALU_control == 0x0111) {
			int reu = set_on_less_than(rsdata, rtdata);
			return reu;
		}
		else
			return 0;
	}
};
