%{
#include "p.h"
%}
%union {
	 Sym	*sym;
	 Node	*node;
	 Type	*type;
	 char	*sval;
	 long	lval;
	 struct {
		Sym *s;
		Type *t;
	 } syty;
}

%type	<node>	oelist oexpr pexpr uexpr cast expr exprlist jmp iter sel stmt
%type	<node>	id ilist init label slist
%type	<type>	suespec tspec otspec sudecor sdlist udlist sudecl sudlist sbody ubody oadecor adecor dadecor parms
%type	<syty>	decor ddecor
%type	<sym>	tag

%token	<sym>	LID LTYPE
%token	<sval>	LSTRING
%token	<lval>	LNUM
%token	LVOID LCHAR LSHORT LINT LLONG LFLOAT LDOUBLE LSIGNED LUNSIGNED LUNION LSTRUCT LENUM
%token	LADDAS LSUBAS LMULAS LDIVAS LMODAS LLSHAS LRSHAS LANDAS LXORAS LORAS LOROR LANDAND
%token	LEQ LNE LLE LGE LLSH LRSH LARROW LINC LDEC LSIZEOF LIF LELSE LSWITCH LCASE LDEFAULT 
%token	LWHILE LDO LFOR LGOTO LCONTINUE LBREAK LRETURN LAUTO LREGISTER LEXTERN LSTATIC LTYPEDEF LCONST LVOLATILE

%left	','
%right	'=' LADDAS LSUBAS LMULAS LDIVAS LMODAS LLSHAS LRSHAS LANDAS LXORAS LORAS
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
	otspec decor
	{
		Type *p;
	
		if($2.t->ttype != TFUNC)
			errorf("expected function type");
		idecl($2.s, $2.t, lastclass);
		push(NULL, DBLOCK);
		for(p = $2.t->list; p != NULL; p = p->list) {
			if(p->sym == NULL) {
				if(p->sub->ttype == TVOID)
					break;
				errorf("parameter name needed in function definition");
			}
			if(p->sub->width == 0)
				errorf("parameter %s has incomplete type", p->sym->name);
			idecl(p->sym, p->sub, CAUTO);
		}
	}
	'{' slist '}'
	{
		fold($5);
		gen($5);
		pop();
	}
|	otspec ';'
	{
		lastclass = CNONE;
	}
|	otspec dlist ';'
	{
		lastclass = CNONE;
	}

decl:
	tspec ';'
	{
		lastclass = CNONE;
	}
|	tspec dlist ';'
	{
		lastclass = CNONE;
	}

dlist:
	decor
	{
		idecl($1.s, $1.t, lastclass);
	}
|	decor '=' init
	{
		$1.s->init = $3;
		idecl($1.s, $1.t, lastclass);
	}
|	dlist ',' dlist

init:
	expr
|	'{' ilist '}'
|	'{' ilist ',' '}'

ilist:
	init
|	ilist ',' ilist
	
decor:
	ddecor
|	'*' qlist decor
	{
		$3.t = type(TPTR, $3.t);
		$$ = $3;
	}

ddecor:
	id
	{
		$$.t = lasttype;
		$$.s = $1->sym;
	}
|	'(' decor ')'
	{
		$$ = $2;
	}
|	ddecor '[' oexpr ']'
	{
		$$.s = $1.s;
		$$.t = type(TARRAY, $1.t);
		if($3 != NULL) {
			$3 = fold($3);
			if($3->op != OCONST)
				errorf("array size must be constant");
			else if($3->lval < 0)
				errorf("array size muust be positive");
			$$.t->width = $3->lval * $$.t->sub->width;
		}
	}
|	ddecor '(' parms ')'
	{
		$$.s = $1.s;
		$$.t = type(TFUNC, $1.t);
		$$.t->list = $3;
	}
