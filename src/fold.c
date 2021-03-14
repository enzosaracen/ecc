#include "u.h"

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
