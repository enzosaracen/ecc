#include "u.h"

unsigned hash(char *s)
{
	int c;
	unsigned h;

	h = 5381;
	while((c = *s++))
		h = ((h << 5) + h) + c;
	return h % HASHSIZE;
}

Sym *slookup(Sym *sym[], char *s)
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

	p = envstack;
	for(; p; p = p->prev)
		if((r = slookup(p->sym, s)))
			return r;
	return NULL;
}

int install(Sym *sym[], char *s)
{
	Sym *p;
	unsigned h;

	if(!(p = slookup(sym, s))) {
		p = emalloc(sizeof(Sym));
		h = hash(s);
		p->name = s;
		p->next = sym[h];
		sym[h] = p;
		return 1;
	}
	return 0;
}

void envpush(void)
{
	Env *t;

	t = envstack;
	envstack = emalloc(sizeof(Env));
	envstack->prev = t;
}

void envpop(void)
{
	Env *t;

	t = envstack->prev;
	free(envstack);
	envstack = t;
}
