CC 	= gcc
CFLAGS	= -Wall -Wextra
OBJ	= src/env.o \
	  src/lex.o \
	  src/util.o \
	  src/main.o \
	  src/cc.o
YACC	= yacc
YFLAGS	= -yd=src/y.tab.h

.c.o:	src/u.h
	$(CC) $(CFLAGS) -o $@ -c $<

cc:	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

.PHONY:	clean

clean:
	rm $(OBJ)	
