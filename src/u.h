#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define NHASH 1024
#define NSYMB 1024

typedef struct Src {
	FILE	*fp;
	int	line, col;
	char	*name;
} Src;

typedef struct Sym Sym;
struct Sym {
	int		lex;
	char		*name;
	Sym		*next;
	unsigned	block;	
};

enum {
	TCHAR,
	TUCHAR,
	TSHORT,
	TUSHORT,
	TINT,
	TUINT,
	TLONG,
	TULONG,
	TLLONG,
	TULLONG,
	TFLOAT,
	TDOUBLE,
	TLDOUBLE,
	TARRAY,
	TENUM,
	TFUNC,
	TPTR,
	TSTRUCT,
	TVOID,
};

enum {
	CNONE,
	CAUTO,
	CEXTERN,
	CREGISTER,
	CSTATIC,
	CTYPEDEF,
};

typedef struct Type Type;
struct Type {
	int	ttype;
	int	width;
	Sym	*sym;
	Type	*sub;
};

enum {
	OADD,
	OADDAS,
	OADDR,
	OAND,
	OANDAND,
	OANDAS,
	OARRAY,
	OARROW,
	OAS,
	OBNOT,
	OBREAK,
	OCAST,
	OCASE,
	OCOND,
	OCONST,
	OCONTINUE,
	ODIV,
	ODIVAS,
	ODOT,
	ODOWHILE,
	OEQ,
	OFOR,
	OFUNC,
	OGE,
	OGOTO,
	OGT,
	OID,
	OIF,
	OIND,
	OLE,
	OLIST,
	OLSH,
	OLSHAS,
	OLT,
	OMOD,
	OMODAS,
	OMUL,
	OMULAS,
	ONE,
	ONEG,
	ONOT,
	OOR,
	OORAS,
	OOROR,
	OPOS,
	OPOSTDEC,
	OPOSTINC,
	OPREDEC,
	OPREINC,
	ORETURN,
	ORSH,
	ORSHAS,
	OSIZEOF,
	OSTRING,
	OSUB,
	OSUBAS,
	OSWITCH,
	OWHILE,
	OXOR,
	OXORAS,
};

typedef struct Node Node;
struct Node {
	int	op;
	Node	*l;
	Node	*r;
	Sym	*sym;
	Type	*t;
	long	lval;
	char	*sval;
};

enum {
	BVOID		= 1<<0,
	BCHAR		= 1<<1,
	BSHORT		= 1<<2,
	BINT		= 1<<3,
	BLONG		= 1<<4,
	BFLOAT		= 1<<5,
	BDOUBLE		= 1<<6,
	BSIGNED		= 1<<7,
	BUNSIGNED	= 1<<8,
	BSTRUCT		= 1<<9,
	BENUM		= 1<<10,
};

/* 
 *	util.c
 */
void *emalloc(int);
char *estrdup(char*);
void panic(char *, ...);
void errorf(char *, ...);
void warnf(char *, ...);

/*
 *	lex.c
 */
void lexinit(void);
Sym *lookup(void);
int yylex(void);
void compile(void);

/*
 * 	ast.c
 */
Node *new(int, Node *, Node *);
void freenode(Node *);

/*
 * 	decl.c
 */
void spec(int);
int getspec(void);

/*
 *	fold.c
 */
Node *fold(Node *);

Src	src;
FILE	*outfile;
char	symb[NSYMB];
Sym	*hash[NHASH];
int	bits;
int	class;
