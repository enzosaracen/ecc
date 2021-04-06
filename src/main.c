#include "u.h"

#define MAX 32

FILE		*outfp;
int		bits;
int		lastclass;
Type		*lasttype;
char		*lastname;
Dstk		*declstk;
unsigned	block;
unsigned	nsue;
Src		src;

int main(int argc, char *argv[])
{
	int i;
	char *p, *bufp, **fsp, buf[MAX], *files[MAX], *outname;

	outname = NULL;
	fsp = files;
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
			if(fsp-files >= MAX-1)
				panic("too many input files");
			*fsp++ = argv[i];
		}
	}
	*fsp = 0;
	if(outname == NULL)
		outname = "a.out";
	outfp = fopen(outname, "w");
	if(outfp == NULL)
		panic("cannot open %s for writing", outname);
	for(fsp = files; *fsp; fsp++)
		compile(*fsp);
	return 0;
}
