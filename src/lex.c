#include "u.h"
#include "y.tab.h"

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

int yylex(void)
{
	int v;
	char c, *p;

	p = buf;
	while(isspace(c = next()));
	if(isdigit(c)) 
		goto LEXNUM;
	if(isalpha(c))
		goto LEXID;
	switch(c) {
	case EOF:
		printf("eof\n");
		return 0;
	default:
		printf("%c\n", c);
		return c;
	}
LEXNUM:
	v = 0;
	while(isdigit(c)) {
		v = v*10 + c-'0';
		c = next();
	}
	yylval.ival = v;
	printf("num\n");
	return TINT;
LEXID:
	while(isalnum(c)) {
		*p++ = c;
		c = next();
	}
	*p = 0;
	if(strcmp(buf, "decl") == 0)
		return TDECL;
	yylval.sval = estrdup(buf);
	printf("id\n");
	return TID;
}

void compile(void)
{
	yyparse();
}
