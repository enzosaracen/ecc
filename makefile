CC 	= gcc
CFLAGS	= -Wall -Wextra -ggdb3 -std=c89
OBJ	= src/lex.o \
	  src/util.o \
	  src/ast.o \
	  src/decl.o \
	  src/amd64.o \
	  src/main.o

HFILES	= src/u.h \
	  src/y.tab.h

src/%.o:	src/%.c $(HFILES)
	$(CC) $(CFLAGS) -o $@ -c $<

ecc:	src/parse.o $(OBJ) $(HFILES)
	$(CC) $(CFLAGS) $(OBJ) src/parse.o -o $@

# this makefile is kind of messed up, if y.tab.h is removed yacc runs twice, need to fix
src/y.tab.h src/cc.o:	src/parse.y src/u.h
	yacc -vd src/parse.y
	mv -f y.tab.h y.tab.c src
	$(CC) $(CFLAGS) -o src/parse.o -c src/y.tab.c
	rm -f src/y.tab.c

.PHONY:	clean

clean:
	rm -f ecc $(OBJ) src/parse.o src/y.tab.c
