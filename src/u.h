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
	Node		*init;
	unsigned	offset;
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
	NTTYPE,
};

enum {
	CNONE = 0,
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
	Type	*type;
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
	BAUTO		= 1<<9,
	BEXTERN		= 1<<10,
	BREGISTER	= 1<<11,
	BSTATIC		= 1<<12,
	BTYPEDEF	= 1<<13,
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
Type *type(int, Type *);
void spec(int);
void setspec(void);

/*
 *	fold.c
 */
Node *fold(Node *);

extern	Src	src;
extern	FILE	*outfile;
extern	char	symb[NSYMB];
extern	Sym	*hash[NHASH];
extern	int	bits;
extern	int	class;
extern	Type	*t;
extern	int	offset;
extern	int	widths[NTTYPE];
