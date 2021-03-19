#include "u.h"

Node *new(int op, Node *l, Node *r)
{
	Node *n;

	n = emalloc(sizeof(Node));
	n->op = op;
	n->l = l;
	n->r = r;
	return n;
}

void freenode(Node *n)
{
	if(n == NULL)
		return;
	freenode(n->l);
	freenode(n->r);
	free(n);
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
	freenode(n);
	n = new(OCONST, NULL, NULL);
	n->lval = l;
	return n;
}

char *op2str(int op)
{
	/* need to update this occasionally */
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
	case OPARM:	return "param";
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
