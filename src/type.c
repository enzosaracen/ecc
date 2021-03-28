#include "u.h"
#include "y.tab.h"

int nlabel;

Type *type(int ttype, Type *sub)
{
	Type *t;

	t = emalloc(sizeof(Type));
	t->ttype = ttype;
	t->sub = sub;
	t->sym = NULL;
	t->list = NULL;
	t->width = 0;
	return t;
}

Node *new(int op, Node *l, Node *r)
{
	Node *n;
	int el, er;

	n = emalloc(sizeof(Node));
	n->op = op;
	n->l = l;
	n->r = r;
	n->sym = NULL;
	n->type = NULL;
	return n;
}

char *newlabel(void)
{
	int n;
	char *s;

	n = snprintf(NULL, 0, ".L%d", nlabel);
	if(n < 0)
		goto err;
	s = emalloc(n);
	if(snprintf(s, n, ".L%d", nlabel) < 0)
err:		errorf("snprintf error...");
	nlabel++;
	return s;
}

/*
 *	declarations
 */
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
	if(c != CTYPEDEF && c != CEXTERN && t->ttype != TFUNC && t->width == 0)
		errorf("storage size of %s not known", s->name);
	s->type = t;
	s->class = c;
	s->block = block;
}

void ldecl(Sym *s)
{
	if(s->label != NULL) {
		if(s->block == block)
			errorf("redeclaration of label %s", s->name);
		push(s, DLABEL);
	}
	s->label = newlabel();
	s->block = block;
}

void tdecl(Sym *s, Type *t)
{
	if(s->tag != NULL && t->width != 0) {
		if(s->block == block)
			errorf("redefinition of tag %s", s->name);
		push(s, DTAG);
	}
	s->tag = t;
	s->block = block;
}

void push(Sym *s, int dtype)
{
	Dstk *d;

	d = emalloc(sizeof(Dstk));
	d->dtype = dtype;
	d->prev = declstk;
	declstk = d;
	switch(d->dtype) {
	case DBLOCK:
		block++;
		return;
	case DTAG:
		d->type = s->tag;
		break;
	case DLABEL:
		d->label = s->label;
		break;
	case DOTHER:
		d->type = s->type;
		d->class = s->class;
		break;
	}
	d->block = s->block;
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
			return;
		case DTAG:
			s = d->sym;
			s->tag = d->type;
			s->block = d->block;
			break;
		case DLABEL:
			s = d->sym;
			s->label = d->label;
			s->block = d->block;
			break;
		case DOTHER:
			s = d->sym;
			s->type = d->type;
			s->class = d->class;
			s->block = d->block;
			break;
		}
	}
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
	case BDOUBLE|BLONG:
		lasttype = types[TDOUBLE];
		break;
	default:
		errorf("illegal combination of types");
	}
	bits = 0;
	return lasttype;
}

void cspec(int c)
{
	if(lastclass != CNONE)
		errorf("multiple storage classes");
	lastclass = c;
}

/*
 *	expressions
 */
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
				if(!sametype(t->list->sub, t2->list->sub))
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

int islval(Node *n)
{
	switch(n->op) {
	case ODOT:
	case OARROW:
	case OID:
	case OIND:
		return 1;
	}
	return 0;
}

