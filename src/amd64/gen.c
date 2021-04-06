#include "g.h"

int	reg[R15];
Prog	*prog;

void load(Node *n)
{
	int o;
	Adr *a;

	switch(n->type->width) {
	case 1:
		o = IMOVB;
		break;
	case 2:
		o = IMOVW;
		break;
	case 4:
		o = IMOVL;
		break;
	case 8:
		o = IMOVQ;
		break;
	}
	a = nadr(n);
	areg(n);
	ins(o, a, nadr(n));
}

void bool(Node *n)
{
	Node *e;

	e = n->l;
}

void gen(Node *n)
{
	int o;
	Adr *a, *a1, *t, *f, *e;
	Node *l, *r;

	if(n == NULL)
		return;
	l = n->l;
	r = n->r;
	switch(n->op) {
	case OID:
	case OCONST:
		load(n);
		break;
	case OLIST:
		gen(l);
		gen(r);
		break;
	case OIF:
	case OCOND:
		bool(n);
		break;
	case OOROR:
	case OANDAND:
		t = ladr(newlabel());
		f = ladr(newlabel());
		e = ladr(newlabel());
		gen(l);
		a = nadr(l);
		ins(ITEST, a, a);
		freg(l);
		if(n->op == OANDAND)
			ins(IJZ, f, NULL);
		else
			ins(IJNZ, t, NULL);
		gen(r);
		a = nadr(r);
		ins(ITEST, a, a);
		freg(r);
		if(n->op == OANDAND) {
			ins(IJZ, f, NULL);
			ins(IJMP, t, NULL);
		} else {
			ins(IJNZ, t, NULL);
			ins(IJMP, f, NULL);
		}
		areg(n);
		a = nadr(n);
		ins(ILABEL, t, NULL);
		/* add mov subroutine to branch out to dif widths n stuff */
		ins(IMOVQ, iadr(1), a);
		ins(IJMP, e, NULL);
		ins(ILABEL, f, NULL);
		ins(IXOR, a, a);
		ins(ILABEL, e, NULL);
		break;
	case OADD:
		gen(l);
		a = nadr(l);
		gen(r);
		a1 = nadr(r);
		if(a->regw != a1->regw)
			gerrorf("type inconsistency in gen, fix frontend");
		switch(a->regw) {
		case 1:
			o = IADDB;
			break;
		case 2:
			o = IADDW;
			break;
		case 4:
			o = IADDL;
			break;
		case 8:
			o = IADDQ;
			break;
		default:
			gerrorf("bad type for add in gen, fix frontedn");
		}
		ins(o, a, a1);
		n->reg = a->reg;
		break;
	default:
		gerrorf("unimplemented op %s during code gen", op2str(n->op));
	}
}
