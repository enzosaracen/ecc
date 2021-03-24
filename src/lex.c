#include "u.h"
#include "y.tab.h"

#define NHASH 1024
#define NLBUF 1024
#define NIO 1024
#define NOPEEK -2

char	peek;
char	oldpeek;
char	lbuf[NLBUF];
Sym	*syms[NHASH];
Io	*iostk[NIO];
int	nio;

struct {
	char *s;
	int lex;
} rsvd[] = {
	{"sizeof",	LSIZEOF},
	{"void",	LVOID},
	{"char",	LCHAR},
	{"short",	LSHORT},
	{"int",		LINT},
	{"long",	LLONG},
	{"float",	LFLOAT},
	{"double",	LDOUBLE},
	{"signed",	LSIGNED},
	{"unsigned",	LUNSIGNED},
	{"union",	LUNION},
	{"struct",	LSTRUCT},
	{"enum",	LENUM},
	{"if",		LIF},
	{"else",	LELSE},
	{"switch",	LSWITCH},
	{"case",	LCASE},
	{"default",	LDEFAULT},
	{"while",	LWHILE},
	{"do",		LDO},
	{"for",		LFOR},
	{"goto",	LGOTO},
	{"continue",	LCONTINUE},
	{"break",	LBREAK},
	{"return",	LRETURN},
	{"auto",	LAUTO},
	{"register",	LREGISTER},
	{"extern",	LEXTERN},
	{"static",	LSTATIC},
	{"typedef",	LTYPEDEF},
	{"const",	LCONST},
	{"volatile",	LVOLATILE},
	{0},
};

void lexinit(void)
{
	int i, j;
	Sym *s;

	peek = NOPEEK;
	oldpeek = NOPEEK;
	src.line = 1;
	for(i = 0; rsvd[i].s; i++) {
		for(j = 0; rsvd[i].s[j]; j++)
			lbuf[j] = rsvd[i].s[j];
		lbuf[j] = 0;
		s = lookup();
		s->lex = rsvd[i].lex;
	}
}

Sym *lookup(void)
{
	Sym *s;
	unsigned h;
	char c, *cp;

	cp = lbuf;
	h = 5381;
	while((c = *cp++))
		h = ((h << 5) + h) + c;
	h %= NHASH;
	c = lbuf[0];
	for(s = syms[h]; s; s = s->next) {
		if(s->name[0] != c)
			continue;
		if(strcmp(s->name, lbuf) == 0)
			return s;
	}
	s = emalloc(sizeof(Sym));
	s->name = estrdup(lbuf);
	lastname = s->name;
	s->next = syms[h];
	syms[h] = s;
	s->type = NULL;
	s->label = NULL;
	s->tag = NULL;
	s->class = CNONE;
	s->nsue = 0;
	s->block = 0;
	s->lex = LID;
	s->mac = NULL;
	return s;
}

void popio(void)
{
	if(nio < 0)
		errorf("cannot pop off iostack...");
	free(iostk[nio--]);
}

void pushio(FILE *fp, char *buf, int len)
{
	Io *i;

	if(nio >= NIO)
		errorf("macro depth limit reached");
	i = emalloc(sizeof(Io));
	i->fp = fp;
	i->buf = buf;
	i->len = len;
	i->p = i->buf;
	iostk[++nio] = i;
}

char next(void)
{
	char c;

	if(peek != NOPEEK) {
		c = peek;
		peek = NOPEEK;
		return c;
	} else if(iostk[nio]->fp == NULL) {
		if(*iostk[nio]->p == 0)
			goto pop;
	} else if(--iostk[nio]->len < 0) {
		iostk[nio]->len = fread(iostk[nio]->buf, 1, BUFSIZ, iostk[nio]->fp);
		if(iostk[nio]->len == -1) 
			panic("error reading file %s", src.name);
		else if(iostk[nio]->len == 0)
			goto pop;
		iostk[nio]->p = iostk[nio]->buf;
	}


	c = *iostk[nio]->p++;
	if(c == '\n')
		src.line++;
	return c;
pop:
	if(oldpeek != NOPEEK) {
		c = oldpeek;
		oldpeek = NOPEEK;
		return c;
	} else if(nio > 1) {
		popio();
		return next();
	} else
		return EOF;
}

void unget(char c)
{
	peek = c;
	if(c == '\n')
		src.line--;
}

void putbuf(char *cp, char c)
{
	if(cp-lbuf >= NLBUF-1)
		errorf("token too large");
	*cp = c;
}

