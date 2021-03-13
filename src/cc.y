%{
#include "u.h"
%}
%union {
	 Sym	*sym;
	 Node	*node;
	 Type	*type;
	 char	*sval;
	 char	cval;
	 long	lval;
}

%type	<node>	xdecl fndef decl dlist dspec init ilist sue subody sudecl sudecor sudecorlist sqlist enumbody enumlist decor ptr ddecor parms pdecl
%type	<node>	idlist adecor dadecor tname stmt label block slist sel iter jmp exprlist expr cast uexpr pexpr oexpr oelist qlist qual scspec tspec

%token	<sym>	LID LTYPE
%token	<lval>	LNUM
%token	<sval>	LSTRING
%token	<cval>	LCHARLIT
%token	LADDAS LSUBAS LMULAS LDIVAS LMODAS LLSHAS LRSHAS LANDAS LXORAS LORAS LOROR LANDAND LEQ LNE LLE LGE LLSH LRSH LARROW LINC LDEC LSIZEOF
%token	LVOID LCHAR LSHORT LINT LLONG LFLOAT LDOUBLE LSIGNED LUNSIGNED LUNION LSTRUCT LENUM LELLIPSES
%token	LIF LELSE LSWITCH LCASE LDEFAULT LWHILE LDO LFOR LGOTO LCONTINUE LBREAK LRETURN LAUTO LREGISTER LEXTERN LSTATIC LTYPEDEF LCONST LVOLATILE

%left	','
%right	'=' LADDAS LSUBAS LMULAS LDIVAS LMODAS LLSHE LRSHE LANDE LXORE LORE
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
|	dspec
	{
		lasttype = bitstype();
		bits = 0;
	}
	dlist ';'
	{
		
	}

dlist:
	decor
|	decor '=' init
|	dlist ',' dlist

dspec:
	scspec
	{
		spec($1);
	}
|	scspec dspec
	{
		spec($1);
	}
|	tspec
	{
		spec($1);
	}
|	tspec dspec
	{
		spec($1);
	}
|	qual
	{
		spec($1);
	}
|	qual dspec
	{
		spec($1);
	}

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
|	'*' qlist ddecor
	{
		$$ = new(OIND, $3, NULL);
	}

ddecor:
      	LID
|	'(' decor ')'
	{
		$$ = $2;
	}
|	ddecor '[' oexpr ']'
	{
		$$ = new(OARRAY, $1, $3);
	}
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
	{
		$$ = new(OIF, $3, $5);
	}
|	LIF '(' expr ')' stmt LELSE stmt
	{
		$$ = new(OIF, $3, new(OLIST, $5, $7));
	}
|	LSWITCH '(' expr ')' stmt
	{
		$$ = new(OSWITCH, $3, $5);
	}

iter:
	LWHILE '(' expr ')' stmt
	{
		$$ = new(OWHILE, $3, $5);
	}
|	LDO stmt LWHILE '(' expr ')' ';'
	{
		$$ = new(ODOWHILE, $2, $5);
	}
|	LFOR '(' oexpr ';' oexpr ';' oexpr ')' stmt
	{
		$$ = new(OFOR, $3, new(OLIST, $5, new(OLIST, $7, $9)));
	}

jmp:
	LGOTO LID ';'
	{
		$$ = new(OGOTO, NULL, NULL);
		$$->sym = $2;
	}
|	LCONTINUE ';'
	{
		$$ = new(OCONTINUE, NULL, NULL);
	}
|	LBREAK ';'
	{
		$$ = new(OBREAK, NULL, NULL);
	}
|	LRETURN oexpr ';'
	{
		$$ = new(ORETURN, $2, NULL);
	}

exprlist:
	expr
|	exprlist ',' exprlist
	{
		$$ = new(OLIST, $1, $3);
	}

expr:
    	uexpr
|	expr '*' expr
	{
		$$ = new(OMUL, $1, $3);
	}
|	expr '/' expr
	{
		$$ = new(ODIV, $1, $3);
	}
|	expr '%' expr
	{
		$$ = new(OMOD, $1, $3);
	}
|	expr '+' expr
	{
		$$ = new(OSUB, $1, $3);
	}
