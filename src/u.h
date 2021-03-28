#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct Io Io;
typedef struct Src Src;
typedef struct Marg Marg;
typedef struct Sym Sym;
typedef struct Dstk Dstk;
typedef struct Type Type;
typedef struct Node Node;

struct Src {
	char	*name;
	int	line;
};

struct Io {
	FILE	*fp;
	char	*buf, *p;
	int	len;
};

struct Marg {
	char	*name;
	Marg	*next;
};

struct Sym {
	int		lex;
	Type		*type;
	Type		*tag;
	char		*label;
	Node		*init;
	char		*name;
	int		class;
	unsigned	block;
	unsigned	nsue;
	char		*mac;
	Marg		*marg;
	Sym		*next;
};

enum {
	DOTHER,
	DTAG,
	DLABEL,
	DBLOCK,
};

struct Dstk {
	Sym		*sym;
	Type		*type;
	int		class;
	int		dtype;
	char		*label;
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
	TFLOAT,
	TDOUBLE,
	TPTR,
	TARRAY,
	TENUM,
	TFUNC,
	TSTRUCT,
	TUNION,
	TWRAP,	/* wrapper type to store lists for members and parms
		 * without causing modifications to the base types stored in types[],
		 * also allows incrementally building up struct and union width,
		 * which consequently provides offsets for members
		 * note - there is probably a better solution to this */
	NTTYPE,
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
	int		ttype;
	int		width;
	Type		*sub;
	Sym		*sym;
	Type		*list;
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
	OLABEL,
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

enum {
	SNONE,
	SIDECL,
	SMEMB,
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
void	popio(void);
void	pushio(FILE *, char *, int);
char	next(void);
void	unget(char);
void	putbuf(char *, char);
int	yylex(void);
void	compile(char *);
void	pp(void);
char	ppnext(void);
void	ppdefine(void);
void	ppundef(void);

/*
 * 	types.c
 */
Type	*type(int, Type *);
char	*newlabel(void);
int	sametype(Type *, Type *);
void	idecl(Sym *, Type *, int );
void	ldecl(Sym *);
void	tdecl(Sym *, Type *);
void	push(Sym *, int);
void	pop(void);
Type	*btype(void);
void	cspec(int);
int	islval(Node *);
Type	*getmemb(Type *t, Sym *s);
Node	*new(int, Node *, Node *);
Node	*ntype(Node *);
Node	*fold(Node *);
char	*op2str(int);
void	prtree(Node *, int);
char	*type2str(int);
void	prtype(Type *, int);

extern	FILE		*outfp;
extern	int		bits;
extern	int		lastclass;
extern	Type		*lasttype;
extern	Type		*types[];
extern	char		*lastname;
extern	Dstk		*declstk;
extern	unsigned	block;
extern	unsigned	nsue;
extern	Src		src;
