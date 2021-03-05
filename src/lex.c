#include "u.h"
#include "y.tab.h"

#define MAXID 32
#define NOPEEK -2

int prevline, prevcol;
char peek;
struct {
	char b[BUFSIZ], *p;
	int c;		
} fin;

void lexinit(void)
{
	peek = NOPEEK;
	prevline = prevcol = 1;
}

void fill(void)
{
	if((fin.c = fread(fin.b, 1, BUFSIZ, src.fp)) == -1)
		panic("error reading file %s", src.name);
	fin.p = fin.b;
}

char next(void)
{
	char c;

	if(--fin.c <= 0) {
		fill();
		if(fin.c == 0)
			return EOF;
	}
	c = *fin.p++;
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

int yylex(void)
{
	int i;
	char c, buf[MAXID];

	if(peek != NOPEEK) {
		c = peek;
		peek = NOPEEK;
	} else
		c = next();

	for(; isspace(c); c = next());
	if(isdigit(c)) 
		goto LEXNUM;
	if(isalpha(c))
		goto LEXID;
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
	for(i = 0; isalnum(c); i++) {
		if(i >= MAXID-2)
			yyerror("identifier too long");
		buf[i] = c;
		c = next();
	}
	peek = c;
	buf[i] = 0;
	if(strcmp(buf, "decl") == 0) {
		printf("<decl>\n");
		return TDECL;
	}
	yylval.sval = estrdup(buf);
	printf("<id, '%s'>\n", yylval.sval);
	return TID;
}

void compile(void)
{
	lexinit();
	yyparse();
}
