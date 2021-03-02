#include <u.h>

void *emalloc(int n)
{
	char *v;

	v = malloc(n);
	if(v == NULL)
		panic("malloc failed");
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
	va_list l;
	
	va_start(l, fmt);
	vfprintf(stderr, fmt, l);
	va_end(l);
	fprintf(stderr, "\n");
	exit(1);
}

void errorposf(char *fmt, ...)
{
	va_list l;

	fprintf(stderr, "%s:%d:%d: ", src.path, src.line, src.col);
	va_start(l, fmt);
	vfprintf(stderr, fmt, l);
	va_end(l);
	fprintf(stderr, "\n");
	exit(1);
}
