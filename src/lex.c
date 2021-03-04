#include "u.h"
#include "y.tab.h"

static char buf[1024];
int prevline, prevcol;

char next(void)
{
	char c;

	c = fgetc(src.fp);
	if(c == '\n') {
		prevline = src.line;
		src.line++;
	}
	else if(isprint(c)) {
		prevcol = src.col;
		src.col++;
	}
	return c;
}

void bkup(char c)
{
	ungetc(c, src.fp);
	src.line = prevline;
	src.col = prevcol;
}

int yylex(void)
{
	int v;
	char c, *p;

	p = buf;
	prevline = src.line;
	prevcol = src.col;
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

void compile(void)
{
	yyparse();
}
