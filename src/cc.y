%{
#include "u.h"
%}
%union {
	int	ival;
	Sym	*sym;
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
     	slist
	'}'
     	;

slist:
|	slist stmt
	;

decl:	TDECL TID
|	expr
	;

expr:	TID '=' TINT
    	;
%%
void yyerror(char *s)
{
	errorposf("%s", s);
}
