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

typedef	enum {
	SC_NONE,
	SC_CHAR,
	SC_INT,
	SC_FLOAT,
	SC_STR,
	SC_VOID,

	SC_OPTIONAL = 0x80		// marker for optional parameters. (e.g. 3rd parameter for "locate")
} SCLASS;

/* Table record for variable */
typedef	struct	symtbl {
	char	name[LABEL_LEN];
	SCLASS	class;
	int	dim;		/* 変数の次元		*/
	int	size[4];	/* 配列は３次元まで (1-origin, max=4 */
}	SYMTBL;

/* Table record for functions */
typedef	struct	fnctbl {
	char	name[LABEL_LEN];
	SCLASS	retClass;		/* return type */
	int		pars;			/* num of parameters */
	SCLASS	parClass[10];
	bool	isDefined;
}	FNCTBL;

/* Table record for labels */
typedef struct	lbltbl {
	char	name[LABEL_LEN];
	bool	isDefined;
}	LBLTBL;

/* Regular Statements Definition Table */
typedef struct defstatement {
	char	*b_name;		/* keyword in basic */
	char	*c_name;		/* nale in c language */
	int		numParams;
	SCLASS	retClass;		/* return type (reserve) */
	SCLASS	param1;
	SCLASS	param2;
	SCLASS	param3;
	SCLASS	param4;
} DEF_STATEMENT;

#define DEF_FUNC0(BNAME, CNAME, R)			{ BNAME, CNAME, 0, R, SC_NONE, SC_NONE, SC_NONE, SC_NONE }
#define DEF_FUNC1(BNAME, CNAME, R, P1)		{ BNAME, CNAME, 1, R, P1, SC_NONE,SC_NONE, SC_NONE }
#define DEF_FUNC2(BNAME, CNAME, R, P1, P2)	{ BNAME, CNAME, 2, R, P1, P2, SC_NONE, SC_NONE }
#define DEF_FUNC3(BNAME, CNAME, R, P1, P2, P3)	{ BNAME, CNAME, 3, R, P1, P2, P3, SC_NONE }

#define STATEMENT0(BNAME, CNAME)			{ BNAME, CNAME, 0, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE }
#define STATEMENT1(BNAME, CNAME, P1)		{ BNAME, CNAME, 1, SC_NONE, P1, SC_NONE,SC_NONE, SC_NONE }
#define STATEMENT2(BNAME, CNAME, P1, P2)	{ BNAME, CNAME, 2, SC_NONE, P1, P2, SC_NONE, SC_NONE }


#define	GLOBAL_TABLE_SIZE	100
#define	LOCAL_TABLE_SIZE	100
#define FUNC_TABLE_SIZE		400
#define	LABEL_TABLE_SIZE	10
// FIXME - was 1000
#define	STR_BUFFER_SIZE		10000
#define	STR_STACK_SIZE		20

/*	EXP.C	*/
volatile void	PutError(char* s, ...);
volatile void	PutErrorE(char* s, ...);
// FIXME void	RuntimeErr(char* s, int ptr);
volatile void	SynErr(void);
void	check(char* s);
void	fcheck(char* s);
void	factor(void);
void	RefVariable(int isGlobal, SYMTBL* p);
void	DoIndexed(SYMTBL* p);
void	RefProp(void);
void	PutPropSet(char*, char*, bool, char*, int);
void	PutPropSet2(char*, char*, bool, char*);
void	expression1(void);
SCLASS	ClassConvert(SCLASS class1, SCLASS class2);
void	ToInt1(SCLASS class);
void	ToInt2(SCLASS class1, SCLASS class2);
void	ToFloat1(SCLASS class);
void	ToFloat2(SCLASS class1, SCLASS class2);
void	ToStr1(SCLASS class);
void	doCast(SCLASS class);
void	expression2(void);
void	expression3(void);
void	expression4(void);
void	expression5(void);
void	expression6(void);
void	expression7(void);
void	expression8(void);
void	expression9(void);
void	expression(void);
SCLASS	doString(void);
void	SkipSpace(void);
int		amatch(char* s);
int		famatch(char* s);
int		pamatch(char* s);
void	ungetNewLine(char* s);
int		GetNewLine(void);
int		aconst(void);
void	GetOctNum(void);
void	GetBinNum(void);
void	GetHexNum(void);
void	GetChrNum(void);
void	GetDecNum(void);
// void	linList(int);
// void	PutDirectBuff(void);
// void	PutStyleData(void**, int);
SCLASS	ClassConvertS(SCLASS, SCLASS);
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
void	RecovLabels(int, int, int, int);
void	doContinue(void);
void	doBreak(void);
void	doRepeat(void);
void	doFor(void);
void	doPrint(void);
void	doUsing(void);
int		endOfLine(void);
void	doReturn(void);
void	doSwitch(void);
void	doCase(void);
void	doDefault(void);
void	DeclareVar(int, SCLASS);
void	DeclareStr(int);
int		GetConst(void);
void	DeclareArray(int);
void	InitArray(int, SCLASS, SYMTBL*);
void	SubArrayVar(int, SYMTBL*);
void	DeclareStrArray(int);
void	InitStrArray(int, SYMTBL*);
void	SubArrayStr(int, SYMTBL*);
int		parseFunction(void);

void	DoParam(FNCTBL*);
void	PrintType(SCLASS);
// FIXME void	AjustParam(void);
void	doLabel(void);
void	GetString(char*);
void	doGoto(void);
void	doInput(void);
void	doInput_Variable(SYMTBL* p, int isGlobal);
void	doLocate(void);

FNCTBL* pcall(void);

int		stateA(void);
int		stateB(void);
int		stateC(void);
int		stateD(void);
int		stateE(void);
int		stateF(void);
int		stateG(void);
int		stateH(void);
int		stateI(void);
int		stateJ(void);
int		stateK(void);
int		stateL(void);
int		stateM(void);
int		stateN(void);
int		stateO(void);
int		stateP(void);
int		stateQ(void);
int		stateR(void);
int		stateS(void);
int		stateT(void);
int		stateU(void);
int		stateV(void);
int		stateW(void);

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
void	Pass2(void);
char* TypeToStr(SCLASS type);
void	PrintFunctionPrototype(void);
void	PredecFunc(void);
void	SkipLineNumber(void);

/*	EFUNC.C		*/
int		efuncs(void);
int		funcD(void);
int		funcE(void);
int		funcF(void);
int		funcI(void);
int		funcL(void);
int		funcM(void);
int		funcO(void);
int		funcP(void);
int		funcQ(void);
int		funcR(void);
int		funcS(void);
int		funcT(void);
int		funcU(void);
int		funcV(void);

void	doEcvt(void);
void	doTransStr(int);
void	doTransStrFunc(int);
void	func0(SCLASS);
void	func1(SCLASS ret, SCLASS p1);
void	func2(SCLASS ret, SCLASS p1, SCLASS p2);
void	func3(SCLASS ret , SCLASS p1, SCLASS p2, SCLASS p3);
void	state0(SCLASS);
void	state1(SCLASS, SCLASS);
void	state2(SCLASS, SCLASS, SCLASS);
void	state3(SCLASS, SCLASS, SCLASS, SCLASS);
void	doFread(void);
int		ParseRegularFunctions(void);
int		ParseSpecialFunctions(void);
