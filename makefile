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

ecc:	src/parse.o $(OBJ) $(HFILES)
	$(CC) $(CFLAGS) $(OBJ) src/parse.o -o $@

src/y.tab.h src/parse.o:	src/parse.y src/u.h
	yacc -vd src/parse.y
	mv -f y.tab.h y.tab.c src
	$(CC) $(CFLAGS) -o src/parse.o -c src/y.tab.c
	rm -f src/y.tab.c

.PHONY:	clean

clean:
	rm -f ecc $(OBJ) src/parse.o src/y.tab.c
