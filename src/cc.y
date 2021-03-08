%{
#include "u.h"
%}
%union {
	int	ival;
	Sym	*sym;
}
%token	<sym>	TID
%token	<ival>	TICONST
%token	TCHAR TELSE TFLOAT TFOR TIF TINT TRETURN TVOID TWHILE

%%
prog:	
|	prog stmt
	;

stmt:	decl ';'
|	block
	;

block:	'{' slist '}'
     	;

slist:
|	slist stmt
	;

decl:	type declarator
	;

declarator:	direct
|		ptr direct
		;

ptr:	'*'
|	'*' ptr
	;

direct:	TID
|	'(' declarator ')'
|	direct '[' TICONST ']'
	;

type:	TINT
|	TCHAR
|	TVOID
|	TFLOAT
    	;
%%
void yyerror(char *s)
{
	errorposf("%s", s);
}
