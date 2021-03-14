#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define NHASH 1024
#define NSYMB 1024

typedef struct Src Src;
typedef struct Sym Sym;
typedef struct Type Type;
typedef struct Node Node;

struct Src {
	FILE	*fp;
	int	line, col;
	char	*name;
};

struct Sym {
	int		lex;
	Type		*type;
	char		*name;
	Sym		*next;
	unsigned	block;
	int		class;
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

struct Type {
	int	ttype;
	int	width;
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
	BCONST		= 1<<9,
	BVOLATILE	= 1<<10,
	BAUTO		= 1<<11,
	BEXTERN		= 1<<12,
	BREGISTER	= 1<<13,
	BSTATIC		= 1<<14,
	BTYPEDEF	= 1<<15,
};

/* 
 *	util.c
 */
void *emalloc(int);
char *estrdup(char*);
void panic(char *, ...);
void errorf(char *, ...);
void warnf(char *, ...);
void yyerror(char *);

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
void prtree(Node *, int);

/*
 * 	decl.c
 */
Type *decl(Node *);
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
