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
	if(n->l != NULL)
		freenode(n->l);
	if(n->r != NULL)
		freenode(n->r);
	free(n);
}

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
