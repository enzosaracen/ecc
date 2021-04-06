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

	fprintf(stderr, "%s:%d: \033[0;31;1merror:\033[0m ", src.name, src.line);
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fprintf(stderr, "\n");
	exit(1);
}

void gerrorf(char *fmt, ...)
{
	va_list arg;

	fprintf(stderr, "%s: \033[0;31;1merror:\033[0m ", src.name);
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fprintf(stderr, "\n");
	exit(1);
}

void warnf(char *fmt, ...)
{
	va_list arg;

	fprintf(stderr, "%s:%d: \033[0;33;1mwarning:\033[0m ", src.name, src.line);
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fprintf(stderr, "\n");
}

void yyerror(char *s)
{
	if(lastname)
		errorf("%s near %s", s, lastname);
	else
		errorf("%s", s);
}
