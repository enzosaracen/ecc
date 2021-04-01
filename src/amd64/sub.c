#include "g.h"

void ins(int op, Adr *f, Adr *t)
{
	Prog *p;

	p = emalloc(sizeof(Prog));
	p->op = op;
	p->from = f;
	p->to = t;
	p->prev = prog;
	p->next = NULL;
	prins(p);
	prog = p;
}

void areg(Node *n)
{
	int i;

	for(i = AX; i <= R15; i <<= 1)
		if(reg & i) {
			n->reg = i;
			return;
		}
	gerrorf("out of regs");
}

void freg(Node *n)
{
	if(n == NULL)
		return;
	reg &= ~n->reg;
	n->reg = 0;
}

Adr *adr(void)
{
	Adr *a;

	a = emalloc(sizeof(Adr));
	return a;
}

Adr *ladr(char *l)
{
	Adr *a;

	a = adr();
	a->atype = ALAB;
	a->label = l;
	return a;
}

Adr *oadr(int o)
{
	Adr *a;

	a = adr();
	a->atype = AOFF;
	a->offset = o;
	return a;
}

Adr *radr(int r, int w)
{
	Adr *a;

	a = adr();
	a->atype = AREG;
	a->reg = r;
	a->regw = w;
	return a;
}

Adr *iadr(int i)
{
	Adr *a;

	a = adr();
	a->atype = AIMM;
	a->imm = i;
	return a;
}

Adr *nadr(Node *n)
{
	if(n == NULL)
		return NULL;
	if(n->reg != 0)
		return radr(n->reg, n->type->width);
	switch(n->op) {
	case OID:
		return oadr(n->offset);
	case OLABEL:
		return ladr(n->sym->label);
	case OCONST:
		return iadr(n->lval);
	default:
		gerrorf("what to do here in nadr...");
	}
}

void prins(Prog *p)
{
	if(p->op == ILABEL) {
		pradr(p->from);
		printf(":\n");
		return;
	}
	printf("\t%s\t", otab[p->op]);
	if(p->from != NULL)
		pradr(p->from);
	if(p->to != NULL) {
		printf(",\t");
		pradr(p->to);
	}
	printf("\n");
}

void pradr(Adr *a)
{
	switch(a->atype) {
	case ALAB:
		printf("%s", a->label);
		break;
	case AREG:
		printf("%s", rtab(a->reg, a->regw));
		break;
	case AOFF:
		printf("-%d(%s)", a->offset, rtab(SP, 8));
		break;
	case AIMM:
		printf("$%d", a->imm);
		break;
	default:
		gerrorf("bad adr type %d in pradr...", a->atype);
	}
}
