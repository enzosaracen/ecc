CC 	= gcc
CFLAGS	= -Wall -Wextra
OBJ	= src/env.o \
	  src/lex.o \
	  src/util.o \
	  src/main.o

HFILES	= src/u.h \
	  src/y.tab.h

.c.o:	$(HFILES)
	$(CC) $(CFLAGS) -o $@ -c $<

cc:	src/cc.o $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) src/cc.o -o $@
	rm -f src/y.tab.h	# todo - organize this makefile so this command runs only when y.tab.h exists

src/y.tab.h src/cc.o:	src/cc.y src/u.h
	yacc -d src/cc.y
	mv -f y.tab.h y.tab.c src
	$(CC) $(CFLAGS) -o src/cc.o -c src/y.tab.c
	rm -f src/y.tab.c	# keep y.tab.h until fully compiled

.PHONY:	clean

clean:
	rm -f cc $(OBJ) src/cc.o
