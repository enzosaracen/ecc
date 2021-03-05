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
		if(!install(envstack->sym, $2))
			errorposf("redeclaration of '%s'", $2);
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
