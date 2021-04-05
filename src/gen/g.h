#include "../u.h"
#include "../par/p.h"

enum {
	NOREG,
	AX,
	BX,
	CX,
	DX,
	SI,
	DI,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15,
	SP,
	BP,
	IP,
};

enum {
	IADDB,
	IADDL,
	IADDQ,
	IADDW,
	IJMP,
	IJNZ,
	IJZ,
	ILABEL,
	IMOVB,
	IMOVL,
	IMOVQ,
	IMOVW,
	ITEST,
	IXOR,
};

typedef struct Adr Adr;
typedef struct Prog Prog;

enum {
	ALAB,
	AOFF,
	AREG,
	AIMM,
};

struct Adr {
	int	atype;
	char	*label;
	int	offset;
	int	reg;
	int	regw;
	int	imm;
};

struct Prog {
	int	op;
	Adr	*to;
	Adr	*from;
	Prog	*prev;
	Prog	*next;
};

/*
 *	gen.c
 */
void	load(Node *);
void	gen(Node *);

/*
 *	tab.c
 */
char	*rtab(int, int);

/*
 *	sub.c
 */
void	ins(int, Adr *, Adr *);
void	areg(Node *);
void	freg(Node *);
Adr	*adr(void);
Adr	*ladr(char *);
Adr	*oadr(int);
Adr	*radr(int, int);
Adr	*iadr(int);
Adr	*nadr(Node *);
void	prins(Prog *);
void	pradr(Adr *);

EXTERN	Prog	*prog;
EXTERN	int	reg[R15];
EXTERN	char	*otab[];
