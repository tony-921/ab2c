/*
**	table.c	-- symbol table handler
**
*/


#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<setjmp.h>
#include	<stdarg.h>
#include	<math.h>

#include	"sxbasic.h"

extern	FILE* outputFp;
extern	bool	toSkipLineNumber;

SYMTBL		gloSymTbl[GLOBAL_TABLE_SIZE];
SYMTBL		locSymTbl[LOCAL_TABLE_SIZE];
FNCTBL		fncSymTbl[FUNC_TABLE_SIZE];
LBLTBL		lblSymTbl[LABEL_TABLE_SIZE];

int			gloSymPtr;
int			locSymPtr;
int			fncSymCnt;
int			lblSymPtr;

char*   ungetBuffer = NULL;		/* unget for getNextLine() */
char	prgBuff[1000];	/* Read Buff for Input */
char	preBuff[1000];	/* Write Buffer for pre-output */
char* TokenPtr;

extern	FILE* inputFp;
extern	FILE* outputFp;
extern	int		indent;

/*
 *  テーブルの初期化
 */
void
InitTable(void)
{
	gloSymPtr =
		fncSymCnt = 0;
	InitLocTbl();
}

/*
 *　変数や関数等の名前をセット
 */
void
SetName(char* p, char* q)
{
	int	cnt = LABEL_LEN;

	while (cnt-- && (isalnum(*q) || *q == '_')) {
		*p++ = *q++;
	}
	*p = '\0';
}

/*
 * ローカルテーブルの初期化
 */
void
InitLocTbl(void)
{
	lblSymPtr =
		locSymPtr = 0;
}

/*
 * グローバル変数を記憶する
 */
SYMTBL
* RegGloVar(char* s)
{
	if (gloSymPtr >= GLOBAL_TABLE_SIZE) {
		PutError("変数領域が足りません");
		return(NULL);
	}
	else {
		SetName(gloSymTbl[gloSymPtr].name, s);
		return	&gloSymTbl[gloSymPtr++];
	}
}

/*
 * ローカル変数を記憶する
 */
SYMTBL
* RegLocVar(char* s)
{
	if (locSymPtr >= LOCAL_TABLE_SIZE) {
		PutError("変数領域が足りません");
		return(NULL);
	}
	else {
		SetName(locSymTbl[locSymPtr].name, s);
		return	&locSymTbl[locSymPtr++];
	}
}

/*
 * Register a user function
 */
FNCTBL
* RegUserFunc(char* s)
{
	if (fncSymCnt >= FUNC_TABLE_SIZE) {
		PutError("Too many user functions");
		return(NULL);
	}
	else {
		SetName(fncSymTbl[fncSymCnt].name, s);
		return	&fncSymTbl[fncSymCnt++];
	}
}
/*
 * ラベルを記憶する
 */
LBLTBL
* RegLabel(char* s)
{
	if (lblSymPtr >= LABEL_TABLE_SIZE) {
		PutError("ラベル用領域が足りません");
		return(NULL);
	}
	else {
		SetName(lblSymTbl[lblSymPtr].name, s);
		return	&lblSymTbl[lblSymPtr++];
	}
}

/*
 *　グローバル変数を探す
 */
SYMTBL
* SearchGlo(char* s)
{
	int	i;
	size_t	len;

	len = TokenLen(s);
	if (len == 0) return(FALSE);
	for (i = 0; i < gloSymPtr; i++) {
		if (!strncmp(s, gloSymTbl[i].name, len)
			&& strlen(gloSymTbl[i].name) == len)
			return(&gloSymTbl[i]);
	}
	return(NULL);
}

/*
 * ローカル変数を探す
 */
SYMTBL
* SearchLoc(char* s)
{
	int	i;
	size_t	len;

	len = TokenLen(s);
	if (len == 0) return(FALSE);
	for (i = 0; i < locSymPtr; i++) {
		if (!strncmp(s, locSymTbl[i].name, len)
			&& strlen(locSymTbl[i].name) == len)
			return(&locSymTbl[i]);
	}
	return(NULL);
}

/*
 * ラベルを探す
 */
LBLTBL
* SearchLbl(char* s)
{
	int	i;
	size_t	len;

	len = TokenLen(s);
	if (len == 0) return(FALSE);
	for (i = 0; i < lblSymPtr; i++) {
		if (!strncmp(s, lblSymTbl[i].name, len)
			&& strlen(lblSymTbl[i].name) == len)
			return(&lblSymTbl[i]);
	}
	return(NULL);
}

/*
 * 関数名を探す
 */
FNCTBL
* SearchFunc(char* s)
{
	int	i;
	size_t	len;

	len = TokenLen(s);
	if (len == 0) return(FALSE);
	for (i = 0; i < fncSymCnt; i++) {
		if (!strncmp(s, fncSymTbl[i].name, len)
			&& strlen(fncSymTbl[i].name) == len)
			return(&fncSymTbl[i]);
	}
	return(NULL);
}

/*
 * 与られたトークンの長さを返す
 */
