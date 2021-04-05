#include "../u.h"

char *flagstr = "o";

void usage(void)
{
	panic("usage: ecc [-%s] file", flagstr);
}

int main(int argc, char *argv[])
{
	int opt;

	while((opt = getopt(argc, argv, flagstr)) != -1)
		switch(opt) {
		case 'o':
			errorf("optind: %d", optind);
			break;
		default:
			usage();
			break;
		}
	if(optind != argc-1)
		usage();
	compile(argv[optind]);
	return 0;
}
