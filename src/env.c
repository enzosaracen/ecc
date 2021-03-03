#include "u.h"

static unsigned hash(char *s)
{
	int c;
	unsigned h;

	h = 5381;
	while(c = *s++)
		h = ((h << 5) + h) + c;
	return h % HASHSIZE;
}

static Sym *symlookup(Sym *sym[], char *s)
{
	Sym *p;

	for(p = sym[hash(s)]; p; p = p->next)
		if(strcmp(s, p->name) == 0)
			return p;
	return NULL;
}

Sym *lookup(char *s)
{
	Sym *r;
	Env *p;

	p = ge;
	for(; p; p = p->prev)
		if(r = symlookup(p->sym, s))
			return r;
	return NULL;
}

Sym *install(Sym *sym[], char *s, int i)
{
	Sym *p;
	unsigned h;

	if(!(p = symlookup(sym, s))) {
		p = emalloc(sizeof(Sym));
		h = hash(s);
		p->next = sym[h];
		sym[h] = p;
	}
	p->ival = i;
	return p;
}

void envpush(void)
{
	Env *t;

	t = ge;
	ge = emalloc(sizeof(Env));
	ge->prev = t;
}

void envpop(void)
{
	Env *t;

	t = ge->prev;
	free(ge);
	ge = t;
}
