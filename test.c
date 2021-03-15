typedef struct Src {
 int line, col;
 char *name;
} Src;

typedef struct Sym Sym;
struct Sym {
 int lexical;
 char *name;
 Sym *next;
 unsigned block;
};

void *emalloc(int);
char *estrdup(char*);
void panic(char *, ...);
void errorposf(char *, ...);

void lexinit(void);
Sym *lookup(void);
int yylex(void);
void compile(void);

void yyerror(char *s);

Src src;
FILE *outfile;
char symb[1024];
Sym *hash[1024];
  enum yytokentype
  {
    LPE = 258,
    LME = 259,
    LMLE = 260,
    LDVE = 261,
    LMDE = 262,
    LLSHE = 263,
    LRSHE = 264,
    LANDE = 265,
    LXORE = 266,
    LORE = 267,
    LOROR = 268,
    LANDAND = 269,
    LEQ = 270,
    LNE = 271,
    LLE = 272,
    LGE = 273,
    LLSH = 274,
    LRSH = 275,
    LARROW = 276,
    LPP = 277,
    LMM = 278,
    LSIZEOF = 279,
    LVOID = 280,
    LCHAR = 281,
    LSHORT = 282,
    LINT = 283,
    LLONG = 284,
    LFLOAT = 285,
    LDOUBLE = 286,
    LSIGNED = 287,
    LUNSIGNED = 288,
    LUNION = 289,
    LSTRUCT = 290,
    LENUM = 291,
    LTYPE = 292,
    LID = 293,
    LSTRING = 294,
    LNUM = 295,
    LELLIPSES = 296,
    LIF = 297,
    LELSE = 298,
    LSWITCH = 299,
    LCASE = 300,
    LDEFAULT = 301,
    LWHILE = 302,
    LDO = 303,
    LFOR = 304,
    LGOTO = 305,
    LCONTINUE = 306,
    LBREAK = 307,
    LRETURN = 308,
    LAUTO = 309,
    LREGISTER = 310,
    LEXTERN = 311,
    LSTATIC = 312,
    LTYPEDEF = 313,
    LCONST = 314,
    LVOLATILE = 315
  };
union YYSTYPE
{

  Sym *sym;

};
typedef union YYSTYPE YYSTYPE;





extern YYSTYPE yylval;

int yyparse (void);



char peek;
struct {
 char b[
       8192
             ], *p;
 int c;
} fin;

struct {
 char *s;
 int lexical;
} rsvd[] = {
 {"sizeof", 279},
 {"void", 280},
 {"char", 281},
 {"short", 282},
 {"int", 283},
 {"long", 284},
 {"float", 285},
 {"double", 286},
 {"signed", 287},
 {"unsigned", 288},
 {"union", 289},
 {"struct", 290},
 {"enum", 291},
 {"if", 297},
 {"else", 298},
 {"switch", 299},
 {"case", 300},
 {"default", 301},
 {"while", 302},
 {"do", 303},
 {"for", 304},
 {"goto", 305},
 {"continue", 306},
 {"break", 307},
 {"return", 308},
 {"auto", 309},
 {"register", 310},
 {"extern", 311},
 {"static", 312},
 {"typedef", 313},
 {"const", 314},
 {"volatile", 315},
 {0},
};

void lexinit(void)
{
 int i, j;
 Sym *s;

 peek = -2;
 for(i = 0; rsvd[i].s; i++) {
  for(j = 0; rsvd[i].s[j]; j++)
   symb[j] = rsvd[i].s[j];
  symb[j] = 0;
  s = lookup();
  s->lexical = rsvd[i].lexical;
 }
}

Sym *lookup(void)
{
 Sym *s;
 int c;
 unsigned h;
 char *cp;

 cp = symb;
 h = 5381;
 while((c = *cp++))
  h = ((h << 5) + h) + c;
 h %= 1024;

 for(s = hash[h]; s; s = s->next)
  if(strcmp(s->name, symb) == 0)
   return s;
 s = emalloc(sizeof(Sym));
 s->name = estrdup(symb);
 s->next = hash[h];
 hash[h] = s;
 s->lexical = 293;
 return s;
}

