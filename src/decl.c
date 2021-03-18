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

Type *decl(Node *n, Type *t, int c)
{
	Node *n2;
	Sym *s;

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
			t = types[TPTR];
			n = n->l;
			break;
		case OFUNC:
			t = type(TFUNC, t);
			t->parms = parms(n->r);
			n = n->l;
			break;
		case OID:
			s = n->sym;
			s->type = t;
			s->class = c;
			goto end;
		}
	}
end:
	return t;
}

Type *parms(Node *n)
{
}

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
	if(lastclass != CNONE)
		errorf("multiple storage classes");
	lastclass = b;
}

Type *basetype(void)
{
	switch(bits) {
	case BVOID:
		return types[TVOID];
	case BCHAR:
	case BCHAR|BSIGNED:
		return types[TCHAR];
	case BCHAR|BUNSIGNED:
		return types[TUCHAR];
	case BSHORT:
	case BSHORT|BINT:
	case BSHORT|BSIGNED:
	case BSHORT|BINT|BSIGNED:
		return types[TSHORT];
	case BSHORT|BUNSIGNED:
	case BSHORT|BUNSIGNED|BINT:
		return types[TUSHORT];
	case 0:
	case BINT:
	case BSIGNED:
	case BINT|BSIGNED:
		return types[TINT];
	case BUNSIGNED:
	case BINT|BUNSIGNED:
		return types[TUINT];
	case BLONG:
	case BLONG|BINT:
	case BLONG|BSIGNED:
	case BLONG|BINT|BSIGNED:
		return types[TLONG];
	case BFLOAT:
		return types[TFLOAT];
	case BDOUBLE:
		return types[TDOUBLE];
	case BDOUBLE|BLONG:
		return types[TLDOUBLE];
	default:
		errorf("illegal combination of types");
	}
}
