#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct Src Src;
typedef struct Sym Sym;
typedef struct Dstk Dstk;
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
	Type		*suetag;
	Node		*label;
	char		*name;
	int		class;
	unsigned	offset;
	unsigned	block;
	Sym		*next;
};

/*
 * new decls in blocks overwrite syms in the hash table,
 * but the old sym info is saved on the decl stack, and on
 * block exit decls from the decl stack are popped and the
 * syms are restored to their saved values
 */
enum {
	DOTHER,
	DSUETAG,
	DLABEL,
	DBLOCK,		/* dblock marks the beginning of a new sym save,
			   so when popping off the stack, we stop when we see a dblock */
};

struct Dstk {
	Sym		*sym;
	Type		*type;
	int		class;
	int		dtype;
	Node		*label;
	unsigned	block;
	Dstk		*prev;
};

enum {
	TVOID,
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
	TPTR,
	TARRAY,
	TENUM,
	TFUNC,
	TSTRUCT,
	TUNION,
};

enum {
	CNONE,
	CAUTO,
	CEXTERN,
	CSTATIC,
	CGLOBAL,
	CTYPEDEF,
};

struct Type {
	int	ttype;
	int	width;
	Type	*sub;
	Sym	*sym;
	Type	*list;
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
	OBIT,
	OBNOT,
	OBREAK,
	OCASE,
	OCAST,
	OCOND,
	OCONST,
	OCONTINUE,
	ODIV,
	ODIVAS,
	ODOT,
	ODOWHILE,
	OELLIPSIS,
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
	OPARM,
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
void	*emalloc(int);
char	*estrdup(char*);
void	panic(char *, ...);
void	errorf(char *, ...);
void	warnf(char *, ...);
void	yyerror(char *);

/*
 *	lex.c
 */
void	lexinit(void);
Sym	*lookup(void);
int	yylex(void);
void	compile(void);

/*
 * 	ast.c
 */
Node	*new(int, Node *, Node *);
void	freenode(Node *);
Node	*fold(Node *);
char	*op2str(int);
void	prtree(Node *, int);

/*
 * 	decl.c
 */
Type	*type(int, Type *);
Type	*decl(Node *, Type *, int, int);
void	idecl(Sym *s, Type *t, int c);
void	ldecl(Sym *, Node *);
void	pdecl(Node *, Type *);
Type	*ptype(Node *);
void	push(Sym *, int);
void	pop(void);
void	spec(int);
Type	*btype(void);
char	*type2str(int);
void	prtype(Type *t, int);

extern	Src		src;
extern	FILE		*outfile;
extern	int		bits;
extern	int		lastclass;
extern	Type		*lasttype;
extern	int		offset;
extern	Type		*types[];
extern	char		*lastname;
extern	Dstk		*declstk;
extern	unsigned	block;
