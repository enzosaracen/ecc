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

%type	<node>	oelist oexpr pexpr uexpr cast expr exprlist jmp iter sel idlist parms 
%type	<node>	stmt id ilist init dlist
%type	<sym>	decor ddecor

%token	<sym>	LID LTYPE
%token	<sval>	LSTRING
%token	<cval>	LCHARLIT
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
|	dspec decor block
|	decor dlist block
|	dspec decor dlist block

decl:
	dspec ';'
	{
		warnf("empty declaration useless");
	}
|	dspec dlist ';'
	{
	}

dlist:
	decor
|	decor '=' init
	{
	}
|	dlist ',' dlist
	{
		$$ = new(OLIST, $1, $3);
	}

init:
	expr
	{
		Node *n;

		n = fold($1);
		if(n->op != OCONST)
			errorf("initializer must be constant");
	}
|	'{' ilist '}'
	{
		$$ = $2;
	}
|	'{' ilist ',' '}'
	{
		$$ = $2;
	}

ilist:
	init
|	ilist ',' ilist
	{
		$$ = new(OLIST, $1, $3);
	}

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
	{
		Node *n;

		n = fold($3);
		if(n->op != OCONST)
			errorf("bit-field width must be constant");
		if(n->lval < 0)
			errorf("bit-field width must be non-negative");
	}

sudecorlist:
|	sudecor
|	sudecorlist ',' sudecor

dspec:
	scspec
	{
		setspec();
	}
|	tspec
	{
		setspec();
	}
|	qual
	{
		setspec();
	}
|	scspec dspec
|	tspec dspec
|	qual dspec

sqlist:
      	tspec
	{
		setspec();
	}
|	qual
	{
		setspec();
	}
|	tspec sqlist
|	qual sqlist

enumbody:
	'{' enumlist '}'

enumlist:
     	id
|	id '=' expr
|	enumlist ',' enumlist
|	enumlist ','

decor:
     	ddecor
|	'*' qlist ddecor
	{
		t = type(TPTR, t);
		t->width = widths[TPTR];
		$$ = $3;
	}

ddecor:
      	id
	{
		$$ = $1->sym;
		$$->type = t;
		$$->class = class;
		$$->offset = offset;
		offset += $$->type->width;
	}
|	'(' decor ')'
	{
		$$ = $2;
	}
|	ddecor '[' oexpr ']'
	{
		Node *n;

		t = type(TARRAY, t);
		t->width = 0;
		if($3 != NULL) {
			n = fold($3);
			if(n->op != OCONST)
				errorf("array size must be constant");
			else if(n->lval <= 0)
				errorf("array size must be positive");
			t->width = n->lval * t->sub->width;
			freenode(n);
		}
		$$ = $1;
	}
|	ddecor '(' parms ')'
	{
		$$ = $1;
	}
|	ddecor '(' ')'
	{
		$$ = $1;
	}

parms:
     	{
		$$ = NULL;
	}
|	pdecl
|	parms ',' parms
	{
		$$ = new(OLIST, $1, $3);
	}
|	parms ',' LELLIPSES

pdecl:
     	dspec
|	dspec decor
|	dspec adecor

idlist:
	id
|	idlist ',' idlist
	{
		$$ = new(OLIST, $1, $3);
	}

adecor:
	'*'
|	dadecor
|	'*' dadecor

dadecor:
	'(' ')'
|	'(' adecor ')'
|	'[' oexpr ']'
|	'(' parms ')'
|	dadecor '[' oexpr ']'
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

qlist:
|	qual
|	qlist qual

qual:
    	LCONST
|	LVOLATILE

scspec:
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

tspec:
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
|	LTYPE
|	sue

sue:
	LSTRUCT id
|	LSTRUCT	id subody
|	LSTRUCT	subody
|	LUNION id
|	LUNION id subody 
|	LUNION subody
|	LENUM id
|	LENUM id enumbody
|	LENUM enumbody

id:
	LID
	{
		$$ = new(OID, NULL, NULL);
		$$->sym = $1;
	}
%%
