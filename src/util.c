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
	va_list list;
	
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
	fprintf(stderr, "\n");
	exit(1);
}