|	expr '-' expr
	{
		$$ = new(OADD, $1, $3);
	}
|	expr LLSH expr
	{
		$$ = new(OLSH, $1, $3);
	}
|	expr LRSH expr
	{
		$$ = new(ORSH, $1, $3);
	}
|	expr '<' expr
	{
		$$ = new(OLT, $1, $3);
	}
|	expr LLE expr
	{
		$$ = new(OLE, $1, $3);
	}
|	expr '>' expr
	{
		$$ = new(OGT, $1, $3);
	}
|	expr LGE expr
	{
		$$ = new(OGE, $1, $3);
	}
|	expr LEQ expr
	{
		$$ = new(OEQ, $1, $3);
	}
|	expr LNE expr
	{
		$$ = new(ONE, $1, $3);
	}
|	expr '&' expr
	{
		$$ = new(OAND, $1, $3);
	}
|	expr '^' expr
	{
		$$ = new(OXOR, $1, $3);
	}
|	expr '|' expr
	{
		$$ = new(OOR, $1, $3);
	}
|	expr LANDAND expr
	{
		$$ = new(OANDAND, $1, $3);
	}
|	expr LOROR expr
	{
		$$ = new(OOROR, $1, $3);
	}
|	expr '=' expr
	{
		$$ = new(OAS, $1, $3);
	}
|	expr LADDAS expr
	{
		$$ = new(OADDAS, $1, $3);
	}
|	expr LSUBAS expr
	{
		$$ = new(OSUBAS, $1, $3);
	}
|	expr LMULAS expr
	{
		$$ = new(OMULAS, $1, $3);
	}
|	expr LDIVAS expr
	{
		$$ = new(ODIVAS, $1, $3);
	}
|	expr LMODAS expr
	{
		$$ = new(OMODAS, $1, $3);
	}
|	expr LLSHAS expr
	{
		$$ = new(OLSHAS, $1, $3);
	}
|	expr LRSHAS expr
	{
		$$ = new(ORSHAS, $1, $3);
	}
|	expr LANDAS expr
	{
		$$ = new(OANDAS, $1, $3);
	}
|	expr LXORAS expr
	{
		$$ = new(OXORAS, $1, $3);
	}
|	expr LORAS expr
	{
		$$ = new(OORAS, $1, $3);
	}
|	expr '?' expr ':' expr
	{
		$$ = new(OCOND, $1, new(OLIST, $3, $5));
	}

cast:
	uexpr
|	'(' tname ')' cast
	{
		$$ = new(OCAST, $2, $4);
	}

uexpr:
	pexpr
|	LINC uexpr
	{
		$$ = new(OPREINC, $2, NULL);
	}
|	LDEC uexpr
	{
		$$ = new(OPREDEC, $2, NULL);
	}
|	'&' cast
	{
		$$ = new(OADDR, $2, NULL);
	}
|	'*' cast
	{
		$$ = new(OIND, $2, NULL);
	}
|	'+' cast
	{
		$$ = new(OPOS, $2, NULL);
	}
|	'-' cast
	{
		$$ = new(ONEG, $2, NULL);
	}
|	'~' cast
	{
		$$ = new(OBNOT, $2, NULL);
	}
|	'!' cast
	{
		$$ = new(ONOT, $2, NULL);
	}
|	LSIZEOF uexpr
	{
		$$ = new(OSIZEOF, $2, NULL);
	}
|	LSIZEOF '(' tname ')'
	{
		$$ = new(OSIZEOF, $3, NULL);
	}
	
pexpr:
	'(' exprlist ')'
	{
		$$ = $2;
	}
|	pexpr '[' expr ']'
	{
		$$ = new(OIND, new(OADD, $1, $3), NULL);
	}
|	pexpr '(' oelist ')'
|	pexpr '.' LID
|	pexpr LARROW LID
|	pexpr LINC
|	pexpr LDEC
|	LID
|	LNUM
|	LSTRING

oexpr:
     	{
		$$ = NULL;
	}
|	expr

oelist:
	oexpr
|	oelist ',' oelist

qlist:
|	qual
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
	errorf("%s", s);
}