void uaconv(Node *n)
{
	int lt, rt;

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

Node *ntype(Node *n)
{
	Type *lty;

	if(n->l != NULL)
		lty = n->l->type;
	switch(n->op) {
	case OCONST:
		n->type = types[TLONG];
		break;
	case OSTRING:
		n->type = type(TPTR, types[TCHAR]);
		break;
	case OADDAS:
	case OSUBAS:
	case OMULAS:
	case ODIVAS:
	case OMODAS:
	case OLSHAS:
	case ORSHAS:
	case OANDAS:
	case OXORAS:
	case OORAS:
		if(!islval(n->l))
			errorf("cannot assign to non lval");
		n->type = lty;
		break;
	case OPOSTINC:
	case OPREINC:
	case OPOSTDEC:
	case OPREDEC:
		if(!islval(n->l))
			errorf("cannot inc/dec non lval");
		n->type = lty;
		break;
	case OADDR:
		if(!islval(n->l))
			errorf("cannot get address of non lval");
		n->type = type(TPTR, lty);
		break;
	case OIND:
		if(lty->ttype != TPTR)
			errorf("cannot dereference non pointer");
		n->type = lty->sub;
		break;
	case ODOT:
		if(lty->ttype != TSTRUCT && lty->ttype != TUNION)
			errorf("expect structure");
		if(lty->sub->width == 0)
			errorf("incomplete type");
		n->type = getmemb(lty, n->sym);
		if(n->type == NULL)
			errorf("structure has no member %s", n->sym->name);
		break;
	case OARROW:
		if(lty->ttype != TPTR && lty->sub->ttype != TSTRUCT && lty->sub->ttype != TUNION)
			errorf("expect structure pointer");
		if(lty->sub->width == 0)
			errorf("incomplete type");
		n->type = getmemb(lty->sub, n->sym);
		if(n->type == NULL)
			errorf("structure pointer has no member %s", n->sym->name);
		break;
	}
	return n;
}

Node *fold(Node *n)
{
	Node *ln, *rn;
	long l, r;

	switch(n->op) {
	case OPOS:
	case ONEG:
	case ONOT:
	case OBNOT:
		goto unop;
	case OADD:
	case OSUB:
	case OMUL:
	case ODIV:
	case OMOD:
	case OLSH:
	case ORSH:
	case OAND:
	case OXOR:
	case OOR:
	case OOROR:
	case OANDAND:
	case OLT:
	case OGT:
	case OGE:
	case OLE:
	case OEQ:
	case ONE:
		goto binop;
	case OSIZEOF:
		if(n->type != NULL) {
			if(n->type->width == 0)
				errorf("cannot get size of incomplete type");
			l = n->type->width;
			goto ret;
		}
		return n;
	default:
		return n;
	}
binop:
	ln = fold(n->l);
	rn = fold(n->r);
	if(ln->op != OCONST || rn->op != OCONST)
		return n;
	l = ln->lval;
	r = rn->lval;
	switch(n->op) {
	case OADD:
		l += r;
		goto ret;
	case OSUB:
		l -= r;
		goto ret;
	case OMUL:
		l *= r;
		goto ret;
	case ODIV:
		l /= r;
		goto ret;
	case OMOD:
		l %= r;
		goto ret;
	case OLSH:
		l <<= r;
		goto ret;
	case ORSH:
		l >>= r;
		goto ret;
	case OAND:
		l &= r;
		goto ret;
	case OXOR:
		l ^= r;
		goto ret;
	case OOR:
		l |= r;
		goto ret;
	case OOROR:
		l = l || r;
		goto ret;
	case OANDAND:
		l = l && r;
		goto ret;
	case OLT:
		l = l < r;
		goto ret;
	case OGT:
		l = l > r;
		goto ret;
	case OGE:
		l = l >= r;
		goto ret;
	case OLE:
		l = l <= r;
		goto ret;
	case OEQ:
		l = l == r;
		goto ret;
	case ONE:
		l = l != r;
		goto ret;
	}
unop:
	ln = fold(n->l);
	if(ln->op != OCONST)
		return n;
	l = ln->lval;
	switch(n->op) {
	case OPOS:
		l = +l;
		goto ret;
	case ONEG:
		l = -l;
		goto ret;
	case ONOT:
		l = !l;
		goto ret;
	case OBNOT:
		l = ~l;
		goto ret;
	}
ret:
	n = new(OCONST, NULL, NULL);
	n->lval = l;
	return n;
}

/*
 *	debugging
 */
char *op2str(int op)
{
	switch(op) {
	case OADD:	return "+";
	case OADDAS:	return "+=";
	case OADDR:	return "&";
	case OAND:	return "&";
	case OANDAND:	return "&&";
	case OANDAS:	return "&=";
	case OARRAY:	return "[]";
	case OARROW:	return "->";
	case OAS:	return "=";
	case OBIT:	return ":";
	case OBNOT:	return "~";
	case OBREAK:	return "break";
	case OCAST:	return "(cast)";
	case OCASE:	return "case";
	case OCOND:	return "?:";
	case OCONST:	return "num: ";
	case OCONTINUE:	return "continue";
	case ODIV:	return "/";
	case ODIVAS:	return "/=";
	case ODOT:	return ".";
	case ODOWHILE:	return "do while";
	case OELLIPSIS:	return "...";
	case OEQ:	return "==";
	case OFOR:	return "for";
	case OFUNC:	return "function";
	case OGE:	return ">=";
	case OGOTO:	return "goto";
	case OGT:	return ">";
	case OID:	return "id: ";
	case OIF:	return "if";
	case OIND:	return "*";
	case OLABEL:	return "label";
	case OLE:	return "<=";
	case OLIST:	return ",";
	case OLSH:	return "<<";
	case OLSHAS:	return "<<=";
	case OLT:	return "<";
	case OMOD:	return "%";
	case OMODAS:	return "%=";
	case OMUL:	return "*";
	case OMULAS:	return "*=";
	case ONE:	return "!=";
	case ONEG:	return "-";
	case ONOT:	return "!";
	case OOR:	return "|";
	case OORAS:	return "|=";
	case OOROR:	return "||";
	case OPOS:	return "+";
	case OPOSTDEC:	return "(post)--";
	case OPOSTINC:	return "(post)++";
	case OPREDEC:	return "--(pre)";
	case OPREINC:	return "++(pre)";
	case ORETURN:	return "return";
	case ORSH:	return ">>";
	case ORSHAS:	return ">>=";
	case OSIZEOF:	return "sizeof";
	case OSTRING:	return "string";
	case OSUB:	return "-";
	case OSUBAS:	return "-=";
	case OSWITCH:	return "switch";
	case OWHILE:	return "while";
	case OXOR:	return "^";
	case OXORAS:	return "^=";
	default:	panic("unrecognized op type %d while printing ast", op);
	}
}

void prtree(Node *n, int indent)
{
	int i;

	if(n == NULL)
		return;
	for(i = 0; i < indent; i++)
		printf("  ");
	printf("%s", op2str(n->op));
	if(n->op == OCONST)
		printf("%ld", n->lval);
	else if(n->op == OID)
		printf("%s", n->sym->name);
	printf("\n");
	prtree(n->l, indent+1);
	prtree(n->r, indent+1);
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
	case TULONG:	return "ulong";
	case TFLOAT:	return "float";
	case TDOUBLE:	return "double";
	case TPTR:	return "ptr";
	case TARRAY:	return "array";
	case TENUM:	return "enum";
	case TFUNC:	return "function";
	case TSTRUCT:	return "struct";
	case TUNION:	return "union";
	case TWRAP:	return "member/param";
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
		printf("width: %d ", t->width);
		printf("%s\n", type2str(t->ttype));
		while(t->list != NULL) {
			prtype(t->list->sub, indent+1);
			t = t->list;
		}
		break;
	case TFUNC:
		printf("%s\n", type2str(t->ttype));
		for(i = 0; i < indent+1; i++)
			printf("  ");
		printf("ret:\n");
		prtype(t->sub, indent+2);
		for(i = 0; i < indent+1; i++)
			printf("  ");
		printf("params:\n");
		while(t->list != NULL) {
			prtype(t->list->sub, indent+2);
			t = t->list;
		}
		break;
	default:
		printf("%s\n", type2str(t->ttype));
		prtype(t->sub, indent+1);
		prtype(t->list, indent);
		break;
	}
}
