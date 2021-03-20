#include "u.h"
#include "y.tab.h"

Type *type(int ttype, Type *sub)
{
	Type *t;

	t = emalloc(sizeof(Type));
	t->ttype = ttype;
	t->sub = sub;
	t->sym = NULL;
	t->list = NULL;
	return t;
}

Type *decl(Node *n, Type *t, int c, int setsym)
{
	Node *n2;

	for(;;) {
		if(n == NULL)
			return t;
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
				freenode(n2);
				n->r = NULL;
			}
			n = n->l;
			break;
		case OIND:
			t = types[TPTR];
			n = n->l;
			break;
		case OFUNC:
			t = type(TFUNC, t);
			t->list = ptype(n->r);
			n = n->l;
			break;
		case OID:
			if(setsym)
				idecl(n->sym, t, c);
			lasttype = t;
			return t;
		}
	}
}

void idecl(Sym *s, Type *t, int c)
{
	if(s->type != NULL) {
		if(s->block == block)
			errorf("redeclaration of %s", s->name);
		else
			push(s, DOTHER);
	}
	s->type = t;
	s->class = c;
	s->block = block;
}

void ldecl(Sym *s, Node *n)
{
	if(s->label != NULL) {
		if(s->block == block)
			errorf("redeclaration of label %s", s->name);
		push(s, DLABEL);
	}
	s->label = n;
	s->block = block;
}

void pdecl(Node *n, Type *t)
{
	if(n == NULL)
		return;
	switch(n->op) {
	case OPARM:
		/* type already stored in t, so we just need to dig until we find id */	
		while(n->op != OID) {
			n = n->l;
			if(n == NULL)
				errorf("parameter name needed in function definition");
		}
		idecl(n->sym, t, CAUTO);
		return;
	case OLIST:
		pdecl(n->l, t);
		pdecl(n->r, t->list);
		return;
	case OFUNC:
		pdecl(n->r, t);
		return;
	case OELLIPSIS:
		/* ignore for now */
		pdecl(n->l, t);
		return;
	}
}

Type *ptype(Node *n)
{
	Type *t;

	if(n == NULL)
		return NULL;
	switch(n->op) {
	case OPARM:
		if(n->l == NULL)
			return n->type;
		t = decl(n->l, n->type, CNONE, 0);
		return t;
	case OLIST:
		t = ptype(n->l);
		t->list = ptype(n->r);
		return t;
	case OELLIPSIS:
		/* ignore for now  */
		return ptype(n->l);
	}
}

void push(Sym *s, int dtype)
{
	Dstk *d;

	d = emalloc(sizeof(Dstk));
	d->dtype = dtype;
	d->prev = declstk;
	if(dtype == DBLOCK)
		block++;
	else {
		d->sym = s;
		d->type = s->type;
		d->class = s->class;
		d->block = s->block;
	}
	declstk = d;
}

void pop(void)
{
	Dstk *d;
	Sym *s;

	for(;;)	{
		d = declstk;
		declstk = d->prev;
		switch(d->dtype) {
		case DBLOCK:
			block--;
			free(d);
			return;
		case DSUETAG:
			s = d->sym;
			s->suetag = d->type;
			s->block = d->block;
			free(d);
			break;
		case DLABEL:
			s = d->sym;
			s->label = d->label;
			s->block = d->block;
			free(d);
			break;
		case DOTHER:
			s = d->sym;
			s->type = d->type;
			s->class = d->class;
			s->block = d->block;
			free(d);
			break;
		}
	}
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
		b = CAUTO;
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

Type *btype(void)
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
