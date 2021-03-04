%{
#include "u.h"
%}
%union {
	Node	*node;
	int	ival;
	char	*sval;
}
%token	<sval>	TID
%token	<ival>	TINT
%token	TDECL

%%
prog:	
|	prog stmt
	;

stmt:	decl ';'
|	block
	;

block:	'{'
     	{
		envpush();
	}
     	slist
	'}'
	{
		envpop();
	}
     	;

slist:
|	slist stmt
	;

decl:	TDECL TID
    	{
		if(install(ge->sym, $2))
			errorposf("redeclaration of %s", $2);
	}
|	expr
	;

expr:	TID '=' TINT
    	{
		Sym *s;

		if(!(s = lookup($1)))
			errorposf("%s undeclared\n", $1);

	}
    	;
%%
void yyerror(char *s)
{
	errorposf("%s", s);
}

void prenv(Env *p, int indent)
{
	int i, j;
	Sym *sp;

	for(i = 0; i < HASHSIZE; i++)
		for(sp = p->sym[i]; sp; sp = sp->next) {
			for(j = 0; j < indent; j++)
				printf("\t");
			printf("%s:%d\n", sp->name, sp->ival);
		}
}
