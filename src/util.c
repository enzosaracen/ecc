#include "u.h"

void *emalloc(int n)
{
	char *p;

	p = malloc(n);
	if(p == NULL)
		panic("out of memory");
	return p;
}

char *estrdup(char *s)
{
	char *r;
	int len;

	len = strlen(s);
	r = emalloc(len+1);
	strncpy(r, s, len);
	return r;
}

void panic(char *fmt, ...)
{
	va_list arg;
	
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fprintf(stderr, "\n");
	exit(1);
}

void errorf(char *fmt, ...)
{
	va_list arg;

	fprintf(stderr, "%s:%d:%d: error: ", src.name, src.line, src.col);
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fprintf(stderr, "\n");
	exit(1);
}

void warnf(char *fmt, ...)
{
	va_list arg;

	fprintf(stderr, "%s:%d:%d: warning: ", src.name, src.line, src.col);
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fprintf(stderr, "\n");
}

void yyerror(char *s)
{
	errorf("%s near %s", s, lastname);
}
