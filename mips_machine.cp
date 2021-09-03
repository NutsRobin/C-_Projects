// Mips Machine Lab
// Simulates a mips machine
// Justin Bowers
// 03/26/21

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <sstream>
#include <cstring>

using namespace std;

struct Machine {
	int regs[32];
	unsigned int pc;
	unsigned int num_instructions;
	unsigned int *instructions;
	bool reg(const string &name, unsigned int &val);
	bool run_instruction();
	int init_mach(string input_2);

};

const int NUM_REGS = 32;
const string reg_names[] = {
	"$zero",
	"$at",
	"$v0",
	"$v1",
	"$a0",
	"$a1",
	"$a2",
	"$a3",
	"$t0",
	"$t1",
	"$t2",
	"$t3",
	"$t4",
	"$t5",
	"$t6",
	"$t7",
	"$s0",
	"$s1",
	"$s2",
	"$s3",
	"$s4",
	"$s5",
	"$s6",
	"$s7",
	"$t8",
	"$t9",
	"$k0",
	"$k1",
	"$gp",
	"$sp",
	"$fp",
	"$ra"
};

// (sub)opcode cases
const unsigned int ADD_SOP = 32;
const unsigned int AND_SOP = 36;
const unsigned int OR_SOP = 37;
const unsigned int SLL_SOP = 0;
const unsigned int SLT_SOP = 42;
const unsigned int SRA_SOP = 3;
const unsigned int SRL_SOP = 2;
const unsigned int ADDI_OP = 8;
const unsigned int BEQ_OP = 4;
const unsigned int BNE_OP = 5;



bool Machine::reg(const string &name, unsigned int &val)
{
	for (int i = 0; i < NUM_REGS; ++i)
	{
		if (name == reg_names[i]) {
			val = regs[i];
			return true;
		}

	}
	return false;
}

int main() {

	Machine *mach = nullptr;
	string sinput;
	string input;
	string input_2;
	unsigned int val;

	while(true) {
		cout << "> ";
		getline(cin,  sinput);
		istringstream ss(sinput);
		ss >> input >> input_2;
		if(sinput == "regs") {
			// check if machine is loaded
			if (mach == nullptr) {
				printf("No machine loaded.\n");
				continue;
			}

			// loop through registers, output in format
			for (int i = 0; i < NUM_REGS; ++i) {
				printf("%-5s: 0x%.8x (%4d) ", reg_names[i].c_str(), mach->regs[i], mach->regs[i]);
				if ((i == 3) ||(i ==  7) || (i == 11) || (i == 15) ||
						(i == 19) || (i == 23) || ( i == 27) || (i == 31)) {
					printf("\n");
				}
			}
			printf("%-5s: %d\n", "PC", mach->pc);

		}
		else if (sinput == "quit") { // exit program
			return 0;
			break;
		}

		else if(input == "reg") {
			// check if machine is loaded
			if (mach == nullptr) {
				printf("No machine loaded.\n");
				continue;
			}

			// print user specified register
			if (mach->reg(input_2, val)) {
				printf("%s: 0x%.8x (%4d)\n", input_2.c_str(), val, val);
			}
			else {printf("Invalid register.\n");}
		}

		else if(sinput == "next") {
			// check if machine is loaded
			if (mach == nullptr) {
				printf("No machine loaded.\n");
				continue;
			}

			// run next Mips instruction
			mach->run_instruction();
		}

		else if(input == "run") {
			// check if machine is loaded
			if (mach == nullptr) {
				printf("No machine loaded.\n");
				continue;
			}

			// execute all REMAINING mips instructions
			while (mach->pc < mach->num_instructions * 4) {
				mach->run_instruction();
			}
		} 

		else if (input == "load") {
			mach = new Machine[sizeof(*mach)];
			mach->init_mach(input_2);
		}
	}

	delete[] mach->instructions;
	delete[] mach;
	return 0;
}
int Machine::init_mach(string input_2) {
	// this function will load and read a binary
	// file into memory for our simulator to perform
	// various commands on


	// open file
	FILE *fin;
	fin = fopen(input_2.c_str(), "rb");
	if (fin == NULL) {
		perror("Failed to open file.\n");
		fclose(fin);
		return 1;
	}
	else {
		// read from opened file
		fseek(fin, 0L, SEEK_END);

		// calculate num_instructions
		num_instructions = ftell(fin) / 4;
		fseek(fin, 0L, SEEK_SET);

		// allocate memory for instructions
		instructions = new unsigned int[num_instructions];
		// read into memory
		fread(instructions, 1, num_instructions * 4, fin);

		pc = 0;
		regs[32] = 0;

		fclose(fin);
		return 0;
	}
}

