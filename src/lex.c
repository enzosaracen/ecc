#include "u.h"
#include "y.tab.h"

#define NHASH 1024
#define NLBUF 1024
#define NOPEEK -2

char peek;
char lbuf[NLBUF];
Sym *hash[NHASH];
struct {
	char b[BUFSIZ], *p;
	int c;		
} fin;

struct {
	char *s;
	int lex;
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
			lbuf[j] = rsvd[i].s[j];
		lbuf[j] = 0;
		s = lookup();
		s->lex = rsvd[i].lex;
	}
}

Sym *lookup(void)
{
	Sym *s;
	int c;
	unsigned h;
	char *cp;

	cp = lbuf;
	h = 5381;
	while((c = *cp++))
		h = ((h << 5) + h) + c;
	h %= NHASH;

	c = lbuf[0];
	for(s = hash[h]; s; s = s->next) {
		if(s->name[0] != c)
			continue;
		if(strcmp(s->name, lbuf) == 0)
			return s;
	}
	s = emalloc(sizeof(Sym));
	s->name = estrdup(lbuf);
	lastname = s->name;
	s->next = hash[h];
	hash[h] = s;
	s->type = NULL;
	s->label = NULL;
	s->tag = NULL;
	s->class = CNONE;
	s->offset = 0;
	s->block = 0;
	s->suenum = 0;
	s->lex = LID;
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

void putbuf(char *cp, char c)
{
	if(cp-lbuf >= NLBUF-1)
		errorf("token too large");
	*cp = c;
}

int yylex(void)
{
	long v;
	int c;
	char c2, *cp;
start:
	if(peek != NOPEEK) {
		c = peek;
		peek = NOPEEK;
	} else
		c = next();

	for(; isspace(c); c = next());
	if(isdigit(c)) 
		goto lexnum;
	if(isalpha(c) || c == '_') {
		cp = lbuf;
		goto lexid;
	}
	switch(c) {
	case EOF:
		return 0;
	case '+':
		c2 = next();
		if(c2 == '=')
			return LADDAS;
		else if(c2 == '+')
			return LINC;
		break;
	case '-':
		c2 = next();
		if(c2 == '=')
			return LSUBAS;
		else if(c2 == '-')
			return LDEC;
		else if(c2 == '>')
			return LARROW;
		break;
	case '*':
		c2 = next();
		if(c2 == '=')
			return LMULAS;
		break;
	case '/':
		c2 = next();
		if(c2 == '=')
			return LDIVAS;
		else if(c2 == '*') {
			for(;;) {
				c = next();
				if(c == '*') {
					c = next();
					if(c == '/')
						goto start;
				}
				if(c == EOF)
					errorf("unterminated comment");
			}
		} else if(c2 == '/') {
			while((c = next()) != '\n')
				if(c == EOF)
					errorf("unterminated comment");
			goto start;
		}
		break;
	case '%':
		c2 = next();
		if(c2 == '=')
			return LMODAS;
		break;
	case '>':
		c2 = next();
		if(c2 == '=')
			return LGE;
		else if(c2 == '>') {
			c2 = next();
			if(c2 == '=')
				return LRSHAS;
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
				return LLSHAS;
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
			return LANDAS;
		break;
	case '|':
		c2 = next();
		if(c2 == '|')
			return LOROR;
		else if(c2 == '=')
			return LORAS;
		break;
	case '^':
		c2 = next();
		if(c2 == '=')
			return LXORAS;
		break;
	case '"':
		cp = lbuf;
		while((c2 = next()) != '"') {
			if(c2 == EOF || c2 == '\n')
				errorf("unterminated string");
			putbuf(cp++, c2);
		}
		yylval.sval = estrdup(lbuf);
		return LSTRING;
		break;
	default:
		return c;
	}
	peek = c2;
	return c; 
lexnum:
	v = 0;
	while(isdigit(c)) {
		v = v*10 + c-'0';
		c = next();
	}
	peek = c;
	yylval.lval = v;
	return LNUM;
lexid:
	while(isalnum(c) || c == '_')  {
		putbuf(cp++, c);
		c = next();
	}
	peek = c;
	*cp = 0;
	yylval.sym = lookup();
	if(yylval.sym->class == CTYPEDEF)
		yylval.sym->lex = LTYPE;
	return yylval.sym->lex;
}

void compile(void)
{
	lexinit();
	yyparse();
}
