CC 	= gcc
CFLAGS	= -Wall -Wextra -g -std=c89
OBJ	= src/lex.o \
	  src/util.o \
	  src/ast.o \
	  src/decl.o \
	  src/fold.o \
	  src/amd64.o \
	  src/main.o

HFILES	= src/u.h \
	  src/y.tab.h

src/%.o:	src/%.c $(HFILES)
	$(CC) $(CFLAGS) -o $@ -c $<

ecc:	src/cc.o $(OBJ) $(HFILES)
	$(CC) $(CFLAGS) $(OBJ) src/cc.o -o $@

src/y.tab.h src/cc.o:	src/cc.y src/u.h
	yacc -d src/cc.y
	mv -f y.tab.h y.tab.c src
	$(CC) $(CFLAGS) -o src/cc.o -c src/y.tab.c
	rm -f src/y.tab.c	# keep y.tab.h so dont have to run yacc each make

.PHONY:	clean

clean:
	rm -f ecc $(OBJ) src/cc.o src/y.tab.c
