#include "u.h"

#define MAX 32

int main(int argc, char *argv[])
{
	int i;
	char *p, *bufp, buf[32];

	ge = emalloc(sizeof(Env));
	if(argc < 2)
		panic("usage: ecc [-options] files");
	for(i = 1; i < argc; i++) {
		/* todo - better option parsing */
		if(argv[i][0] == '-') {
			switch(argv[i][1]) {
			case 'o':
				bufp = buf;
				if(argv[i][2]) {
					p = argv[i][2];
					while(*p++) {
						if(bufp-buf < MAX-1)
							panic("filename after -o too long");
						*bufp++ = *p;
					}
					*bufp = 0;
					outfile = estrdup(buf);
				} else {
					if(i+1 >= argc)
						panic("missing filename after -o");
					outfile = argv[++i];
				}
				break;
			default:
				panic("unrecognized option '-%c'", argv[i][1]);
			}
			continue;
		}
		src.fp = fopen(argv[i], "r");
		if(!src.fp)
			panic("cannot open %s for reading", argv[i]);
		src.name = argv[i];
		src.line = src.col = 1;
		compile();
	}
	return 0;
}
