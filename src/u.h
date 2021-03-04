#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 16

typedef struct Src {
	FILE	*fp;
	int	line, col;
	char	*name;
} Src;

typedef struct Sym Sym;
struct Sym {
	char	*name;
	Sym	*next;
};

typedef struct Env Env;
struct Env {
	Sym	*sym[HASHSIZE];
	Env	*prev;
};

enum {
	NASSN,
	NBINOP,
	NID,
	NINT,
	NBLOCK,
};

typedef struct Node Node;
struct Node {
	int type;
	union {
		struct {
			Node *l;
			Node *r;
		} Assn;
		struct {
			int op;
			Node *l;
			Node *r;
		} Binop;
		struct {
			Sym *sym;	
		} Id;
		struct {
			int v;
		} Int;
	};
};

/* 
 *	util.c
 */
void *emalloc(int);
char *estrdup(char*);
void panic(char *, ...);
void errorposf(char *, ...);

/*
 *	env.c
 */
Sym *lookup(char *);
int install(Sym *[], char *);
void envpush(void);
void envpop(void);

/*
 *	lex.c
 */
void yyerror(char *s);
int yylex(void);
void compile(void);

/*
 *	gen.c
 */
void emit(char *, ...);
void gen(Node *n);

Env	*ge;
Src	src;
char	*outfile;
