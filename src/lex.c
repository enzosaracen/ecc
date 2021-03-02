#include "u.h"
#include "y.tab.h"

yyerror(s)
{
	errorposf("%s", s);
}

static char buf[1024];

static char next(void)
{
	char c;

	c = fgetc(src.fp);
	if(c == '\n')
		src.line++;
	else if(isprint(c))
		src.col++;
	return c;
}

yylex()
{
	int v;
	char c, *p;

	p = buf;
	while(isspace(c = next()));
	if(isdigit(c)) 
		goto LEXNUM;
	if(isalph(c))
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
	yylval.ival = v;
	return TNUM;
LEXID:
	while(isalnum(c)) {
		*p++ = c;
		c = next();
	}
	*p = 0;
	if(strcmp(buf, "decl") == 0)
		return TDECL;
	yylval.sval = estrdup(buf);
	return TID;
}

void compile(void)
{

}
