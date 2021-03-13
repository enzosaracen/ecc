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
	TFLOAT
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
	int t;
	Type *sub;
};

enum {
	OADD,
	OADDAS,
	OADDR,
	OAND,
	OANDAND,
	OANDAS,
	OARROW,
	OAS,
	OBNOT,
	OBREAK,
	OCAST,
	OCOND,
	OCONTINUE,
	ODIV,
	ODIVAS,
	ODOWHILE,
	OEQ,
	OFOR,
	OGE,
	OGOTO,
	OGT,
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
};

/* 
 *	util.c
 */
void *emalloc(int);
char *estrdup(char*);
void panic(char *, ...);
void errorf(char *, ...);

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

/*
 * 	type.c
 */
void spec(int);
int bitstype(void)

Src	src;
FILE	*outfile;
char	symb[NSYMB];
Sym	*hash[NHASH];
int	bits
int	lasttype;
