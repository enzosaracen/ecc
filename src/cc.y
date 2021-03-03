%{
#include "u.h"

static Sym *s;
%}
%union {
	Node	*node;
	int	ival;
	char	*sval;
}
%token	<sval>	TID
%token	<ival>	TINT
%token	TDECL

%right 	'='
%%
prog:	
|	prog stmt
	;

stmt:	decl ';'
|	block
	;

block:	'{'	{ envpush(); }
     	slist
	'}'	{ envpop(); }
     	;

slist:
|	slist stmt
	;

decl:	TDECL TID	{ install(ge->sym, $2, 0); }
|	expr
	;

expr:	TID '=' TINT
    	{
		s = lookup($1);
		if(!s)
			errorposf("%s undeclared\n", $1);
		s->ival = $3;
	}
    	;
%%
void yyerror(char *s)
{
	errorposf("%s", s);
}
