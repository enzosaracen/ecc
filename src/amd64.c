#include "u.h"

Type *types[] = {
	[TVOID]		= &(Type){.t = TVOID};
	[TUCHAR]	= &(Type){.t = TUCHAR,	.width = 1};
	[TSHORT]	= &(Type){.t = TSHORT,	.width = 2};
	[TUSHORT]	= &(Type){.t = TUSHORT,	.width = 2};
	[TINT]		= &(Type){.t = TINT,	.width = 4};
	[TUINT]		= &(Type){.t = TUINT,	.width = 4};
	[TLONG]		= &(Type){.t = TLONG,	.width = 8};
	[TULONG]	= &(Type){.t = TULONG,	.width = 8};
	[TLLONG]	= &(Type){.t = TLLONG,	.width = 8};
	[TULLONG]	= &(Type){.t = TULLONG,	.width = 8};
	[TFLOAT]	= &(Type){.t = TFLOAT,	.width = 4};
	[TDOUBLE]	= &(Type){.t = TDOUBLE,	.width = 8};
	[TLDOUBLE]	= &(Type){.t = TLDOUBLE,	.width = 8};
	[TPTR]		= &(Type){.t = TPTR,	.width = 8};
};
