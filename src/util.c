#include "u.h"

void *emalloc(int n)
{
	char *v;

	v = malloc(n);
	if(v == NULL)
		panic("out of memory");
	return v;
}

char *estrdup(char *s)
{
	char *d;
	int len;

	len = strlen(s);
	d = emalloc(len+1);
	strncpy(d, s, len);
	return d;
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
