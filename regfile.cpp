#include "regfile.h"
using namespace std;
void regfile::WBOP(int rt,int data) {
	reg[rt] = data;
}
void regfile::IFOP(int rs, int rt,int &rsdata,int &rtdata) {
	rsdata = reg[rs];
	rtdata = reg[rs];
}