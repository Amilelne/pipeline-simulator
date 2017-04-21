#pragma once
class Instruction {
public:
	char *ID;
	Instruction() {
		ID = "NOP";
	}
	void getname(int opcode, int funct) {
		if (opcode == 0x00) {
			switch (funct) {
			case 0x20:
				ID = "ADD";
				break;
			case 0x21:
				ID = "ADDU";
				break;
			case 0x22:
				ID = "SUB";
				break;
			case 0x24:
				ID = "AND";
				break;
			case 0x25:
				ID = "OR";
				break;
			case 0x26:
				ID = "XOR";
				break;
			case 0x27:
				ID = "NOR";
				break;
			case 0x28:
				ID = "NAND";
				break;
			case 0x2A:
				ID = "SLT";
				break;
			case 0x00:
				ID = "SLL";
				break;
			case 0x02:
				ID = "SRL";
				break;
			case 0x03:
				ID = "SRA";
				break;
			case 0x08:
				ID = "JR";
				break;
			case 0x18:
				ID = "MULT";
				break;
			case 0x19:
				ID = "MULTU";
				break;
			case 0x10:
				ID = "MFHI";
				break;
			case 0x12:
				ID = "MFLO";
				break;
			default:
				ID = "NOP";
				break;
			}
		}
		else if (opcode == 0x02 || opcode == 0x03) 
		{
			if (opcode == 0x02)
				ID = "J";
			else
				ID = "JAL";
		}
		else if (opcode == 0x3F) {
			ID = "HALT";
		}
		else {
			switch (opcode) {
			case 0x23:
				ID = "LW";
				break;
			case 0x08:
				ID = "ADDI";
				break;
			case 0X09:
				ID = "ADDIU";
				break;
			case 0X21:
				ID = "LH";
				break;
			case 0x25:
				ID = "LHU";
				break;
			case 0x20:
				ID = "LB";
				break;
			case 0x24:
				ID = "LBU";
				break;
			case 0x2B:
				ID = "SW";
				break;
			case 0x29:
				ID = "SH";
				break;
			case 0x28:
				ID = "SB";
				break;
			case 0x0F:
				ID = "LUI";
				break;
			case 0x0C:
				ID = "ANDI";
				break;
			case 0x0D:
				ID = "ORI";
				break;
			case 0x0E:
				ID = "NORI";
				break;
			case 0x0A:
				ID = "SLTI";
				break;
			case 0x04:
				ID = "BEQ";
				break;
			case 0x05:
				ID = "BNE";
				break;
			case 0x07:
				ID = "BGTZ";
				break;
			default:
				return;
			}
		}
	}
	void show() {
		printf("ID=%s\n", ID);
	}
};
