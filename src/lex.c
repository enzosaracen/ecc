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
	int lexical;
} rsvd[] = {
	{"sizeof",	LSIZEOF},
	{"void",	LVOID},
	{"char",	LCHAR},
	{"short",	LSHORT},
	{"int",		LINT},
	{"long",	LLONG},
	{"float",	LFLOAT},
	{"double",	LDOUBLE},
	{"signed",	LSIGNED},
	{"unsigned",	LUNSIGNED},
	{"union",	LUNION},
	{"struct",	LSTRUCT},
	{"enum",	LENUM},
	{"if",		LIF},
	{"else",	LELSE},
	{"switch",	LSWITCH},
	{"case",	LCASE},
	{"default",	LDEFAULT},
	{"while",	LWHILE},
	{"do",		LDO},
	{"for",		LFOR},
	{"goto",	LGOTO},
	{"continue",	LCONTINUE},
	{"break",	LBREAK},
	{"return",	LRETURN},
	{"auto",	LAUTO},
	{"register",	LREGISTER},
	{"extern",	LEXTERN},
	{"static",	LSTATIC},
	{"typedef",	LTYPEDEF},
	{"const",	LCONST},
	{"volatile",	LVOLATILE},
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
		symb[j] = 0;
		s = lookup();
		s->lexical = rsvd[i].lexical;
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
	s->lexical = LID;
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
	int c;
	char c2, *cp;

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
		return 0;
	case '+':
		c2 = next();
		if(c2 == '=')
			return LPE;
		else if(c2 == '+')
			return LPP;
		break;
	case '-':
		c2 = next();
		if(c2 == '=')
			return LME;
		else if(c2 == '-')
			return LMM;
		else if(c2 == '>')
			return LARROW;
		break;
	case '*':
		c2 = next();
		if(c2 == '=')
			return LMLE;
		break;
	case '/':
		c2 = next();
		if(c2 == '=')
			return LDVE;
		break;
	case '%':
		c2 = next();
		if(c2 == '=')
			return LMDE;
		break;
	case '>':
		c2 = next();
		if(c2 == '=')
			return LGE;
		else if(c2 == '>') {
			c2 = next();
			if(c2 == '=')
				return LRSHE;
			c = LRSH;
		}
		break;
	case '<':
		c2 = next();
		if(c2 == '=')
			return LLE;
		else if(c2 == '<') {
			c2 = next();
			if(c2 == '=')
				return LLSHE;
			c = LLSH;
		}
		break;
	case '!':
		c2 = next();
		if(c2 == '=')
			return LNE;
		break;
	case '=':
		c2 = next();
		if(c2 == '=')
			return LEQ;
		break;
	case '&':
		c2 = next();
		if(c2 == '&')
			return LANDAND;
		else if(c2 == '=')
			return LANDE;
		break;
	case '|':
		c2 = next();
		if(c2 == '|')
			return LOROR;
		else if(c2 == '=')
			return LORE;
		break;
	case '^':
		c2 = next();
		if(c2 == '=')
			return LXORE;
		break;
	case '.':
		c2 = next();
		if(c2 == '.') {
			if(next() != '.')
				errorposf("expected '...'");
			return LELLIPSES;
		}
		break;
	default:
		return c;
	peek = c2;
	return c; 
	}
LEXNUM:
	i = 0;
	while(isdigit(c)) {
		i = i*10 + c-'0';
		c = next();
	}
	peek = c;
	return LNUM;
LEXID:
	while(isalnum(c) || c == '_') {
		*cp++ = c;
		c = next();
	}
	peek = c;
	*cp = 0;
	yylval.sym = lookup();
	return yylval.sym->lexical;
}

void compile(void)
{
	lexinit();
	yyparse();
}
