CC 	= gcc
CFLAGS	= -Wall -Wextra -ggdb3
ECCO	= src/cmd/ecc.o \
	  src/util.o \
	  src/par/type.o \
	  src/par/lex.o \
	  src/par/par.o \
	  src/gen/gen.o \
	  src/gen/sub.o \
	  src/gen/tab.o

EVMO	= src/cmd/evm.o \
	  src/util.o

HFILES	= src/u.h \
	  src/par/y.tab.h \
	  src/gen/g.h \
	  src/evm/v.h

all:	ecc \
	evm

%.o:	%.c $(HFILES)
	$(CC) $(CFLAGS) -o $@ -c $<

ecc:	$(ECCO) $(HFILES)
	$(CC) $(CFLAGS) $(ECCO) -o $@

evm:	$(EVMO) $(HFILES)
	$(CC) $(CFLAGS) $(EVMO) -o $@

src/y.tab.h src/par/par.o:	src/par/par.y src/u.h
	yacc -vd src/par/par.y
	mv -f y.tab.h y.tab.c src/par/
	$(CC) $(CFLAGS) -o src/par/par.o -c src/par/y.tab.c
	rm -f src/par/y.tab.c

.PHONY:	clean

clean:
	rm -f ecc evm $(EVMO) $(ECCO) src/par/y.tab.c
