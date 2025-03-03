/*
	SX-BASIC Ver 0.5
	Programmed By ISHIGAMI Tatsuya
	05/10/94

*/
#define	TRUE	1
#define	FALSE	0

#define	MAX_BUFF_SIZE	256

/********************************************************/
/*                                                      */
/*  SYMBOL TABLE                                        */
/*                                                      */
/********************************************************/
typedef	char	bool;
#define	LABEL_LEN	30

/* Expression Type */
typedef	enum {
	ET_NONE,
	ET_CHAR,
	ET_INT,
	ET_FLOAT,
	ET_STR,
	ET_VOID,
} E_TYPE;

/* Table record for variable */
typedef	struct	symtbl {
	char	name[LABEL_LEN];
	E_TYPE	type;
	int	dim;		/* array dimention	*/
	int	size[4];	/* up to 3 dim (1-origin, max=4 */
} SYMTBL;

/* Table record for functions */
typedef	struct	fnctbl {
	char	name[LABEL_LEN];
	E_TYPE	retType;		/* return type */
	int		pars;			/* num of parameters */
	E_TYPE	parTypes[10];
	bool	isDefined;
} FNCTBL;

/* Table record for labels */
typedef struct	lbltbl {
	char	name[LABEL_LEN];
	bool	isDefined;
}	LBLTBL;

/* Regular Statement/Function Definition */
typedef struct deffunctions {
	char	*b_name;		/* keyword in basic */
	char	*c_name;		/* name in c language */
	int		numParams;
	E_TYPE	retType;		/* return type (function only) */
	E_TYPE	param1;
	E_TYPE	param2;
	E_TYPE	param3;
	E_TYPE	param4;
	E_TYPE	param5;
	E_TYPE	param6;
} DEF_FUNCTIONS;

#define DEF_FUNC0(BNAME, CNAME, R)					{ BNAME, CNAME, 0, R, ET_NONE, ET_NONE, ET_NONE, ET_NONE, ET_NONE, ET_NONE }
#define DEF_FUNC1(BNAME, CNAME, R, P1)				{ BNAME, CNAME, 1, R, P1, ET_NONE,ET_NONE, ET_NONE, ET_NONE, ET_NONE }
#define DEF_FUNC2(BNAME, CNAME, R, P1, P2)			{ BNAME, CNAME, 2, R, P1, P2, ET_NONE, ET_NONE, ET_NONE, ET_NONE }
#define DEF_FUNC3(BNAME, CNAME, R, P1, P2, P3)		{ BNAME, CNAME, 3, R, P1, P2, P3, ET_NONE, ET_NONE, ET_NONE }
#define DEF_FUNC4(BNAME, CNAME, R, P1, P2, P3, P4)	{ BNAME, CNAME, 4, R, P1, P2, P3, P4, ET_NONE, ET_NONE }
#define DEF_FUNC5(BNAME, CNAME, R, P1, P2, P3, P4, P5)	{ BNAME, CNAME, 5, R, P1, P2, P3, P4, P5, ET_NONE }
#define DEF_FUNC6(BNAME, CNAME, R, P1, P2, P3, P4, P5, P6)	{ BNAME, CNAME, 6, R, P1, P2, P3, P4, P5, P6 }


#define	GLOBAL_TABLE_SIZE	100
#define	LOCAL_TABLE_SIZE	100
#define FUNC_TABLE_SIZE		400
#define	LABEL_TABLE_SIZE	10
#define	STR_BUFFER_SIZE		1000
#define	STR_STACK_SIZE		20