int
TokenLen(char* s)
{
	int	cnt = 0;

	while (isalnum(*s) || *s == '_') {
		s++;
		cnt++;
	}
	return (cnt);
}

/*
**  DefGloVar -- Define Global Variable into global symbol table
*/
SYMTBL
* DefGloVar(char* s)
{
	SYMTBL* p;

	p = SearchGlo(s);
	if (p)	PutError("既に同じ名前が使われています");
	else    p = RegGloVar(s);

	return(p);
}

/*
**  Define function
*/
FNCTBL
* DefFunc(char* s)
{
	FNCTBL* p;

	p = SearchFunc(s);
	if (p)	PutError("既に同じ名前が使われています");
	else 	p = RegUserFunc(s);

	return(p);
}

/*
**  defLocVar -- define local Variable into local symbol table
*/
SYMTBL
* DefLocVar(char* s)
{
	SYMTBL* p;

	p = SearchLoc(s);
	if (p)
		PutError("既に同じ名前が使われています");
	else
		p = RegLocVar(s);

	return(p);
}

/*
**  defLabel -- define Label into local symbol table
*/
LBLTBL
* DefLabel(char* s)
{
	LBLTBL* p;

	p = SearchLbl(s);
	if (p) {
		if (p->isDefined)
			PutError("Multipul Label Name: %s", p->name);
	}
	else {
		p = RegLabel(s);
	}
	p->isDefined = TRUE;
	return(p);
}

/*
**  whether there is undefined local label
*/
LBLTBL
* SearchUndefLabel(void)
{
	int	i;

	for (i = 0; i < lblSymPtr; i++)
		if (lblSymTbl[i].isDefined == FALSE) return(&lblSymTbl[i]);

	return(NULL);
}


/*
** Put code for predeclare of functions.
*/
void
Pass1(void)
{
	PutCode("#include\t<stdio.h>\n");
	PutCode("#include\t<string.h>\n");
	PutCode("#include\t<basic0.h>\n");
	PutCode("#include\t\"ab2c_run.h\"\n");

	rewind(inputFp);
	while (!feof(inputFp)) {
		int ret;
		ret = GetNewLine();
		if (amatch("func"))
			PredecFunc();
	}
	PrintFunctionPrototype();
}


/*
** 関数の前宣言を作成する
*/
void
PrintFunctionPrototype(void)
{
	int	i, j;
	FNCTBL* p;

	for (i = 0; i < fncSymCnt; i++) {
		p = &fncSymTbl[i];

		PutCode("%s\t%s(", TypeToStr(p->retType), p->name);
		if (p->pars == 0)	PutCode("void ");
		for (j = 0; j < p->pars; j++) {
			PrintType(p->parTypes[j]);
			if (j != p->pars - 1)	PutCode(",");
		}
		PutCode(");\n");
	}
}

char*
TypeToStr(E_TYPE type)
{
	if (type == ET_VOID)		return "void ";
	else if (type == ET_CHAR)	return "char";
	else if (type == ET_INT)	return "int";
	else if (type == ET_FLOAT)	return "double";
	else if (type == ET_STR)	return "char *";

	PutError("Undefined type %d", type);
}

/*
** 関数のパラメータを前もって確認
*/
void
PredecFunc(void)
{
	FNCTBL* p;
	E_TYPE	funcType;

	funcType = ET_INT;
	if (amatch("char"))		funcType = ET_CHAR;
	else if (amatch("int"))  funcType = ET_INT;
	else if (amatch("float"))funcType = ET_FLOAT;
	else if (amatch("str"))	funcType = ET_STR;

	SkipSpace();
	p = DefFunc(TokenPtr);
	TokenPtr += TokenLen(TokenPtr);

	p->retType = funcType;
	p->isDefined = FALSE;
	p->pars = 0;

	check("(");
	do {
		SkipSpace();
		if (!isalpha(*TokenPtr)) break;
		TokenPtr += TokenLen(TokenPtr);

		p->parTypes[p->pars] = ET_INT;
		if (amatch(";")) {
			if (amatch("int"));
			else if (amatch("char"))  p->parTypes[p->pars] = ET_CHAR;
			else if (amatch("float")) p->parTypes[p->pars] = ET_FLOAT;
			else if (amatch("str"))	 p->parTypes[p->pars] = ET_STR;
			else SynErr();
		}
		p->pars++;
	} while (amatch(","));
	check(")");
}

/*
** 一行読み込む
*/
int
GetNewLine(void)
{
	char* p;

	preBuff[0] = '\0';
	do {
		if (feof(inputFp) || fgets(prgBuff, sizeof(prgBuff), inputFp) == NULL) {
			prgBuff[0] = '\0';
			return(FALSE);
		}

		TokenPtr = prgBuff;
		if (toSkipLineNumber) {
			SkipLineNumber();
		}
		SkipSpace();
	} while (amatch("/*") || *TokenPtr == '\n');
	p = strstr(prgBuff, "\n");
	if (p) *p = '\0';

	return(TRUE);
}

void
SkipLineNumber(void)
{
	SkipSpace();
	while(isdigit(*TokenPtr)) {
		TokenPtr++;
	}
	SkipSpace();
}