|	ddecor '(' ')'
	{
		$$.s = $1.s;
		$$.t = type(TFUNC, $1.t);
		$$.t->list = type(TWRAP, types[TVOID]);
	}

parms:
     	/* parameter names without explicit types are not allowed */
	tspec oadecor
	{
		if(lastclass != CNONE)
			errorf("parameter declaration cannot have storage class");
		switch($2->ttype) {
		case TFUNC:
			$2 = type(TPTR, $2);
			break;
		case TARRAY:
			$2 = type(TPTR, $2->sub);
			break;
		}
		$$ = type(TWRAP, $2);
	}
|	tspec decor
	{
		if(lastclass != CNONE)
			errorf("parameter declaration cannot have storage class");
		switch($2.t->ttype) {
		case TFUNC:
			$2.t = type(TPTR, $2.t);
			break;
		case TARRAY:
			$2.t = type(TPTR, $2.t->sub);
			break;
		}
		$$ = type(TWRAP, $2.t);
		$$->sym = $2.s;
	}
|	parms ',' parms
	{
		if($1->sub->ttype == TVOID || $3->sub->ttype == TVOID)
			errorf("void must be the only parameter");
		$$ = $1;
		$1->list = $3;
	}
|	parms ',' '.' '.' '.'
	{
		/* ignore for now */
	}

oadecor:
	{
		$$ = lasttype;
	}
|	adecor

adecor:
	dadecor
|	'*' qlist
	{
		$$ = type(TPTR, lasttype);
	}
|	'*' qlist dadecor
	{
		$$ = type(TPTR, $3);
	}

dadecor:
	'(' oadecor ')'
	{
		$$ = $2;
	}
|	'[' oexpr ']'
	{
		$$ = type(TARRAY, lasttype);
		if($2 != NULL) {
			$2 = fold($2);
			if($2->op != OCONST)
				errorf("array size must be constant");
			else if($2->lval < 0)
				errorf("array size muust be positive");
			$$->width = $2->lval * $$->sub->width;
		}
	}
|	'(' parms ')'
	{
		$$ = type(TFUNC, lasttype);
		$$->list = $2;
	}
|	dadecor '[' oexpr ']'
	{
		$$ = type(TARRAY, $1);
		if($3 != NULL) {
			$3 = fold($3);
			if($3->op != OCONST)
				errorf("array size must be constant");
			else if($3->lval < 0)
				errorf("array size muust be positive");
			$$->width = $3->lval * $$->sub->width;
		}
	}
|	dadecor '(' parms ')'
	{
		$$ = type(TFUNC, $1);
		$$->list = $3;
	}

otspec:
      	{
      		$$ = types[TINT];
		lastclass = CNONE;
		lasttype = $$;
	}
|	tspec

tspec:
     	qctlist
	{
		$$ = btype();
	}
|	suespec
	{
		lasttype = $$;
	}
|	qclist suespec
	{
		$$ = $2;
		lasttype = $$;
	}
|	suespec qclist
	{
		lasttype = $$;
	}
|	qclist suespec qclist
	{
		$$ = $2;
		lasttype = $$;
	}
|	LTYPE
	{
		$$ = $1->type;
		lasttype = $$;
	}
|	qclist LTYPE
	{
		$$ = $2->type;
		lasttype = $$;
	}
|	LTYPE qclist
	{
		$$ = $1->type;
		lasttype = $$;
	}
|	qclist LTYPE qclist
	{
		$$ = $2->type;
		lasttype = $$;
	}

suespec:
	LSTRUCT tag
	{
		$$ = type(TSTRUCT, NULL);
		tdecl($2, $$);
	}
|	LSTRUCT	tag
	{
		$<type>$ = type(TSTRUCT, NULL);
		tdecl($2, $<type>$);
		nsue++;
	}
	sbody
	{
		$$ = $<type>3;
		$$->list = $4;
		$$->width = $4->width;
	}
