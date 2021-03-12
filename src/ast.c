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
