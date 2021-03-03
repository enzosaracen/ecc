#include "u.h"

int main(int argc, char *argv[])
{
	int i;

	ge = emalloc(sizeof(Env));
	if(argc < 2)
		panic("usage: ecc file...");
	for(i = 1; i < argc; i++) {
		src.fp = fopen(argv[i], "r");
		if(!src.fp)
			panic("cannot open %s for reading", argv[i]);
		src.path = argv[i];
		src.line = src.col = 1;
		compile();
	}
	return 0;
}