bool Machine::run_instruction() {
	// This function will decode a point in memory
	// and run the corresponding command
		
	// check for valid pc value
	if (pc >= num_instructions * 4) {
		printf("No more instructions to run.\n");

		return false;
	}

	// index to instruction and get opcode
	int curr_instr = instructions[pc / 4];
	unsigned int opcode = (curr_instr >> 26) & 0x3f;
	int rt = 0;
	int rs = 0;
	int rd = 0;

	if (opcode == 0) {
		unsigned int sub_opcode = curr_instr & 0x3F;
		unsigned int sa = 0;
		unsigned int srl = 0;
		switch (sub_opcode) {
			case ADD_SOP:
				// extract instruction bits
				rd = (curr_instr >> 11) & 0x1F;
				rs = (curr_instr >> 21) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;

				// perform operation on registers
				regs[rd] = regs[rs] + regs[rt];
				if (rd == 0)
					regs[rd] = 0;

				// update pc
				pc += 4;
				break;

			case AND_SOP:
				// extract instruction bits
				rd = (curr_instr >> 11) & 0x1F;
				rs = (curr_instr >> 21) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;

				// perform operation on registers
				regs[rd] = regs[rs] & regs[rt];
				if (rd == 0)
					regs[rd] = 0;

				// update pc
				pc += 4;
				break;

			case OR_SOP:
				// extract instruction bits
				rd = (curr_instr >> 11) & 0x1F;
				rs = (curr_instr >> 21) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;

				// perform operation on registers
				regs[rd] = regs[rs] | regs[rt];
				if (rd == 0)
					regs[rd] = 0;

				// update pc
				pc += 4;
				break;

			case SLL_SOP:
				// extract instruction bits
				rd = (curr_instr >> 11) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;
				sa = (curr_instr >> 6) & 0x1F;

				// perform operation on registers
				regs[rd] = regs[rt] << sa;
				if (rd == 0)
					regs[rd] = 0;

				// update pc
				pc += 4;
				break;

			case SLT_SOP:
				// extract instruction bits
				rd = (curr_instr >> 11) & 0x1F;
				rs = (curr_instr >> 21) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;

				// perform operation
				if (regs[rs] < regs[rt]) {
					regs[rd] = 1;
				}
				else {regs[rd] = 0;}
				if (rd == 0)
					regs[rd] = 0;

				// update pc
				pc += 4;
				break;

			case SRA_SOP:
				rd = (curr_instr >> 11) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;
				sa = (curr_instr >> 6) & 0x1F;

				// perform operation on registers
				regs[rd] = regs[rt] >> sa;
				if (rd == 0)
					regs[rd] = 0;

				// update pc
				pc += 4;
				break;

			case SRL_SOP:
				// extract instruction bits
				rd = (curr_instr >> 11) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;
				sa = (curr_instr >> 6) & 0x1F;

				// perform operation on registers
				srl = regs[rt];
				regs[rd] = srl >> sa;
				regs[rd] = regs[rd] ^ (0 << sa);
				if (rd == 0)
					regs[rd] = 0;

				// update pc
				pc += 4;
				break;

			default:
				printf("Unknown instruction...continuing.\n");
				pc += 4;
				break;

		}
	}

	else {
		int imm = 0;
		int off = 0;
		switch (opcode) {
			case ADDI_OP:
				// extract instruction bits
				rt = (curr_instr >> 16) & 0x1F;
				rs = (curr_instr >> 21) & 0x1F;
				imm = curr_instr & 0xFFFF;

				// assign to registers and perform instruction
				if ((imm >> 15) == 1) {
					imm = (imm << 16) >> 16; 
				}
				regs[rt] = regs[rs] + imm;
				if (rt == 0)
					regs[rt] = 0;

				// update pc
				pc += 4;
				break;

			case BEQ_OP:
				// extract instruction bits
				rs = (curr_instr >> 21) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;
				off = (curr_instr) & 0xFFFF;

				// branch and update PC
				if ((off >> 15) == 1) {
					off = (off << 16) >> 16;
				}
				
				if (regs[rs] == regs[rt]) {
					pc += (off * 4) + 4;
				}
				else {pc += 4;}
				break;

			case BNE_OP:
				// extract instruction bits
				rs = (curr_instr >> 21) & 0x1F;
				rt = (curr_instr >> 16) & 0x1F;
				off = curr_instr & 0xFFFF;

				// branch and update PC
				if ((off >> 15) == 1) {
					off = (off << 16) >> 16;
				}
				if (regs[rs] != regs[rt]) {
					pc += (off * 4) + 4;
				}
				break;

			default:
				printf("Unknown instruction...continuing.\n");
				pc += 4;
				break;
		}
	}

	return true;
}
