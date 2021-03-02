%{
#include "u.h"
%}
%union {
	Sym	*sym;
	Node	*node;
	int	ival;
}
%token	<sym>	TID
%token	<ival>	TINT
%token	TDECL

%type	<node>	prog stmt slist decl expr

%right 	'='
%%
prog:	
	| prog stmt
	;
stmt:	  decl ';'
    	| block
	;
block:	  '{' slist '}'
     	;
slist:
     	| slist stmt
	;
decl:	  TDECL TID
    	| expr
	;
expr:	  TID '=' TINT
    	;
