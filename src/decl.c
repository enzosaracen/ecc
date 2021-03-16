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

/*
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
			n = n->l;
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
	return t;
}*/

void spec(int b)
{
	switch(b) {
	case 0:
		return;
	case BAUTO:
		b = CAUTO;
		goto class;
	case BEXTERN:
		b = CEXTERN;
		goto class;
	case BREGISTER:
		b = CREGISTER;
		goto class;
	case BSTATIC:
		b = CSTATIC;
		goto class;
	case BTYPEDEF:
		b = CTYPEDEF;
		goto class;
	default:
		if(bits & b)
			errorf("duplicate types");
		bits |= b;
		return;
	}
class:
	if(class != CNONE)
		errorf("multiple storage classes");
	class = b;
}

void setspec(void)
{
	t = emalloc(sizeof(Type));
	switch(bits) {
	case BVOID:
		t->ttype = TVOID;
		goto end;
	case BCHAR:
	case BCHAR|BSIGNED:
		t->ttype = TCHAR;
		goto end;
	case BCHAR|BUNSIGNED:
		t->ttype = TUCHAR;
		goto end;
	case BSHORT:
	case BSHORT|BINT:
	case BSHORT|BSIGNED:
	case BSHORT|BINT|BSIGNED:
		t->ttype = TSHORT;
		goto end;
	case BSHORT|BUNSIGNED:
	case BSHORT|BUNSIGNED|BINT:
		t->ttype = TUSHORT;
		goto end;
	case 0:
	case BINT:
	case BSIGNED:
	case BINT|BSIGNED:
		t->ttype = TINT;
		goto end;
	case BUNSIGNED:
	case BINT|BUNSIGNED:
		t->ttype = TUINT;
		goto end;
	case BLONG:
	case BLONG|BINT:
	case BLONG|BSIGNED:
	case BLONG|BINT|BSIGNED:
		t->ttype = TLONG;
		goto end;
	case BFLOAT:
		t->ttype = TFLOAT;
		goto end;
	case BDOUBLE:
		t->ttype = TDOUBLE;
		goto end;
	case BDOUBLE|BLONG:
		t->ttype = TLDOUBLE;
		goto end;
	default:
		errorf("illegal combination of types");
	}
end:
	class = bits = 0;
	t->width = widths[t->ttype];
}
