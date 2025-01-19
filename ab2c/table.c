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

void	PutFunctionName(char* name, char* event, char* index);

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


char*   ungetBuffer = NULL;		// unget for getNextLine()
char	prgBuff[1000];	/* Read Buff for Input */
char	preBuff[1000];	/* Write Buffer for pre-output */
char* TokenPtr;

extern	FILE* inputFp;
extern	FILE* outputFp;
extern	int		indent;

/*
 *  �e�[�u���̏�����
 */
void
InitTable(void)
{
	gloSymPtr =
		fncSymCnt = 0;
	InitLocTbl();
}

/*
 *�@�ϐ���֐����̖��O���Z�b�g
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
 * ���[�J���e�[�u���̏�����
 */
void
InitLocTbl(void)
{
	lblSymPtr =
		locSymPtr = 0;
}

/*
 * �O���[�o���ϐ����L������
 */
SYMTBL
* RegGloVar(char* s)
{
	if (gloSymPtr >= GLOBAL_TABLE_SIZE) {
		PutError("�ϐ��̈悪����܂���");
		return(NULL);
	}
	else {
		SetName(gloSymTbl[gloSymPtr].name, s);
		return	&gloSymTbl[gloSymPtr++];
	}
}

/*
 * ���[�J���ϐ����L������
 */
SYMTBL
* RegLocVar(char* s)
{
	if (locSymPtr >= LOCAL_TABLE_SIZE) {
		PutError("�ϐ��̈悪����܂���");
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
 * ���x�����L������
 */
LBLTBL
* RegLabel(char* s)
{
	if (lblSymPtr >= LABEL_TABLE_SIZE) {
		PutError("���x���p�̈悪����܂���");
		return(NULL);
	}
	else {
		SetName(lblSymTbl[lblSymPtr].name, s);
		return	&lblSymTbl[lblSymPtr++];
	}
}

/*
 *�@�O���[�o���ϐ���T��
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
 * ���[�J���ϐ���T��
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
 * ���x����T��
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
 * �֐�����T��
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
 * �^��ꂽ�g�[�N���̒�����Ԃ�
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
	if (p)	PutError("���ɓ������O���g���Ă��܂�");
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
	if (p)	PutError("���ɓ������O���g���Ă��܂�");
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
		PutError("���ɓ������O���g���Ă��܂�");
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
	// FIXME - Confirm the header files.
	PutCode("#include\t\"ab2c_run.h\"\n");
	PutCode("#include\t\"ab2c_var.h\"\n");

	rewind(inputFp);
	while (!feof(inputFp)) {
		int ret;
		ret = GetNewLine();
		if (amatch("func"))
			PredecFunc();
	}
	PrintPredec();
}

// FIXME -- Not needed anymore ?
#ifdef FIXME
/*
** Put code to arrange items in window engine.
*/
void
Pass2(void)
{
	int	eventmask;
	char	itemname[100];

	PutCode("void\n"
		"ab2c_Initialize(void )\n"
		"{\n");

	rewind(inputFp);

	while (!feof(inputFp)) {
		fgets(prgBuff, sizeof(prgBuff), inputFp);
		TokenPtr = prgBuff;
		SkipSpace();

		if (sscanf(prgBuff, "��SX-BASIC Version %hf", &version) == 1) {
			if (fabs(version - 0.70) > 0.001)
				PutError("Sorry, I can compile only version 0.70 sxb file.\n"
					"This file seems to be for version %4.2f.\n", version);
		}
		else if (amatch("��")) {
			p = strstr(prgBuff, "\n");
			if (p)	*p = '\0';
			PutCode("CreateItems(\"%s\");\n", prgBuff);
			sscanf(prgBuff, "��%d,%s (%d,%d,%d,%d),%d,%d,%x",
				&item, itemname, &d, &d, &d, &d, &d, &d, &eventmask);
			PutEventFunction(item, itemname, eventmask);
		}
	}
	PutCode("}\n");
}
#endif

void
PutFunctionName(char* name, char* event, char* index)
{
	char	buff[100];
	FNCTBL* p;

	sprintf(buff, "%s_%s", name, event);
	p = SearchFunc(buff);
	if (p) {
		fprintf(outputFp, "(*gp->lastItem)->%s = %s;\n", index, buff);
	}
}

/*
** �֐��̑O�錾���쐬����
*/
void
PrintPredec(void)
{
	int	i, j;
	FNCTBL* p;

	for (i = 0; i < fncSymCnt; i++) {
		p = &fncSymTbl[i];

		if (p->retClass == SC_VOID)		PutCode("void ");
		else if (p->retClass == SC_CHAR)	PutCode("char");
		else if (p->retClass == SC_INT)	PutCode("int");
		else if (p->retClass == SC_FLOAT)PutCode("double");
		else if (p->retClass == SC_STR)	PutCode("char *");

		PutCode("\t%s(", p->name);
		if (p->pars == 0)	PutCode("void ");
		for (j = 0; j < p->pars; j++) {
			PutClass(p->parClass[j]);
			if (j != p->pars - 1)	PutCode(",");
		}
		PutCode(");\n");
	}
}

/*
** �֐��̃p�����[�^��O�����Ċm�F
*/
void
PredecFunc(void)
{
	FNCTBL* p;
	SCLASS	funcClass;

	funcClass = SC_INT;
	if (amatch("char"))		funcClass = SC_CHAR;
	else if (amatch("int"))  funcClass = SC_INT;
	else if (amatch("float"))funcClass = SC_FLOAT;
	else if (amatch("str"))	funcClass = SC_STR;

	SkipSpace();
	p = DefFunc(TokenPtr);
	TokenPtr += TokenLen(TokenPtr);

	p->retClass = funcClass;
	p->isDefined = FALSE;
	p->pars = 0;

	check("(");
	do {
		SkipSpace();
		if (!isalpha(*TokenPtr)) break;
		TokenPtr += TokenLen(TokenPtr);

		p->parClass[p->pars] = SC_INT;
		if (amatch(";")) {
			if (amatch("int"));
			else if (amatch("char"))  p->parClass[p->pars] = SC_CHAR;
			else if (amatch("float")) p->parClass[p->pars] = SC_FLOAT;
			else if (amatch("str"))	 p->parClass[p->pars] = SC_STR;
			else SynErr();
		}
		p->pars++;
	} while (amatch(","));
	check(")");
}

void
ungetNewLine(char*s)
{
	ungetBuffer = s;
}

/*
** ��s�ǂݍ���
*/
int
GetNewLine(void)
{
	char* p;

	preBuff[0] = '\0';
	if (ungetBuffer != NULL) {
		TokenPtr = ungetBuffer;
		ungetBuffer = NULL;
	} else {
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
	}

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