/*	EXP.C	*/
volatile void	PutError(char* s, ...);
volatile void	SynErr(void);
void	check(char* s);
void	fcheck(char* s);
void	factor(void);
void	RefVariable(int isGlobal, SYMTBL* p);
void	doArrayIndex(SYMTBL* p);
void	expression1(void);
E_TYPE	PickAlignedType(E_TYPE type1, E_TYPE type2);
void	ToInt1(E_TYPE type);
void	ToInt2(E_TYPE type1, E_TYPE type2);
void	ToFloat1(E_TYPE type);
void	ToFloat2(E_TYPE type1, E_TYPE type2);
void	ToStr1(E_TYPE type);
void	doCast(E_TYPE type);
void	expression2(void);
void	expression3(void);
void	expression4(void);
void	expression5(void);
void	expression6(void);
void	expression7(void);
void	expression8(void);
void	expression9(void);
void	expression(void);
E_TYPE	doString(void);
void	SkipSpace(void);
int		amatch(char* s);
int		famatch(char* s);
int		pamatch(char* s);
int		GetNewLine(void);
int		aconst(void);
void	GetOctNum(void);
void	GetBinNum(void);
void	GetHexNum(void);
void	GetChrNum(void);
void	GetDecNum(void);
E_TYPE	PickAlignedTypeS(E_TYPE type1, E_TYPE type2);
char* strpop(void);
void	strpush(char*, ...);
void	sxb_strcat(char*, ...);
void	PutCode(char*, ...);

/* COMP.C */
int		Compile(void);
void	CompInit(void);
void	parseVariableDeclarations(int isGlobal);
void	parseStatement(void);
void	LoopStatement(char*);
void	doSubsutitute(int, SYMTBL*);
int		GetIndexValueSize(SYMTBL*);
void	doIf(void);
void	doWhile(void);
void	doContinue(void);
void	doBreak(void);
void	doRepeat(void);
void	doFor(void);
void	doPrint(void);
void	doUsing(void);
int		IsEndOfLine(void);
void	doReturn(void);
void	doSwitch(void);
void	doCase(void);
void	doDefault(void);
void	DeclareVariable(int, E_TYPE);
void	DeclareStr(int);
int		GetConst(void);
void	DeclareArray(int);
void	InitArray(int, E_TYPE, SYMTBL*);
void	SubArrayVar(int, SYMTBL*);
void	DeclareStrArray(int);
void	InitStrArray(int, SYMTBL*);
void	SubArrayStr(int, SYMTBL*);
int		parseFunction(void);

void	DoParam(FNCTBL*);
void	PrintType(E_TYPE);
void	doLabelDefinition(void);
void	GetString(char*);
void	doGoto(void);
void	doInput(void);
int		doInput_Prompt(void);
void	doInput_Variable(SYMTBL* p, int isGlobal);
void	doLocate(void);

FNCTBL* doFunctionCall(void);
int		ParseSpecialStatement(void);
void	doLinput(void);

/*	 TABLE.C	 */
void	InitTable(void);
void	InitLocTbl(void);
SYMTBL* RegGloVar(char*);
SYMTBL* RegLocVar(char*);
FNCTBL* RegUserFunc(char*);
LBLTBL* RegLabel(char*);
SYMTBL* SearchGlo(char*);
SYMTBL* SearchLoc(char*);
LBLTBL* SearchLbl(char*);
FNCTBL* SearchFunc(char*);
int		TokenLen(char*);
SYMTBL* DefGloVar(char*);
FNCTBL* DefFunc(char*);
SYMTBL* DefLocVar(char*);
LBLTBL* DefLabel(char*);
LBLTBL* SearchUndefLabel(void);
void	Pass1(void);
char* TypeToStr(E_TYPE type);
void	PrintFunctionPrototype(void);
void	PredecFunc(void);
void	SkipLineNumber(void);

/*	functions.C		*/
int		efuncs(void);
int		ParseSpecialFunctions(void);
void	doEcvt(void);
void	doTransStr(int);
void	doTransStrFunc(int);
void	func0(E_TYPE);
void	func1(E_TYPE ret, E_TYPE p1);
void	func2(E_TYPE ret, E_TYPE p1, E_TYPE p2);
void	func3(E_TYPE ret , E_TYPE p1, E_TYPE p2, E_TYPE p3);
void	func4(E_TYPE ret, E_TYPE p1, E_TYPE p2, E_TYPE p3, E_TYPE p4);
void	func5(E_TYPE ret, E_TYPE p1, E_TYPE p2, E_TYPE p3, E_TYPE p4, E_TYPE p5);
void	func6(E_TYPE ret, E_TYPE p1, E_TYPE p2, E_TYPE p3, E_TYPE p4, E_TYPE p5, E_TYPE p6);
void	doFread(void);

int		ParseRegularFunctions(bool forExpression, bool needReturnValue);

