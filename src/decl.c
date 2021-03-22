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
	t->width = 0;
	t->offset = 0;
	return t;
}

int sametype(Type *t, Type *t2)
{
	if(t == t2)
		return 1;
	if(t->ttype == t2->ttype) {
		switch(t->ttype) {
		case TFUNC:
			if(!sametype(t->sub, t2->sub))
				return 0;
			/* fallthrough */
		case TUNION:
		case TSTRUCT:
			while(t->list != NULL && t2->list != NULL) {
				if(!sametype(t->list, t2->list))
					return 0;
				t = t->list;
				t2 = t2->list;
			}
			return 1;
		case TPTR:
		case TARRAY:
			return sametype(t->sub, t2->sub);
		default:
			return 1;
		}
	}
	return 0;
}

Type *decor(Node *n, Type *t, int c, int st, Sym **ms)
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
			t = type(TPTR, t);
			t->width = types[TPTR]->width;
			n = n->l;
			break;
		case OFUNC:
			t = type(TFUNC, t);
			t->list = ptype(n->r);
			n = n->l;
			break;
		case OBIT:
			/* ignore for now */
			n = n->l;
			break;
		case OID:
			switch(st) {
			case SIDECL:
				idecl(n->sym, t, c);
				break;
			case SMEMB:
				if(n->sym->nsue == nsue)
					errorf("duplicate member %s", n->sym->name);
				n->sym->nsue = nsue;
				*ms = n->sym;
				break;
			}
			lasttype = t;
			return t;
		}
	}
}

void idecl(Sym *s, Type *t, int c)
{
	if(block == 0) {
		switch(c) {
		case CNONE:
			c = CGLOBAL;
			break;
		case CAUTO:
			errorf("auto declaration of %s not allowed at global scope", s->name);
			break;
		}
		if(s->type != NULL && s->class != c && !sametype(s->type, t))
			errorf("conflicting types for %s", s->name);
	} else {
		if(s->block == block)
			errorf("auto redeclaration of %s", s->name);
		push(s, DOTHER);
	}
	if(c != CTYPEDEF && c != CEXTERN && incomp(t))
		errorf("storage size of %s not known", s->name);
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

void tdecl(Sym *s, Type *t)
{
	if(s->tag != NULL && !incomp(t)) {
		if(s->block == block)
			errorf("redefinition of tag %s", s->name);
		push(s, DTAG);
	}
	s->tag = t;
	s->block = block;
}

Type *sdecl(Node *n, Type *st, Type *t)
{
	Type *b;

	if(n == NULL)
		return NULL;
	switch(n->op) {
	case OMEMB:
		b = n->type;
		while(n != NULL) {
			n = n->l;
			t->list = type(TMEMB, NULL);
			t = t->list;
			if(n->op == OLIST)
				t->sub = decor(n->r, b, CNONE, SMEMB, &t->sym);
			else {
				t->sub = decor(n, b, CNONE, SMEMB, &t->sym);
				break;
			}
		}
		st->width += t->sub->width;
		return t;
	case OLIST:
		t = sdecl(n->r, t, st);
		sdecl(n->l, t, st);
		return NULL;
	}
}

Type *getmemb(Type *t, Sym *s)
{
	while(t->list != NULL) {
		if(t->list->sym == s)
			return t->list;
		t = t->list;
	}
	return NULL;
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
			if(n == NULL) {
				if(t->ttype == TVOID)
					return;
				errorf("parameter name needed in function definition");
			}
		}
		if(incomp(t))
			errorf("parameter %s has incomplete type", n->sym->name);
		idecl(n->sym, t, CAUTO);
		return;
	case OLIST:
		pdecl(n->l, t);
		pdecl(n->r, t->list);
		return;
	case OFUNC:
		pdecl(n->r, t->list);
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
		if(n->l == NULL) {
			if(n->type == TVOID)
				return NULL;
			return n->type;
		}
		t = decor(n->l, n->type, CNONE, 0, NULL);
		switch(t->ttype) {
		case TFUNC:
			t = type(TPTR, t);
			break;
		case TARRAY:
			t = type(TPTR, t->sub);
			break;
		}
		return t;
	case OLIST:
		t = ptype(n->l);
		if(t == NULL)
			goto err;
		t->list = ptype(n->r);
		if(t->list == NULL)
err:			errorf("void must be the only parameter");
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
		case DTAG:
			s = d->sym;
			s->tag = d->type;
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
		lasttype = types[TVOID];
		break;
	case BCHAR:
	case BCHAR|BSIGNED:
		lasttype = types[TCHAR];
		break;
	case BCHAR|BUNSIGNED:
		lasttype = types[TUCHAR];
		break;
	case BSHORT:
	case BSHORT|BINT:
	case BSHORT|BSIGNED:
	case BSHORT|BINT|BSIGNED:
		lasttype = types[TSHORT];
		break;
	case BSHORT|BUNSIGNED:
	case BSHORT|BUNSIGNED|BINT:
		lasttype = types[TUSHORT];
		break;
	case 0:
	case BINT:
	case BSIGNED:
	case BINT|BSIGNED:
		lasttype = types[TINT];
		break;
	case BUNSIGNED:
	case BINT|BUNSIGNED:
		lasttype = types[TUINT];
		break;
	case BLONG:
	case BLONG|BINT:
	case BLONG|BSIGNED:
	case BLONG|BINT|BSIGNED:
		lasttype = types[TLONG];
		break;
	case BFLOAT:
		lasttype = types[TFLOAT];
		break;
	case BDOUBLE:
		lasttype = types[TDOUBLE];
		break;
	case BDOUBLE|BLONG:
		lasttype = types[TLDOUBLE];
		break;
	default:
		errorf("illegal combination of types");
	}
	bits = 0;
	return lasttype;
}

int incomp(Type *t)
{
	return t->ttype == TVOID ||
		(t->ttype == TSTRUCT || t->ttype == TENUM || t->ttype == TUNION) && t->list == NULL;
}

char *type2str(int ttype)
{
	switch(ttype) {
	case TVOID:	return "void";
	case TCHAR:	return "char";
	case TUCHAR:	return "uchar";
	case TSHORT:	return "short";
	case TUSHORT:	return "ushort";
	case TINT:	return "int";
	case TUINT:	return "uint";
	case TLONG:	return "long";
	case TLLONG:	return "llong";
	case TULLONG:	return "ullong";
	case TFLOAT:	return "float";
	case TDOUBLE:	return "double";
	case TLDOUBLE:	return "ldouble";
	case TPTR:	return "ptr";
	case TARRAY:	return "array";
	case TENUM:	return "enum";
	case TFUNC:	return "func";
	case TSTRUCT:	return "struct";
	case TUNION:	return "union";
	case TMEMB:	return "member";
	default:
		errorf("unrecognized ttype %d when printing type", ttype);
	}
}

void prtype(Type *t, int indent)
{
	int i;

	if(t == NULL)
		return;
	for(i = 0; i < indent; i++)
		printf("  ");
	switch(t->ttype) {
	case TSTRUCT:
	case TUNION:
		printf("width: %d\t", t->width);
		/* fallthrough */
	case TFUNC:
		printf("%s\n", type2str(t->ttype));
		prtype(t->sub, indent);
		prtype(t->list, indent+1);
		break;
	case TMEMB:
		printf("%s: ", type2str(t->ttype));
		prtype(t->sub, indent);
		prtype(t->list, indent);
		break;
	default:
		prtype(t->sub, indent+1);
		prtype(t->list, indent);
		break;
	}
}
