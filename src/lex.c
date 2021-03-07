#include "u.h"
#include "y.tab.h"

#define NOPEEK -2

char peek;
struct {
	char b[BUFSIZ], *p;
	int c;		
} fin;

struct {
	char *s;
	int toktype;
} rsvd[] = {
	{"decl", TDECL,},
	{0},
};

void lexinit(void)
{
	int i, j;
	Sym *s;

	peek = NOPEEK;
	for(i = 0; rsvd[i].s; i++) {
		for(j = 0; rsvd[i].s[j]; j++)
			symb[j] = rsvd[i].s[j];
		s = lookup();
		s->toktype = rsvd[i].toktype;
	}
}

Sym *lookup(void)
{
	Sym *s;
	int c;
	unsigned h;
	char *cp;

	cp = symb;
	h = 5381;
	while((c = *cp++))
		h = ((h << 5) + h) + c;
	h %= NHASH;

	for(s = hash[h]; s; s = s->next)
		if(strcmp(s->name, symb) == 0)
			return s;
	s = emalloc(sizeof(Sym));
	s->name = estrdup(symb);
	s->next = hash[h];
	hash[h] = s;
	s->toktype = TID;
	return s;
}

char next(void)
{
	char c;

	if(--fin.c <= 0) {
		fin.c = fread(fin.b, 1, BUFSIZ, src.fp);
		if(fin.c == -1) 
			panic("error reading file %s", src.name);
		else if(fin.c == 0)
			return EOF;
		fin.p = fin.b;
	}
	c = *fin.p++;
	if(c == '\n') {
		src.line++;
		src.col = 1;
	} else if(isprint(c))
		src.col++;
	return c;
}

int yylex(void)
{
	int i;
	char c, *cp;

	if(peek != NOPEEK) {
		c = peek;
		peek = NOPEEK;
	} else
		c = next();

	for(; isspace(c); c = next());
	if(isdigit(c)) 
		goto LEXNUM;
	if(isalpha(c)) {
		cp = symb;
		goto LEXID;
	}
	switch(c) {
	case EOF:
		printf("<eof>\n");
		return 0;
	default:
		printf("<%c>\n", c);
		return c;
	}
LEXNUM:
	i = 0;
	while(isdigit(c)) {
		i = i*10 + c-'0';
		c = next();
	}
	peek = c;
	yylval.ival = i;
	printf("<int, %d>\n", yylval.ival);
	return TINT;
LEXID:
	while(isalnum(c) || c == '_') {
		*cp++ = c;
		c = next();
	}
	peek = c;
	*cp = 0;
	yylval.sym = lookup();
	printf("<id, %s>\n", symb);
	return yylval.sym->toktype;
}

void compile(void)
{
	lexinit();
	yyparse();
}
