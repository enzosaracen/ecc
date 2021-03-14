#include "u.h"
#include "y.tab.h"

Type *type(int ttype, Type *sub)
{
	Type *t;

	t = emalloc(sizeof(Type));
	t->ttype = ttype;
	t->sub = sub;
	return t;
}

Type *decl(Node *n)
{
	Type *t;
	Node *n2;

	t = type(getspec(), NULL);
	for(;;) {
		switch(n->op) {
		case OARRAY:
			t = type(TARRAY, t);
			t->width = 0;
			if(n->r != NULL) {
				n2 = fold(n->r);
				if(n2->op != OCONST)
					errorf("array size must be constant");
				else if(n2->lval < 0)
					errorf("array size must be positive");
				t->width = n2->lval * t->sub->width;
			}
			break;
		case OIND:
			t = type(TPTR, t);
			n = n->l;
			break;
		}
	}
	class = CNONE;
	bits = 0;
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
	errorf("duplicate types");
}

int getspec(void)
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
	errorf("illegal combination of types");
}
