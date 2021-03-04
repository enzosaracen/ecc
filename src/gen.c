#include "u.h"

void emit(char *fmt, ...)
{
	va_list l;

	va_start(l, fmt);
	vfprintf(fmt, l);
	va_end(l);
}

char *gen(Node *n)
{
	switch(n->type) {
	case NASSN:

	case NBINOP:
	case NID:
	case NINT:
	case NBLOCK:
	default:	errorposf("internal: unknown node type");
	}
}
