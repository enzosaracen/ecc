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
	OPE = 256,
	OME,
	OMLE,
	ODVE,
	OMDE,
	OLSHE,
	ORSHE,
	OANDE,
	OXORE,
	OORE,
	OOROR,
	OANDAND,
	OEQ,
	ONE,
	OLE,
	OGE
	OLSH,
	ORSH,
	OARROW,
	OPP,
	OMM,
	OAND,
	OADDR,
	OSIZEOF,
	OIF,
	OWHILE,
	ODOWHILE,
	OFOR,
	OLIST,
	OADD,
	OSUB,
	OPOS,
	ONEG,
	OBREAK,
	OCONTINUE,
	ORETURN,
	OSWITCH,
};

typedef struct Node Node;
struct Node {
	int	op;
	Node	*l;
	Node	*r;
};

/* 
 *	util.c
 */
void *emalloc(int);
char *estrdup(char*);
void panic(char *, ...);
void errorposf(char *, ...);

/*
 *	lex.c
 */
void lexinit(void);
Sym *lookup(void);
int yylex(void);
void compile(void);

void yyerror(char *s);

Src	src;
FILE	*outfile;
char	symb[NSYMB];
Sym	*hash[NHASH];
