#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 16

typedef struct Src {
	FILE	*fp;
	int	line, col;
	char	*path;
} Src;

typedef struct Sym Sym;
struct Sym {
	int	ival;
	char	*name;
	Sym	*next;
};

typedef struct Env Env;
struct Env {
	Sym	*sym[HASHSIZE];
	Env	*prev;
} Env;

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
Sym *install(Sym *[], char *, int);
void envpush(void);
void envpop(void);

/*
 *	lex.c
 */
void compile(void);

Env *ge;
Src src;
