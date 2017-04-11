#pragma once
class ALU {
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
};
