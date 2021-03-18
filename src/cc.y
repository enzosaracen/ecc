%{
#include "u.h"
%}
%union {
	 Sym	*sym;
	 Node	*node;
	 Type	*type;
	 char	*sval;
	 long	lval;
}

%type	<node>	oelist oexpr pexpr uexpr cast expr exprlist jmp iter sel idlist stmt 
%type	<node>	id ilist init dlist decor ddecor oadecor adecor dadecor parms
%type	<type>	tspec

%token	<sym>	LID LTYPE
%token	<sval>	LSTRING
%token	<lval>	LNUM
%token	LVOID LCHAR LSHORT LINT LLONG LFLOAT LDOUBLE LSIGNED LUNSIGNED LUNION LSTRUCT LENUM
%token	LADDAS LSUBAS LMULAS LDIVAS LMODAS LLSHAS LRSHAS LANDAS LXORAS LORAS LOROR LANDAND
%token	LEQ LNE LLE LGE LLSH LRSH LARROW LINC LDEC LSIZEOF LELLIPSES LIF LELSE LSWITCH LCASE LDEFAULT 
%token	LWHILE LDO LFOR LGOTO LCONTINUE LBREAK LRETURN LAUTO LREGISTER LEXTERN LSTATIC LTYPEDEF LCONST LVOLATILE

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
|	tspec decor block
|	decor dlist block
|	tspec decor dlist block

decl:
	tspec ';'
|	tspec dlist ';'

dlist:
	decor
	{
		decl($1, lasttype, lastclass);
	}
|	decor '=' init
	{
		decl($1, lasttype, lastclass);
	}
|	dlist ',' dlist

init:
	expr
|	'{' ilist '}'
|	'{' ilist ',' '}'

ilist:
	init
|	ilist ',' ilist
	{
		$$ = new(OLIST, $1, $3);
	}
	
decor:
	ddecor
|	'*' qlist decor
	{
		$$ = new(OIND, $3, NULL);
	}

ddecor:
      	id
|	'(' decor ')'
	{
		$$ = $2;
	}
|	ddecor '[' oexpr ']'
	{
		$$ = new(OARRAY, $1, $3);
	}
|	ddecor '(' parms ')'
	{
		$$ = new(OFUNC, $1, $3);
	}

parms:
	tspec adecor
	{
		$$ = $2;
	}
|	tspec decor
	{
		$$ = $2;
	}
|	parms ',' parms
	{
		$$ = new(OLIST, $1, $3);
	}
|	'.' '.' '.'
	{
		$$ = new(OELLIPSIS, NULL, NULL);
	}

idlist:
	id
|	idlist ',' idlist
	{
		$$ = new(OLIST, $1, $3);
	}

oadecor:
	{
		$$ = NULL;
	}
|	adecor

adecor:
	dadecor
|	'*' qlist
	{
		$$ = new(OIND, NULL, NULL);
	}
|	'*' qlist dadecor
	{
		$$ = new(OIND, $3, NULL);
	}

dadecor:
	'(' oadecor ')'
	{
		$$ = new(OFUNC, NULL, $2);
	}
|	'[' oexpr ']'
	{
		$$ = new(OARRAY, NULL, $2);
	}
|	'(' parms ')'
	{
		$$ = new(OFUNC, NULL, $2);
	}
|	dadecor '[' oexpr ']'
	{
		$$ = new(OARRAY, $1, $3);
	}
|	dadecor '(' parms ')'
	{
		$$ = new(OFUNC, $1, $3);
	}

tspec:
	qctlist
	{
		$$ = basetype();
		bits = 0;
		lasttype = $$;
	}
|	suespec
	{
		lasttype = $$;
	}
|	LTYPE
	{
		$$ = $1->type;
		lasttype = $$;
	}

suespec:
	LSTRUCT tag
|	LSTRUCT	tag subody
|	LSTRUCT	subody
|	LUNION tag
|	LUNION tag subody 
|	LUNION subody
|	LENUM tag
|	LENUM tag enumbody
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

enumbody:
	'{' enumlist '}'

enumlist:
     	id
|	id '=' expr
|	enumlist ',' enumlist
|	enumlist ','

qctlist:
	cname
|	tname
|	qname
|	cname qctlist
|	tname qctlist
|	qname qctlist

sqlist:
	cname
|	qname
|	cname sqlist
|	qname sqlist

qlist:
|	qname
|	qlist qname

qname:
    	LCONST
|	LVOLATILE

cname:
      	LAUTO
	{
		spec(BAUTO);
	}
|	LREGISTER
	{
		spec(BREGISTER);
	}
|	LSTATIC
	{
		spec(BSTATIC);
	}
|	LEXTERN
	{
		spec(BEXTERN);
	}
|	LTYPEDEF
	{
		spec(BTYPEDEF);
	}

tname:
     	LVOID
	{
		spec(BVOID);
	}
|	LCHAR
	{
		spec(BCHAR);
	}
|	LSHORT
	{
		spec(BSHORT);
	}
|	LINT
	{
		spec(BINT);
	}
|	LLONG
	{
		spec(BLONG);
	}
|	LFLOAT
	{
		spec(BFLOAT);
	}
|	LDOUBLE
	{
		spec(BDOUBLE);
	}
|	LSIGNED
	{
		spec(BSIGNED);
	}
|	LUNSIGNED
	{
		spec(BUNSIGNED);
	}


stmt:
	label
|	oexpr ';'
|	sel
|	iter
|	jmp

label:
	id ':' stmt
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
	LGOTO id ';'
	{
		$$ = new(OGOTO, $2, NULL);
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
		$$ = new(OADD, $1, $3);
	}
|	expr '-' expr
	{
		$$ = new(OSUB, $1, $3);
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
		$$ = new(OCAST, $4, NULL);
		$$->type = t;
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
		$$ = new(OSIZEOF, NULL, NULL);
		$$->type = t;
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
	{
		$$ = new(OFUNC, $1, $3);
	}
|	pexpr '.' id
	{
		$$ = new(ODOT, $1, $3);
	}
|	pexpr LARROW id
	{
		$$ = new(OARROW, $1, $3);
	}
|	pexpr LINC
	{
		$$ = new(OPOSTINC, $1, NULL);
	}
|	pexpr LDEC
	{
		$$ = new(OPOSTDEC, $1, NULL);
	}
|	id
|	LNUM
	{
		$$ = new(OCONST, NULL, NULL);
		$$->lval = $1;
	}
|	LSTRING	
	{
		$$ = new(OSTRING, NULL, NULL);
		$$->sval = $1;
	}

oexpr:
	{
		$$ = NULL;
	}
|	expr

oelist:
	oexpr
|	oelist ',' oelist
	{
		$$ = new(OLIST, $1, $3);
	}

tag:
	LID
|	LTYPE

id:
	LID
	{
		$$ = new(OID, NULL, NULL);
		$$->sym = $1;
	}
%%
