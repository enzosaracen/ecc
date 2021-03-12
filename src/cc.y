%{
#include "u.h"
%}
%union {
	 Sym *sym;
	 Node *node;
	 Type *type;
}

%type	xdecl fndef decl dlist dspec init ilist sue subody sudecl sudecor sudecorlist sqlist enumbody enumlist decor ptr ddecor parms pdecl
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
|	'*' ptr
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
	LIF '(' expr ')' stmt				{ $$ = new(OIF, $3, $5); }
|	LIF '(' expr ')' stmt LELSE stmt		{ $$ = new(OIF, $3, new(OLIST, $5, $7)); }
|	LSWITCH '(' expr ')' stmt			{ $$ = new(OSWITCH, $3, $5); }

iter:
	LWHILE '(' expr ')' stmt			{ $$ = new(OWHILE, $3, $5); }
|	LDO stmt LWHILE '(' expr ')' ';'		{ $$ = new(ODOWHILE, $2, $5); }
|	LFOR '(' oexpr ';' oexpr ';' oexpr ')' stmt	{ $$ = new(OFOR, $3, new(OLIST, $5, new(OLIST, $7, $9))); }

jmp:
	LGOTO LID ';'					{ $$ = new(OGOTO, $2, NULL); }
|	LCONTINUE ';'					{ $$ = new(OCONTINUE, NULL, NULL); }
|	LBREAK ';'					{ $$ = new(OBREAK, NULL, NULL); }
|	LRETURN oexpr ';'				{ $$ = new(ORETURN, $2, NULL); }

exprlist:
	expr
|	exprlist ',' exprlist				{ $$ = new(OLIST, $1, $3); }

expr:
    	uexpr
|	expr '*' expr					{ $$ = new('*', $1, $3); }
|	expr '/' expr					{ $$ = new('/', $1, $3); }
|	expr '%' expr					{ $$ = new('%', $1, $3); }
|	expr '+' expr					{ $$ = new(OSUB, $1, $3); }
|	expr '-' expr					{ $$ = new(OADD, $1, $3); }
|	expr LLSH expr					{ $$ = new(OLSH, $1, $3); }
|	expr LRSH expr					{ $$ = new(ORSH, $1, $3); }
|	expr '<' expr					{ $$ = new('<', $1, $3); }
|	expr LLE expr					{ $$ = new(OLE, $1, $3); }
|	expr '>' expr					{ $$ = new('>', $1, $3); }
|	expr LGE expr					{ $$ = new(OGE, $1, $3); }
|	expr LEQ expr					{ $$ = new(OEQ, $1, $3); }
|	expr LNE expr					{ $$ = new(ONE, $1, $3); }
|	expr '&' expr					{ $$ = new(OAND, $1, $3); }
|	expr '^' expr					{ $$ = new('^', $1, $3); }
|	expr '|' expr					{ $$ = new('|', $1, $3); }
|	expr LANDAND expr				{ $$ = new(OANDAND, $1, $3); }
|	expr LOROR expr					{ $$ = new(OOROR, $1, $3); }
|	expr '=' expr					{ $$ = new('=', $1, $3); }
|	expr LPE expr					{ $$ = new(OPE, $1, $3); }
|	expr LME expr					{ $$ = new(OME, $1, $3); }
|	expr LMLE expr					{ $$ = new(OMLE, $1, $3); }
|	expr LDVE expr					{ $$ = new(ODVE, $1, $3); }
|	expr LMDE expr					{ $$ = new(OMDE, $1, $3); }
|	expr LLSHE expr					{ $$ = new(OLSHE, $1, $3); }
|	expr LRSHE expr					{ $$ = new(ORSHE, $1, $3); }
|	expr LANDE expr					{ $$ = new(OANDE, $1, $3); }
|	expr LXORE expr					{ $$ = new(OXORE, $1, $3); }
|	expr LORE expr					{ $$ = new(OORE, $1, $3); }
|	expr '?' expr ':' expr				{ $$ = new('?', $1, new(OLIST, $3, $5)); }

cast:
	uexpr
|	'(' tname ')' cast

uexpr:
	pexpr
|	LPP uexpr					{ $$ = new(OPP, $2, NULL); }
|	LMM uexpr					{ $$ = new(OMM, $2, NULL); }
|	'&' cast					{ $$ = new(OADDR, $2, NULL); }
|	'*' cast					{ $$ = new('*', $2, NULL); }
|	'+' cast					{ $$ = new(OPOS, $2, NULL); }
|	'-' cast					{ $$ = new(ONEG, $2, NULL); }
|	'~' cast					{ $$ = new('~', $2, NULL); }
|	'!' cast					{ $$ = new('!', $2, NULL); }
|	LSIZEOF uexpr					{ $$ = new(OSIZEOF, $2, NULL); }
|	LSIZEOF '(' tname ')'				{ $$ = new(OSIZEOF, $2, NULL); }
	
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
