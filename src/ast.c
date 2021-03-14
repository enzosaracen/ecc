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
