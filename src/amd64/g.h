#include "../u.h"

enum {
	AX	= 1<<0,
	BX	= 1<<1,
	CX	= 1<<2,
	DX	= 1<<3,
	SI	= 1<<4,
	DI	= 1<<5,
	R8	= 1<<6,
	R9	= 1<<7,
	R10	= 1<<8,
	R11	= 1<<9,
	R12	= 1<<10,
	R13	= 1<<11,
	R14	= 1<<12,
	R15	= 1<<13,
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

extern	char	*otab[];
extern	Prog	*prog;
extern	int	reg;
