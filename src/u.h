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
	int		lexical;
	char		*name;
	Sym		*next;
	unsigned	block;	
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
