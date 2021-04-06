CC 	= gcc
CFLAGS	= -Wall -Wextra -ggdb3
OBJ	= src/lex.o \
	  src/util.o \
	  src/type.o \
	  src/main.o \
	  src/amd64/gen.o \
	  src/amd64/tab.o \
	  src/amd64/sub.o

HFILES	= src/u.h \
	  src/y.tab.h \
	  src/amd64/g.h

%.o:	%.c $(HFILES)
	$(CC) $(CFLAGS) -o $@ -c $<

ecc:	src/par.o $(OBJ) $(HFILES)
	$(CC) $(CFLAGS) $(OBJ) src/par.o -o $@

src/y.tab.h src/par.o:	src/par.y src/u.h
	yacc -vd src/par.y
	mv -f y.tab.h y.tab.c src
	$(CC) $(CFLAGS) -o src/par.o -c src/y.tab.c
	rm -f src/y.tab.c

.PHONY:	clean

clean:
	rm -f ecc $(OBJ) src/par.o src/y.tab.c
