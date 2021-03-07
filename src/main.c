#include "u.h"

#define MAX 32

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
	/*
	 * todo - only open outfile when need to b/c if an infile is unreadable, will create an empty outfile (or in any compiler error as well).
	 * or maybe just delete outfile in any error case, idk
	 */
	if(!outname)
		outname = "a.out";
	outfile = fopen(outname, "w");
	if(!outfile)
		panic("cannot open %s for writing", outname);
	*filesp = 0;
	for(filesp = files; *filesp; filesp++) {
		src.fp = fopen(*filesp, "r");
		if(!src.fp)
			panic("cannot open %s for reading", *filesp);
		src.name = *filesp;
		src.line = src.col = 1;
		compile();
	}
	return 0;
}
