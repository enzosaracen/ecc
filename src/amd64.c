#include "u.h"

Type *types[] = {
	[TVOID]		= &(Type){.ttype = TVOID},
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
