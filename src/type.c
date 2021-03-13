#include "u.h"
#include "y.tab.h"

enum {
	BVOID		= 1<<0,
	BCHAR		= 1<<1,
	BSHORT		= 1<<2,
	BINT		= 1<<3,
	BLONG		= 1<<4,
	BFLOAT		= 1<<5,
	BDOUBLE		= 1<<6,
	BSIGNED		= 1<<7,
	BUNSIGNED	= 1<<8,
	BSTRUCT		= 1<<9,
	BENUM		= 1<<10,
};

Type *type()

int bitstype(void)
{
	switch(bits) {
	case BVOID:
		return TVOID;
	case BCHAR:
	case BCHAR|BSIGNED:
		return TCHAR;
	case BCHAR|BUNSIGNED:
		return TUCHAR;
	case BSHORT:
	case BSHORT|BINT:
	case BSHORT|BSIGNED:
	case BSHORT|BINT|BSIGNED:
		return TSHORT;
	case BSHORT|BUNSIGNED:
	case BSHORT|BUNSIGNED|BINT:
		return TUSHORT;
	case 0:
	case BINT:
	case BSIGNED:
	case BINT|BSIGNED:
		return TINT;
	case BUNSIGNED:
	case BINT|BUNSIGNED:
		return TUINT;
	case BLONG:
	case BLONG|BINT:
	case BLONG|BSIGNED:
	case BLONG|BINT|BSIGNED:
		return TLONG;
	case BFLOAT:
		return TFLOAT;
	case BDOUBLE:
		return TDOUBLE;
	case BDOUBLE|BLONG:
		return TLDOUBLE;
	case BSTRUCT:
		return TSTRUCT;
	case BENUM:
		return TENUM;
	}
	errorf("bad type specifier");
}

void spec(int lex)
{
	switch(lex) {
	case LCONST:
	case LVOLATILE:
		return;
	case LVOID:
		if(bits & BVOID)
			goto err;
		bits |= BVOID;
		break;
	case LCHAR:
		if(bits & BCHAR)
			goto err;
		bits |= BCHAR;
		break;
	case LSHORT:
		if(bits & BSHORT)
			goto err;
		bits |= BSHORT;
		break;
	case LINT:
		if(bits & BINT)
			goto err;
		bits |= BINT;
		break;
	case LLONG:
		if(bits & BLONG)
			goto err;
		bits |= BLONG;
		break;
	case LFLOAT:
		if(bits & BFLOAT)
			goto err;
		bits |= BFLOAT;
		break;
	case LDOUBLE:
		if(bits & BDOUBLE)
			goto err;
		bits |= BDOUBLE;
		break;
	case LSIGNED:
		if(bits & BSIGNED)
			goto err;
		bits |= BSIGNED;
		break;
	case LUNSIGNED:
		if(bits & BUNSIGNED)
			goto err;
		bits |= BUNSIGNED;
		break;
	case LSTRUCT:
	case LUNION:
		if(bits & BSTRUCT)
			goto err;
		bits |= BSTRUCT;
		break;
	case LENUM:
		if(bits & BENUM)
			goto err;
		bits |= BENUM;
		break;
	default:
		goto isclass;
	}
	return;
isclass:
	if(class != CNONE)
			errorf("multiple storage classes");
	switch(lex) {
	case LAUTO:
		class = CAUTO;
		break;
	case LEXTERN:
		class = CEXTERN;
		break;
	case LREGISTER:
		class = CREGISTER;
		break;
	case LSTATIC:
		class = CSTATIC;
		break;
	case LTYPEDEF:
		class = CTYPEDEF;
		break;
	}
err:
	errorf("bad type specifier");
}