|	LSTRUCT
	{
		nsue++;
	}
	sbody
	{
		$$ = type(TSTRUCT, NULL);
		$$->list = $3;
		$$->width = $3->width;
	}
|	LUNION tag
	{
		$$ = type(TUNION, NULL);
		tdecl($2, $$);
	}
|	LUNION tag
	{
		$<type>$ = type(TUNION, NULL);
		tdecl($2, $<type>$);
		nsue++;
	}
	ubody 
	{
		$$ = $<type>3;
		$$->list = $4;
		$$->width = $4->width;
	}
|	LUNION
	{
		nsue++;
	}
	ubody
	{
		$$ = type(TUNION, NULL);
		$$->list = $3;
		$$->width = $3->width;
	}
|	LENUM tag
|	LENUM tag enumbody
|	LENUM enumbody

sbody:
	'{' sdlist '}'
	{
		$$ = $2;
	}

sdlist:
	sudecl
	{
		$$->width = $$->sub->width;
	}
|	sudlist sudecl
	{
		$1->list = $2;
		$1->width += $2->sub->width;
		$$ = $1;
	}

ubody:
	'{' udlist '}'
	{
		$$ = $2;
	}

udlist:
	sudecl
	{
		$$->width = $$->sub->width;
	}
|	sudlist sudecl
	{
		$1->list = $2;
		if($2->sub->width > $1->width)
			$1->width = $2->sub->width;
		$$ = $1;
	}

sudecl:
	tspec sudlist ';'
	{
		if(lastclass != CNONE)
			errorf("member cannot have storage class");
		$$ = $2;
	}

sudecor:
	decor
	{
		if($1.s->nsue == nsue)
			errorf("duplicate member %s", $1.s->name);
		$1.s->nsue = nsue;
		$$ = type(TWRAP, $1.t);
		$$->sym = $1.s;
	}
|	decor ':' expr
	{
		/* ignore bit field for now */
		if($1.s->nsue == nsue)
			errorf("duplicate member %s", $1.s->name);
		$1.s->nsue = nsue;
		$$ = type(TWRAP, $1.t);
		$$->sym = $1.s;
	}

sudlist:
	sudecor
|	sudlist ',' sudecor
	{
		$1->list = $3;
		$$ = $1;
	}

enumbody:
	'{' enumlist '}'

enumlist:
     	id
|	id '=' expr
|	enumlist ',' enumlist
|	enumlist ','

qclist:
	qctlist
	{
		if(bits != 0)
			errorf("multiple data types");
	}

qctlist:
	qctname
|	qctname qctlist

qctname:
	qname
|	cname
|	tname

qlist:
|	qname qlist

qname:
    	LCONST
|	LVOLATILE

cname:
      	LAUTO
	{
		cspec(CAUTO);
	}
|	LREGISTER
	{
		cspec(CAUTO);
	}
|	LSTATIC
	{
		cspec(CSTATIC);
	}
|	LEXTERN
	{
		cspec(CEXTERN);
	}
|	LTYPEDEF
	{
		cspec(CTYPEDEF);
	}

tname:
     	LVOID
	{
		bits |= BVOID;
	}
|	LCHAR
	{
		bits |= BCHAR;
	}
|	LSHORT
	{
		bits |= BSHORT;
	}
|	LINT
	{
		bits |= BINT;
	}
|	LLONG
	{
		bits |= BLONG;
	}
|	LFLOAT
	{
		bits |= BFLOAT;
	}
|	LDOUBLE
	{
		bits |= BDOUBLE;
	}
|	LSIGNED
	{
		bits |= BSIGNED;
	}
|	LUNSIGNED
	{
		bits |= BUNSIGNED;
	}


stmt:
	label
|	oexpr ';'
|	sel
|	iter
|	jmp
|	'{'
	{
		push(NULL, DBLOCK);
	}
	slist '}'
	{
		pop();
		$$ = $3;
	}

