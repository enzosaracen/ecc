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

%type	<node>	prog stmt block slist decl expr

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
expr:	  TID '=' TINT	{ elookup() }
    	;
