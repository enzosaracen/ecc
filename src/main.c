#include "u.h"

#define MAX 32

Src	src;
FILE	*outfile;
char	symb[NSYMB];
Sym	*hash[NHASH];
int	bits;
int	lastclass;
Type	*lasttype;
int	offset;

int main(int argc, char *argv[])
{
	int i;
	char *p, *bufp, **filesp, buf[MAX], *files[MAX], *outname;

	outname = NULL;
	filesp = files;
	if(argc < 2)
		panic("usage: ecc [-options] files");
	for(i = 1; i < argc; i++) {
		/* todo - cleaner option parsing */
		if(argv[i][0] == '-') {
			switch(argv[i][1]) {
			case 'o':
				bufp = buf;
				if(argv[i][2]) {
					p = &argv[i][2];
					while(*p++) {
						if(bufp-buf >= MAX-1)
							panic("filename after -o too long");
						*bufp++ = *p;
					}
					*bufp = 0;
					outname = estrdup(buf);
				} else {
					if(i+1 >= argc)
						panic("missing filename after -o");
					outname = argv[++i];
				}
				break;
			default:
				panic("unrecognized option '-%c'", argv[i][1]);
			}
			continue;
		} else {
			if(filesp-files >= MAX-1)
				panic("too many input files");
			*filesp++ = argv[i];
		}
	}
	*filesp = 0;
	if(outname == NULL)
		outname = "a.out";
	for(filesp = files; *filesp; filesp++) {
		src.fp = fopen(*filesp, "r");
		if(src.fp == NULL)
			panic("cannot open %s for reading", *filesp);
		src.name = *filesp;
		src.line = src.col = 1;
		compile();
	}
	return 0;
}