label:
	LID ':' stmt
	{
		ldecl($1);
		$$ = new(OLABEL, $3, NULL);
		$$->sym = $1;
	}
|	LCASE expr ':' stmt
|	LDEFAULT ':' stmt

slist:
     	{
     		$$ = NULL;	
	}
|	slist decl
|	slist stmt
	{
		$$ = new(OLIST, $1, $2);
	}

sel:
	LIF '(' expr ')' stmt
	{
		$$ = new(OIF, $3, $5);
		ntype($$);
	}
|	LIF '(' expr ')' stmt LELSE stmt
	{
		$$ = new(OIF, $3, new(OLIST, $5, $7));
		ntype($$);
	}
|	LSWITCH '(' expr ')' stmt
	{
		$$ = new(OSWITCH, $3, $5);
		ntype($$);
	}

iter:
	LWHILE '(' expr ')' stmt
	{
		$$ = new(OWHILE, $3, $5);
		ntype($$);
	}
|	LDO stmt LWHILE '(' expr ')' ';'
	{
		$$ = new(ODOWHILE, $2, $5);
		ntype($$);
	}
|	LFOR '(' oexpr ';' oexpr ';' oexpr ')' stmt
	{
		$$ = new(OFOR, $3, new(OLIST, $5, new(OLIST, $7, $9)));
		ntype($$);
	}

jmp:
	LGOTO id ';'
	{
		$$ = new(OGOTO, $2, NULL);
		ntype($$);
	}
|	LCONTINUE ';'
	{
		$$ = new(OCONTINUE, NULL, NULL);
		ntype($$);
	}
|	LBREAK ';'
	{
		$$ = new(OBREAK, NULL, NULL);
		ntype($$);
	}
|	LRETURN oexpr ';'
	{
		$$ = new(ORETURN, $2, NULL);
		ntype($$);
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
		ntype($$);
	}
|	expr '/' expr
	{
		$$ = new(ODIV, $1, $3);
		ntype($$);
	}
|	expr '%' expr
	{
		$$ = new(OMOD, $1, $3);
		ntype($$);
	}
|	expr '+' expr
	{
		$$ = new(OADD, $1, $3);
		ntype($$);
	}
|	expr '-' expr
	{
		$$ = new(OSUB, $1, $3);
		ntype($$);
	}
|	expr LLSH expr
	{
		$$ = new(OLSH, $1, $3);
		ntype($$);
	}
|	expr LRSH expr
	{
		$$ = new(ORSH, $1, $3);
		ntype($$);
	}
|	expr '<' expr
	{
		$$ = new(OLT, $1, $3);
		ntype($$);
	}
|	expr LLE expr
	{
		$$ = new(OLE, $1, $3);
		ntype($$);
	}
|	expr '>' expr
	{
		$$ = new(OGT, $1, $3);
		ntype($$);
	}
|	expr LGE expr
	{
		$$ = new(OGE, $1, $3);
		ntype($$);
	}
|	expr LEQ expr
	{
		$$ = new(OEQ, $1, $3);
		ntype($$);
	}
|	expr LNE expr
	{
		$$ = new(ONE, $1, $3);
		ntype($$);
	}
|	expr '&' expr
	{
		$$ = new(OAND, $1, $3);
		ntype($$);
	}
|	expr '^' expr
	{
		$$ = new(OXOR, $1, $3);
		ntype($$);
	}
|	expr '|' expr
	{
		$$ = new(OOR, $1, $3);
		ntype($$);
	}
|	expr LANDAND expr
	{
		$$ = new(OANDAND, $1, $3);
		ntype($$);
	}
|	expr LOROR expr
	{
		$$ = new(OOROR, $1, $3);
		ntype($$);
	}
|	expr '=' expr
	{
		$$ = new(OAS, $1, $3);
		ntype($$);
	}
|	expr LADDAS expr
	{
		$$ = new(OADDAS, $1, $3);
		ntype($$);
	}
