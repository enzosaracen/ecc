#include "u.h"
#include "y.tab.h"

int prevline, prevcol;
char peek;
struct {
	char b[BUFSIZ], *p;
	int c;		
} fin;

void lexinit(void)
{
	prevline = src.line;
	prevcol = src.col;
}

void fill(void)
{
	char *t;

	if((fin.c = read(src.fp, fin.b, BUFSIZ)) == -1)
		panic("error reading file %s", src.name);
	fin.p = fin.b;
}

char next(void)
{
	char c;

	if(--fin.c < 0)
		fill();
	c = *fin.p--;
	if(c == '\n') {
		prevline = src.line;
		prevcol = src.col;
		src.line++;
		src.col = 1;
	} else if(isprint(c)) {
		prevcol = src.col;
		src.col++;
	}
	return c;
}

void bkup(char c)
{
	src.line = prevline;
	src.col = prevcol;
}

int yylex(void)
{
	int v;
	char c, *p;

	while(isspace(c = next()));
	if(isdigit(c)) 
		goto LEXNUM;
	if(isalpha(c))
		goto LEXID;
	switch(c) {
	case EOF:
		return 0;
	default:
		return c;
	}
LEXNUM:
	v = 0;
	while(isdigit(c)) {
		v = v*10 + c-'0';
		c = next();
	}
	bkup(c);
	yylval.ival = v;
	return TINT;
LEXID:
	while(isalnum(c)) {
		*p++ = c;
		c = next();
	}
	bkup(c);
	*p = 0;
	if(strcmp(buf, "decl") == 0)
		return TDECL;
	yylval.sval = estrdup(buf);
	return TID;
}
