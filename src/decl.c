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
		printf("--------\n");
		prtree(n, 0);
		printf("--------\n");
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
				printf("lval: %ld\n", n2->lval);
				t->width = n2->lval * t->sub->width;
			}
			break;
		case OIND:
			t = type(TPTR, t);
			t->width = 8;
			n = n->l;
			break;
		case OID:
			n->sym->type = t;
			n->sym->class = class;
			goto end;
		}
	}
end:
	class = CNONE;
	bits = 0;
	return t;
}

void spec(int btype)
{
	switch(btype) {
	case BCONST:
	case BVOLATILE:
		return;
	case BAUTO:
		btype = CAUTO;
		goto isclass;
	case BEXTERN:
		btype = CEXTERN;
		goto isclass;
	case BREGISTER:
		btype = CREGISTER;
		goto isclass;
	case BSTATIC:
		btype = CSTATIC;
		goto isclass;
	case BTYPEDEF:
		btype = CTYPEDEF;
		goto isclass;
	default:
		if(bits & btype)
			errorf("duplicate types");
		bits |= btype;
		return;
	}
isclass:
	if(class != CNONE)
		errorf("multiple storage classes");
	class = btype;
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
	}
	errorf("illegal combination of types");
}