char next(void)
{
 char c;

 if(--fin.c <= 0) {
  fin.c = fread(fin.b, 1, 
                         8192
                               , src.fp);
  if(fin.c == -1)
   panic("error reading file %s", src.name);
  else if(fin.c == 0)
   return 
         (-1)
            ;
  fin.p = fin.b;
 }
 c = *fin.p++;
 if(c == '\n') {
  src.line++;
  src.col = 1;
 } else if(
          ((*__ctype_b_loc ())[(int) ((
          c
          ))] & (unsigned short int) _ISprint)
                    )
  src.col++;
 return c;
}

int yylex(void)
{
 int i;
 int c;
 char c2, *cp;

 if(peek != -2) {
  c = peek;
  peek = -2;
 } else
  c = next();

 for(; 
      ((*__ctype_b_loc ())[(int) ((
      c
      ))] & (unsigned short int) _ISspace)
                ; c = next());
 if(
   ((*__ctype_b_loc ())[(int) ((
   c
   ))] & (unsigned short int) _ISdigit)
             )
  goto LEXNUM;
 if(
   ((*__ctype_b_loc ())[(int) ((
   c
   ))] & (unsigned short int) _ISalpha)
             ) {
  cp = symb;
  goto LEXID;
 }
 switch(c) {
 case 
     (-1)
        :
  return 0;
 case '+':
  c2 = next();
  if(c2 == '=')
   return 258;
  else if(c2 == '+')
   return 277;
  break;
 case '-':
  c2 = next();
  if(c2 == '=')
   return 259;
  else if(c2 == '-')
   return 278;
  else if(c2 == '>')
   return 276;
  break;
 case '*':
  c2 = next();
  if(c2 == '=')
   return 260;
  break;
 case '/':
  c2 = next();
  if(c2 == '=')
   return 261;
  break;
 case '%':
  c2 = next();
  if(c2 == '=')
   return 262;
  break;
 case '>':
  c2 = next();
  if(c2 == '=')
   return 273;
  else if(c2 == '>') {
   c2 = next();
   if(c2 == '=')
    return 264;
   c = 275;
  }
  break;
 case '<':
  c2 = next();
  if(c2 == '=')
   return 272;
  else if(c2 == '<') {
   c2 = next();
   if(c2 == '=')
    return 263;
   c = 274;
  }
  break;
 case '!':
  c2 = next();
  if(c2 == '=')
   return 271;
  break;
 case '=':
  c2 = next();
  if(c2 == '=')
   return 270;
  break;
 case '&':
  c2 = next();
  if(c2 == '&')
   return 269;
  else if(c2 == '=')
   return 265;
  break;
 case '|':
  c2 = next();
  if(c2 == '|')
   return 268;
  else if(c2 == '=')
   return 267;
  break;
 case '^':
  c2 = next();
  if(c2 == '=')
   return 266;
  break;
 case '.':
  c2 = next();
  if(c2 == '.') {
   if(next() != '.')
    errorposf("expected '...'");
   return 296;
  }
  break;
 case '"':
  while((c2 = next()) != '"');
  return 294;
  break;
 default:
  return c;
 peek = c2;
 return c;
 }
LEXNUM:
 i = 0;
 while(
      ((*__ctype_b_loc ())[(int) ((
      c
      ))] & (unsigned short int) _ISdigit)
                ) {
  i = i*10 + c-'0';
  c = next();
 }
 peek = c;
 return 295;
LEXID:
 while(
      ((*__ctype_b_loc ())[(int) ((
      c
      ))] & (unsigned short int) _ISalnum) 
                 || c == '_') {
  *cp++ = c;
  c = next();
 }
 peek = c;
 *cp = 0;
 yylval.sym = lookup();
 return yylval.sym->lexical;
}

void compile(void)
{
 lexinit();
 yyparse();
}