int yylex(void)
{
	long v;
	int c;
	char c2, *cp;

start:
	while(isspace(c = next()));
	if(isdigit(c)) 
		goto lexnum;
	if(isalpha(c) || c == '_') {
		cp = lbuf;
		goto lexid;
	}
	switch(c) {
	case EOF:
		return 0;
	case '+':
		c2 = next();
		if(c2 == '=')
			return LADDAS;
		else if(c2 == '+')
			return LINC;
		break;
	case '-':
		c2 = next();
		if(c2 == '=')
			return LSUBAS;
		else if(c2 == '-')
			return LDEC;
		else if(c2 == '>')
			return LARROW;
		break;
	case '*':
		c2 = next();
		if(c2 == '=')
			return LMULAS;
		break;
	case '/':
		c2 = next();
		if(c2 == '=')
			return LDIVAS;
		else if(c2 == '*') {
			for(;;) {
				c = next();
				if(c == '*') {
					c = next();
					if(c == '/')
						goto start;
				}
				if(c == EOF)
					errorf("unterminated comment");
			}
		} else if(c2 == '/') {
			while((c = next()) != '\n')
				if(c == EOF)
					errorf("unterminated comment");
			goto start;
		}
		break;
	case '%':
		c2 = next();
		if(c2 == '=')
			return LMODAS;
		break;
	case '>':
		c2 = next();
		if(c2 == '=')
			return LGE;
		else if(c2 == '>') {
			c2 = next();
			if(c2 == '=')
				return LRSHAS;
			c = LRSH;
		}
		break;
	case '<':
		c2 = next();
		if(c2 == '=')
			return LLE;
		else if(c2 == '<') {
			c2 = next();
			if(c2 == '=')
				return LLSHAS;
			c = LLSH;
		}
		break;
	case '!':
		c2 = next();
		if(c2 == '=')
			return LNE;
		break;
	case '=':
		c2 = next();
		if(c2 == '=')
			return LEQ;
		break;
	case '&':
		c2 = next();
		if(c2 == '&')
			return LANDAND;
		else if(c2 == '=')
			return LANDAS;
		break;
	case '|':
		c2 = next();
		if(c2 == '|')
			return LOROR;
		else if(c2 == '=')
			return LORAS;
		break;
	case '^':
		c2 = next();
		if(c2 == '=')
			return LXORAS;
		break;
	case '"':
		cp = lbuf;
		while((c2 = next()) != '"') {
			if(c2 == EOF || c2 == '\n')
				errorf("unterminated string");
			putbuf(cp++, c2);
		}
		yylval.sval = estrdup(lbuf);
		return LSTRING;
		break;
	case '#':
		pp();
		goto start;
	default:
		return c;
	}
	unget(c2);
	return c; 
lexnum:
	v = 0;
	while(isdigit(c)) {
		v = v*10 + c-'0';
		c = next();
	}
	unget(c);
	yylval.lval = v;
	return LNUM;
lexid:
	while(isalnum(c) || c == '_')  {
		putbuf(cp++, c);
		c = next();
	}
	unget(c);
	*cp = 0;
	yylval.sym = lookup();
	if(yylval.sym->mac != NULL) {
		pushio(NULL, yylval.sym->mac, 0);
		if(peek != NOPEEK) {
			oldpeek = peek;
			peek = NOPEEK;
		}
		goto start;
	}
	if(yylval.sym->class == CTYPEDEF)
		yylval.sym->lex = LTYPE;
	return yylval.sym->lex;
}

void compile(char *file)
{
	FILE *fp;

	src.name = file;
	src.line = 0;
	fp = fopen(file, "r");
	if(fp == NULL)
		panic("cannot open %s for reading", file);
	pushio(fp, emalloc(BUFSIZ), 0);
	lexinit();
	yyparse();
}

/*
 *	preprocessor
 */
void pp(void)
{
	/* note - ppname will error on a null directive, so maybe fix,
	 * should probably not have ppname */
	ppname();
	if(strcmp(lbuf,	"include") == 0)
		errorf("unimp");
	else if(strcmp(lbuf, "define") == 0)
		ppdefine();
	else if(strcmp(lbuf, "undef") == 0)
		ppundef();
	else if(strcmp(lbuf, "if") == 0)
		errorf("unimp");
	else if(strcmp(lbuf, "elseif") == 0)
		errorf("unimp");
	else if(strcmp(lbuf, "else") == 0)
		errorf("unimp");
	else if(strcmp(lbuf, "endif") == 0)
		errorf("unimp");
	else
		errorf("invalid directive");
}

char ppnext(void)
{
	char c;

	c = next();
	if(c == '\\')
		if(next() != '\n')
			errorf("expected newline after backslash in macro definition");
	if(c == EOF)
		errorf("eof in macro definition");
	return c;
}

void ppname(void)
{
	char c, *cp;

	while(isspace(c = ppnext()))
		if(c == '\n')
			errorf("unexpected newline in macro definition");
	if(!isalpha(c) && c != '_')
		errorf("macro names must be identifiers");
	cp = lbuf;
	while(isalnum(c) || c == '_') {
		putbuf(cp++, c);
		c = ppnext();
	}
	*cp = 0;
	if(!isspace(c))
		errorf("whitespace required after macro name");
	unget(c);
}

void ppdefine(void)
{
	Sym *s;
	char c, *cp;

	ppname();
	s = lookup();
	if(s->mac != NULL)
		errorf("redefinition of macro %s", s->name);
	/* shouldn't limit macro size to size of lbuf, and have yet
	 * to implement macro functions, but this works for now... */
	cp = lbuf;
	while((c = ppnext()) != '\n')
		putbuf(cp++, c);
	*cp = 0;
	s->mac = estrdup(lbuf);
}

void ppundef(void)
{
	Sym *s;

	ppname();
	s = lookup();
	if(s->mac != NULL) {
		free(s->mac);
		s->mac = NULL;
	}
}
