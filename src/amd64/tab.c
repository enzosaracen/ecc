#define EXTERN
#include "g.h"

Type *types[] = {
	[TVOID]		= &(Type){.ttype = TVOID},
	[TCHAR]		= &(Type){.ttype = TCHAR,		.width = 1},
	[TUCHAR]	= &(Type){.ttype = TUCHAR,		.width = 1},
	[TSHORT]	= &(Type){.ttype = TSHORT,		.width = 2},
	[TUSHORT]	= &(Type){.ttype = TUSHORT,		.width = 2},
	[TINT]		= &(Type){.ttype = TINT,		.width = 4},
	[TUINT]		= &(Type){.ttype = TUINT,		.width = 4},
	[TLONG]		= &(Type){.ttype = TLONG,		.width = 8},
	[TULONG]	= &(Type){.ttype = TULONG,		.width = 8},
	[TFLOAT]	= &(Type){.ttype = TFLOAT,		.width = 4},
	[TDOUBLE]	= &(Type){.ttype = TDOUBLE,		.width = 8},
	[TPTR]		= &(Type){.ttype = TPTR,		.width = 8},
};

char *otab[] = {
	[IADDB]		= "addb",
	[IADDL]		= "addl",
	[IADDQ]		= "addq",
	[IADDW]		= "addw",
	[IJMP]		= "jmp",
	[IJNZ]		= "jnz",
	[IJZ]		= "jz",
	[IMOVB]		= "movb",
	[IMOVL]		= "movl",
	[IMOVQ]		= "movq",
	[IMOVW]		= "movw",
	[ITEST]		= "test",
	[IXOR]		= "xor",
};

char *rtab(int r, int w)
{
	switch(r) {
	case AX:
		switch(w) {
		case 1:	return "al";
		case 2:	return "ax";
		case 4:	return "eax";
		case 8:	return "rax";
		}
		break;
	case BX:
		switch(w) {
		case 1:	return "bl";
		case 2:	return "bx";
		case 4:	return "ebx";
		case 8:	return "rbx";
		}
		break;
	case CX:
		switch(w) {
		case 1:	return "bl";
		case 2:	return "bx";
		case 4:	return "ebx";
		case 8:	return "rbx";
		}
		break;
	case DX:
		switch(w) {
		case 1:	return "dl";
		case 2:	return "dx";
		case 4:	return "edx";
		case 8:	return "rdx";
		}
		break;
	case SI:
		switch(w) {
		case 1:	return "sil";
		case 2:	return "si";
		case 4:	return "esi";
		case 8:	return "rsi";
		}
		break;
	case DI:
		switch(w) {
		case 1:	return "dil";
		case 2:	return "di";
		case 4:	return "edi";
		case 8:	return "rdi";
		}
		break;
	case R8:
		switch(w) {
		case 1:	return "r8b";
		case 2:	return "r8w";
		case 4:	return "r8d";
		case 8:	return "r8";
		}
		break;
	case R9:
		switch(w) {
		case 1:	return "r9b";
		case 2:	return "r9w";
		case 4:	return "r9d";
		case 8:	return "r9";
		}
		break;
	case R10:
		switch(w) {
		case 1:	return "r10b";
		case 2:	return "r10w";
		case 4:	return "r10d";
		case 8:	return "r10";
		}
		break;
	case R11:
		switch(w) {
		case 1:	return "r11b";
		case 2:	return "r11w";
		case 4:	return "r11d";
		case 8:	return "r11";
		}
		break;
	case R12:
		switch(w) {
		case 1:	return "r12b";
		case 2:	return "r12w";
		case 4:	return "r12d";
		case 8:	return "r12";
		}
		break;
	case R13:
		switch(w) {
		case 1:	return "r13b";
		case 2:	return "r13w";
		case 4:	return "r13d";
		case 8:	return "r13";
		}
		break;
	case R14:
		switch(w) {
		case 1:	return "r14b";
		case 2:	return "r14w";
		case 4:	return "r14d";
		case 8:	return "r14";
		}
		break;
	case R15:
		switch(w) {
		case 1:	return "r15b";
		case 2:	return "r15w";
		case 4:	return "r15d";
		case 8:	return "r15";
		}
		break;
	case SP:
		switch(w) {
		case 1:	return "spl";
		case 2:	return "sp";
		case 4:	return "esp";
		case 8:	return "rsp";
		}
		break;
	case BP:
		switch(w) {
		case 1:	return "bpl";
		case 2:	return "bp";
		case 4:	return "ebp";
		case 8:	return "rbp";
		}
		break;
	}
	gerrorf("bad in rtab...");
}
