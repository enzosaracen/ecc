%{
#include "u.h"
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
%type	<type>	suespec tspec otspec sudecor sudecorlist sudecl sudecllist subody oadecor adecor dadecor parms
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
		/* link the types to the attached syms */
		for(p = $2.t->list; p != NULL; p = p->list) {
			if(p->sym == NULL) {
				if(p->sub->ttype == TVOID)
					break;
				errorf("parameter name needed in function definition");
			}
			if(incomp(p->sub))
				errorf("parameter %s has incomplete type", p->sym->name);
			idecl(p->sym, p->sub, CAUTO);
		}
	}
	'{' slist '}'
	{
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
|	decor '=' init
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
		$$.t = NULL;
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
			freenode($3);
		}
	}
|	ddecor '(' parms ')'
	{
		$$.t = type(TFUNC, $1.t);
		$$.t->list = $3;
		$$.s = $1.s;
	}
|	ddecor '(' ')'
	{
		$$.t = type(TFUNC, $1.t);
		$$.s = $1.s;
	}

parms:
	tspec oadecor
	{
		if(lastclass != CNONE)
			errorf("parameter declaration cannot have storage class");
		$$ = $2;
		switch($$->ttype) {
		case TFUNC:
			$$ = type(TPTR, $$);
			break;
		case TARRAY:
			$$ = type(TPTR, $$->sub);
			break;
		}
	}
	/* attach syms to types we return because we do not know if it is a func def or not */
|	tspec decor
	{
		if(lastclass != CNONE)
			errorf("parameter declaration cannot have storage class");
		$$ = $2.t;
		switch($$->ttype) {
		case TFUNC:
			$$ = type(TPTR, $$);
			break;
		case TARRAY:
			$$ = type(TPTR, $$->sub);
			break;
		}
		$$->sym = $2.s;
	}
|	parms ',' parms
	{
		if($1->ttype == TVOID || $3->ttype == TVOID)
			errorf("void must be the only parameter");
		$$ = $1;
		$1->list = $3->list;
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
			freenode($2);
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
			freenode($3);
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
	subody
	{
		$$->sub = $<type>3;
	}
|	LSTRUCT
	{
		nsue++;
	}
	subody
	{
		$$ = type(TSTRUCT, NULL);
		$$->sub = $<type>2;
	}
|	LUNION tag
	{
		$$ = type(TUNION, NULL);
		tdecl($2, $$);
	}
|	LUNION tag
	{
		nsue++;
		tdecl($2, $<type>$);
	}
	subody 
	{
		$$ = type(TUNION, NULL);
		$$->sub = $<type>3;
	}
|	LUNION
	{
		nsue++;
	}
	subody
	{
		$$ = type(TUNION, NULL);
		$$->sub = $<type>2;
	}
|	LENUM tag
|	LENUM tag enumbody
|	LENUM enumbody

subody:
	'{' sudecllist '}'
	{
		$$ = $2;
	}

sudecllist:
	sudecl
|	sudecllist sudecl
	{
		$1->list = $2->list;
		$$ = $1;
	}

sudecl:
	tspec sudecorlist ';'
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
		$$ = type(TMEMB, $1.t);
		$$->sym = $1.s;
	}
|	decor ':' expr
	{
		/* ignore bit field for now */
		if($1.s->nsue == nsue)
			errorf("duplicate member %s", $1.s->name);
		$1.s->nsue = nsue;
		$$ = type(TMEMB, $1.t);
		$$->sym = $1.s;
	}

sudecorlist:
	sudecor
|	sudecorlist ',' sudecor
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
|	'{'
	{
		push(NULL, DBLOCK);
	}
	slist '}'
	{
		pop();
		$$ = $<node>2;
	}

label:
	LID ':' stmt
	{
		ldecl($1, $3);
		$$ = $3;
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
|	'(' tspec ')' cast
	{
		$$ = new(OCAST, $4, NULL);
		$$->type = lasttype;
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
|	LSIZEOF '(' tspec ')'
	{
		$$ = new(OSIZEOF, NULL, NULL);
		$$->type = lasttype;
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
