#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct Src {
	FILE *fp;
	int line, col;
	char *path;
} Src;

void panic(char *, ...);
void *emalloc(int);
char *estrdup(char*);