|	expr LSUBAS expr
	{
		$$ = new(OSUBAS, $1, $3);
		ntype($$);
	}
|	expr LMULAS expr
	{
		$$ = new(OMULAS, $1, $3);
		ntype($$);
	}
|	expr LDIVAS expr
	{
		$$ = new(ODIVAS, $1, $3);
		ntype($$);
	}
|	expr LMODAS expr
	{
		$$ = new(OMODAS, $1, $3);
		ntype($$);
	}
|	expr LLSHAS expr
	{
		$$ = new(OLSHAS, $1, $3);
		ntype($$);
	}
|	expr LRSHAS expr
	{
		$$ = new(ORSHAS, $1, $3);
		ntype($$);
	}
|	expr LANDAS expr
	{
		$$ = new(OANDAS, $1, $3);
		ntype($$);
	}
|	expr LXORAS expr
	{
		$$ = new(OXORAS, $1, $3);
		ntype($$);
	}
|	expr LORAS expr
	{
		$$ = new(OORAS, $1, $3);
		ntype($$);
	}
|	expr '?' expr ':' expr
	{
		$$ = new(OCOND, $1, new(OLIST, $3, $5));
		ntype($$);
	}

cast:
	uexpr
|	'(' tspec oadecor ')' cast
	{
		$$ = $5;
		if(!sametype($5->type, lasttype)) {
			$$ = new(OCAST, $5, NULL);
			$$->type = lasttype;
			ntype($$);
		}
	}
uexpr:
	pexpr
|	LINC uexpr
	{
		$$ = new(OPREINC, $2, NULL);
		ntype($$);
	}
|	LDEC uexpr
	{
		$$ = new(OPREDEC, $2, NULL);
		ntype($$);
	}
|	'&' cast
	{
		$$ = new(OADDR, $2, NULL);
		ntype($$);
	}
|	'*' cast
	{
		$$ = new(OIND, $2, NULL);
		ntype($$);
	}
|	'+' cast
	{
		$$ = new(OPOS, $2, NULL);
		ntype($$);
	}
|	'-' cast
	{
		$$ = new(ONEG, $2, NULL);
		ntype($$);
	}
|	'~' cast
	{
		$$ = new(OBNOT, $2, NULL);
		ntype($$);
	}
|	'!' cast
	{
		$$ = new(ONOT, $2, NULL);
		ntype($$);
	}
|	LSIZEOF uexpr
	{
		$$ = new(OSIZEOF, $2, NULL);
		ntype($$);
	}
|	LSIZEOF '(' tspec ')'
	{
		$$ = new(OSIZEOF, NULL, NULL);
		$$->type = lasttype;
		ntype($$);
	}
	
pexpr:
	'(' exprlist ')'
	{
		$$ = $2;
	}
|	pexpr '[' expr ']'
	{
		$$ = new(OIND, new(OADD, $1, $3), NULL);
		ntype($$);
	}
|	pexpr '(' oelist ')'
	{
		$$ = new(OFUNC, $1, $3);
		ntype($$);
	}
|	pexpr '.' tag
	{
		$$ = new(ODOT, $1, NULL);
		$$->sym = $3;
		ntype($$);
	}
|	pexpr LARROW tag
	{
		$$ = new(OARROW, $1, NULL);
		$$->sym = $3;
		ntype($$);
	}
|	pexpr LINC
	{
		$$ = new(OPOSTINC, $1, NULL);
		ntype($$);
	}
|	pexpr LDEC
	{
		$$ = new(OPOSTDEC, $1, NULL);
		ntype($$);
	}
|	id
|	LNUM
	{
		$$ = new(OCONST, NULL, NULL);
		$$->lval = $1;
		ntype($$);
	}
|	LSTRING	
	{
		$$ = new(OSTRING, NULL, NULL);
		$$->sval = $1;
		ntype($$);
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

