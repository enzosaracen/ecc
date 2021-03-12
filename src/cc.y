%{
#include "u.h"
%}
%union {
	 Sym *sym;
}

%type	prog xdecl fndef decl dlist dspec init ilist sue subody sudecl sudecor sudecorlist sqlist enumbody enumlist decor ptr ddecor parms pdecl
%type	idlist adecor dadecor tname stmt label block slist sel iter jmp exprlist expr cast uexpr pexpr oexpr oelist qlist qual scspec tspec

%token	LPE LME LMLE LDVE LMDE LLSHE LRSHE LANDE LXORE LORE LOROR LANDAND LEQ LNE LLE LGE LLSH LRSH LARROW LPP LMM LSIZEOF
%token	LVOID LCHAR LSHORT LINT LLONG LFLOAT LDOUBLE LSIGNED LUNSIGNED LUNION LSTRUCT LENUM LTYPE LID LSTRING LNUM LELLIPSES
%token	LIF LELSE LSWITCH LCASE LDEFAULT LWHILE LDO LFOR LGOTO LCONTINUE LBREAK LRETURN LAUTO LREGISTER LEXTERN LSTATIC LTYPEDEF LCONST LVOLATILE

%left	','
%right	'=' LPE LME LMLE LDVE LMDE LLSHE LRSHE LANDE LXORE LORE
%right	'?' ':'
%left	LOROR
%left	LANDAND
%left	'|'
%left	'^'
%left	'&'
%left	LEQ LNE
%left	'<' LLE '>' LGE
%left	LLSH LRSH
%left	'+' '-'
%left	'*' '/' '%'
%%
prog:	
|	prog xdecl

xdecl:
	fndef
|	decl

fndef:
	decor block
|	dspec decor block
|	decor dlist block
|	dspec decor dlist block

decl:
	dspec ';'
|	dspec dlist ';'

dlist:
	decor
|	decor '=' init
|	dlist ',' dlist

dspec:
	scspec
|	scspec dspec
|	tspec
|	tspec dspec
|	qual
|	qual dspec

init:
	expr
|	'{' ilist '}'
|	'{' ilist ',' '}'

ilist:
	init
|	ilist ',' ilist


sue:
	LSTRUCT LID
|	LSTRUCT	LID subody
|	LSTRUCT	subody
|	LUNION LID
|	LUNION LID subody 
|	LUNION subody
|	LENUM LID
|	LENUM LID enumbody
|	LENUM enumbody

subody:
	'{' sudecllist '}'

sudecllist:
	sudecl
|	sudecllist sudecl

sudecl:
	sqlist sudecorlist ';'

sudecor:
	decor
|	decor ':' expr

sudecorlist:
|	sudecor
|	sudecorlist ',' sudecor

sqlist:
      	tspec
|	tspec sqlist
|	qual
|	qual sqlist

enumbody:
	'{' enumlist '}'

enumlist:
     	LID
|	LID '=' expr
|	enumlist ',' enumlist
|	enumlist ','

decor:
     	ddecor
|	ptr ddecor

ptr:
   	'*'	
|	'*' ptr		{printf("here??\n");}
|	'*' qlist
|	'*' qlist ptr

ddecor:
      	LID
|	'(' decor ')'
|	ddecor '[' oexpr ']'
|	ddecor '(' parms ')'
|	ddecor '(' idlist ')'
|	ddecor '(' ')'

parms:
     	pdecl
|	parms ',' parms
|	parms ',' LELLIPSES

pdecl:
     	dspec
|	dspec decor
|	dspec adecor

idlist:
	LID
|	idlist ',' idlist

adecor:
	ptr
|	dadecor
|	ptr dadecor

dadecor:
       	'(' adecor ')'
|	'[' oexpr ']'
|	dadecor '[' oexpr ']'
|	'(' ')'
|	'(' parms ')'
|	dadecor '(' ')'
|	dadecor '(' parms ')'

tname:
	sqlist 
|	sqlist adecor

stmt:
	label
|	oexpr ';'
|	sel
|	iter
|	jmp

label:
	LID ':' stmt
|	LCASE expr ':' stmt
|	LDEFAULT ':' stmt

block:
	'{' slist '}'

slist:
|	slist decl
|	slist stmt

sel:
	LIF '(' expr ')' stmt
|	LIF '(' expr ')' stmt LELSE stmt
|	LSWITCH '(' expr ')' stmt

iter:
	LWHILE '(' expr ')' stmt
|	LDO stmt LWHILE '(' expr ')' ';'
|	LFOR '(' oexpr ';' oexpr ';' oexpr ')' stmt

jmp:
	LGOTO LID ';'
|	LCONTINUE ';'
|	LBREAK ';'
|	LRETURN oexpr ';'

exprlist:
	expr
|	exprlist ',' exprlist

expr:
    	uexpr
|	expr '*' expr
|	expr '/' expr
|	expr '%' expr
|	expr '+' expr
|	expr '-' expr
|	expr LLSH expr
|	expr LRSH expr
|	expr '<' expr
|	expr LLE expr
|	expr '>' expr
|	expr LGE expr
|	expr LEQ expr
|	expr LNE expr
|	expr '&' expr
|	expr '^' expr
|	expr '|' expr
|	expr LANDAND expr
|	expr LOROR expr
|	expr '?' expr ':' expr
|	expr '=' expr
|	expr LPE expr
|	expr LME expr
|	expr LMLE expr
|	expr LDVE expr
|	expr LMDE expr
|	expr LLSHE expr
|	expr LRSHE expr
|	expr LANDE expr
|	expr LXORE expr
|	expr LORE expr

cast:
	uexpr
|	'(' tname ')' cast

uexpr:
	pexpr
|	LPP uexpr
|	LMM uexpr
|	'&' cast
|	'*' cast
|	'+' cast
|	'-' cast
|	'~' cast
|	'!' cast
|	LSIZEOF uexpr
|	LSIZEOF '(' tname ')'
	
pexpr:
	'(' exprlist ')'
|	pexpr '[' expr ']'
|	pexpr '(' oelist ')'
|	pexpr '.' LID
|	pexpr LARROW LID
|	pexpr LPP
|	pexpr LMM
|	LID
|	LNUM
|	LSTRING

oexpr:
|	expr

oelist:
	oexpr
|	oelist ',' oelist

qlist:
	qual
|	qlist qual

qual:
    	LCONST
|	LVOLATILE

scspec:
      	LAUTO
|	LREGISTER
|	LSTATIC
|	LEXTERN
|	LTYPEDEF

tspec:
     	LVOID
|	LCHAR
|	LSHORT
|	LINT
|	LLONG
|	LFLOAT
|	LDOUBLE
|	LSIGNED
|	LUNSIGNED
|	LTYPE
|	sue
%%
void yyerror(char *s)
{
	errorposf("%s", s);
}
