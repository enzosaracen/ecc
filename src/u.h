#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef EXTERN
#define EXTERN extern
#endif

typedef struct Io Io;
typedef struct Src Src;
typedef struct Adr Adr;
typedef struct Sym Sym;
typedef struct Dstk Dstk;
typedef struct Type Type;
typedef struct Node Node;

/*
 *	par
 */
void	compile(char *);

/*
 *	gen
 */
void	gen(Node *n);

/* 
 *	util.c
 */
void	*emalloc(int);
char	*estrdup(char*);
void	panic(char *, ...);
