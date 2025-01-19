/*
	SX-BASIC	�R���p�C���{��
	Programmed By ISHIGAMI Tatsuya
	08/26/95

*/

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<setjmp.h>

#include	"sxbasic.h"


char* TokenPtr;
SCLASS	casClass;			/* swict���Ŏg��ꂽ�ϐ��̌^ */
SCLASS	retClass;			/* �֐��̖߂�l�̃^�C�v */

int		indent = 0;			/* Indent depth of Output File */
bool	inFunction;			/* �֐��̒��� */
bool	isFirstFunc;		/* �ŏ��̊֐�		*/
int		brkLabel;
int		contLabel;
int		caseLabel;
int		dfltLabel;


extern	int		vali;
extern	SCLASS	lastClass;

extern	FILE* inputFp, * outputFp;
extern	char	prgBuff[];

extern int	gloSymPtr;
extern int	locSymPtr;
extern int	fncSymCnt;
extern int	lblSymPtr;

extern	char	strBuffer[];
extern	char* strStackPtr;

int
Compile(void)
{
	LBLTBL* p;

	CompInit();
	parseVariableDeclarations(TRUE);
	PutCode("\n");
	PutCode("void _sxb_start(void) {");
	indent++;
	do {
		PutCode("\n");
		do {
			parseStatement();
		} while (amatch(":") && !endOfLine());
		if (!endOfLine()) SynErr();
	} while (GetNewLine());

	if (isFirstFunc) {	/* �֐�������Ȃ������� */
		if (p = SearchUndefLabel()) {
			PutError("���錾�̃��x���ł�");
			PutErrorE("Undefined label");
		}
		PutCode("\n};");
	}
	if (inFunction) {
		PutError("�֐��̓r���Ńv���O�������I���Ă��܂�");
		PutErrorE("Incomplete function");
	}
	return(TRUE);
}

/*
** Initialize the Compiler
*/
void
CompInit(void)
{
	InitTable();
	TokenPtr = prgBuff;

	printf("pass 1\n");
	Pass1();		/* Parse code to generate function table, labels (withoug generating code) */
	printf("pass 2\n");
	// Pass2();		/* put code to arrange items */

	rewind(inputFp);

	dfltLabel = caseLabel =
		isFirstFunc = TRUE;
	inFunction = FALSE;
	strStackPtr = strBuffer;
	strBuffer[0] = '\0';
}

/*
** Parse Variable Declarations
*/
void
parseVariableDeclarations(int isGlobal)
{
	while (GetNewLine()) {
		do {
			if (amatch("dim"))	 DeclareArray(isGlobal);
			else if (famatch("int ") || famatch("int\t"))
				DeclareVar(isGlobal, SC_INT);
			else if (famatch("char ") || famatch("char\t"))
				DeclareVar(isGlobal, SC_CHAR);
			else if (famatch("float ") || famatch("float\t"))
				DeclareVar(isGlobal, SC_FLOAT);
			else if (amatch("str ") || amatch("str\t")) {
				PutCode("char\t");
				DeclareStr(isGlobal);
			}
			else {
//				ungetNewLine(TokenPtr);
				return;
			}
			PutCode(";\n");
		} while (amatch(":"));
	}
}

/*
** Parse satements
*/
void
parseStatement(void)
{
	SYMTBL* p;
	char* nextToken;
	int		ret = FALSE;

	SkipSpace();
	ret = func_external();
	if (ret) return TRUE;

	switch (*TokenPtr) {
	case 'a':
		ret = stateA();
		break;
	case 'b':
		ret = stateB();
		break;
	case 'c':
		ret = stateC();
		break;
	case 'd':
		ret = stateD();
		break;
	case 'e':
		ret = stateE();
		break;
	case 'f':
		ret = stateF();
		break;
	case 'g':
		ret = stateG();
		break;
	case 'h':
		ret = stateH();
		break;
	case 'i':
		ret = stateI();
		break;
	case 'j':
		ret = stateJ();
		break;
	case 'k':
		ret = stateK();
		break;
	case 'l':
		ret = stateL();
		break;
	case 'm':
		ret = stateM();
		break;
	case 'n':
		ret = stateN();
		break;
	case 'o':
		ret = stateO();
		break;
	case 'p':
		ret = stateP();
		break;
	case 'q':
		ret = stateQ();
		break;
	case 'r':
		ret = stateR();
		break;
	case 's':
		ret = stateS();
		break;
	case 't':
		ret = stateT();
		break;
	case 'u':
		ret = stateU();
		break;
	case 'v':
		ret = stateV();
		break;
	case 'w':
		ret = stateW();
		break;
	case 'x':
		ret = stateX();
		break;
	case 'y':
		ret = stateY();
		break;
	case 'z':
		ret = stateZ();
		break;
	case '?':
		TokenPtr++;
		doPrint();
		ret = TRUE;
		break;
	}
	if (ret)	return;

	if (*TokenPtr == '{') {
		TokenPtr++;
		LoopStatement("}");
		return;
	}
	else if (isalpha(*TokenPtr)) {
		nextToken = &TokenPtr[TokenLen(TokenPtr)];
		if (*nextToken == ':')
			doLabel();
		else if (p = SearchLoc(TokenPtr))
			doSubsutitute(FALSE, p);
		else if (p = SearchGlo(TokenPtr))
			doSubsutitute(TRUE, p);
		else if (*nextToken == '(') {
			pcall();
			PutCode("%s", strpop());
		}
//		FIXME - remove
//		else if (*nextToken == '.' || *nextToken == '[')
//			doProp();
		else
			PutError("Undefined Variable Error");
	}
	else	SynErr();

	PutCode(";");
}

int
stateA(void)
{
	if (amatch("alart(")) {
		strpush("_sxb_alart(");
		state2(SC_INT, SC_STR, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
stateB(void)
{
	if (amatch("break"))		doBreak();
	else if (amatch("beep")) {
		PutCode("DMBeep(1);");
	}
	else
		return(FALSE);

	return(TRUE);
}

int
stateC(void)
{
	if (amatch("continue"))		doContinue();
	else if (amatch("case"))	doCase();
	else if (amatch("cls")) {
		PutCode("_sxb_cls(); // FIXME");
	} else {
		return(FALSE);
	}

	return(TRUE);
}

int
stateD(void)
{
	if (amatch("default")) 			doDefault();
	else if (amatch("di()"));
	else if (amatch("decross()"))	PutCode("EMDeCross();");
	else
		return(FALSE);

	return(TRUE);
}

int
stateE(void)
{
	if (amatch("endfunc"))		PutError("endfunc without func");
	else if (amatch("endswitch"))PutError("endswitch without switch");
	else if (amatch("endwhile"))	PutError("endwhile without while");
	else if (amatch("endtask(")) state1(SC_INT, SC_INT);
	else if (amatch("end"))		PutCode("_sxb_end();");
	else if (amatch("ei()"));
	else if (amatch("encross()")) 	PutCode("EMEnCross();");
	else if (amatch("exit()")) {
		PutCode("_sxb_exit(1);");
	}
	else
		return(FALSE);

	return(TRUE);
}

int
stateF(void)
{
	if (amatch("for")) {
		doFor();
	}
	else if (amatch("func")) {
		parseFunction();
	}
	else if (amatch("fclose(")) {
		strpush("_sxb_fclose(");
		state1(SC_INT, SC_INT);
	}
	else if (amatch("fcloseall(")) {
		strpush("_sxb_fcloseall(");
		state0(SC_INT);
	}
	else if (amatch("fdelete(")) {
		strpush("_sxb_fdelete(");
		state1(SC_STR, SC_INT);
	}
	else if (amatch("fputc(")) {
		strpush("_sxb_fputc(");
		state2(SC_CHAR, SC_INT, SC_INT);
	}
	else if (amatch("frename(")) {
		strpush("_sxb_frename(");
		state2(SC_STR, SC_STR, SC_INT);
	}
	else if (amatch("fseek(")) {
		strpush("_sxb_fseek(");
		state3(SC_INT, SC_INT, SC_INT, SC_INT);
	}
	else if (amatch("fread(")) {
		strpush("_sxb_fread(");
		doFread();
		PutCode("%s;", strpop());
	}
	else if (amatch("fwrite(")) {
		strpush("_sxb_fwrite(");
		doFread();
		PutCode("%s;", strpop());
	}
	else if (amatch("freads(")) {
		strpush("_sxb_freads(");
		doTransStr(1);
		PutCode("%s;", strpop());
	}
	else if (amatch("fwrites(")) {
		strpush("_sxb_fwrites(");
		state2(SC_STR, SC_INT, SC_INT);
	}
	else if (pamatch("fock(")) {
		state1(SC_STR, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
stateG(void)
{
	if (amatch("goto"))			doGoto();
	else
		return(FALSE);

	return(TRUE);
}

int
stateH(void)
{
	return(FALSE);
}

int
stateI(void)
{
	if (amatch("if")) {
		doIf();
	}
	else if (amatch("input")) {
		doInput();
	}
	else if (pamatch("iocs(")) {
		state1(SC_INT, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

/*
** LINPUT�̏���
*/
void
doLinput(void)
{
	SYMTBL* p;
	int	isGlobal;

	SkipSpace();
	expression();
	if (lastClass == SC_NONE) {
		printf("FIXME - No prompt string");
	} else {
		ToStr1(lastClass);
		check(";");

	}

	if (p = SearchLoc(TokenPtr)) {
		isGlobal = FALSE;
	} else if (p = SearchGlo(TokenPtr)) {
		isGlobal = TRUE;
	} else {
		PutError("���錾�̕ϐ��ł�");
	}

	if (p->class != SC_STR)	PutError("Invalid Variable Type");
	// FIXME - Support variable string variable
	if (p->dim != 1) PutError("Cannot use index variable here.");
	TokenPtr += TokenLen(TokenPtr);
	PutCode("scanf(\"\%s\", &%s);\n", "%s", p->name);

#ifdef FIXME
	// FIXME - Support referencing an indexed array variable.
	if (p->dim >= 2)	doIndex(p);

	if (isGlobal)	PutArrayCode(0x9c, p);
	else		PutArrayCode(0x9b, p);
#endif
}

int
stateJ(void)
{
	return(FALSE);
}

int
stateK(void)
{
	return(FALSE);
}

int
stateL(void)
{
	if (amatch("linput")) {
		doLinput();
	} else if (amatch("locate")) {
		doLocate();
	} else
		return(FALSE);

	return(TRUE);
}

int
stateM(void)
{
	return(FALSE);
}

int
stateN(void)
{
	if (amatch("next"))		PutError("Missing \'for\' keyword before \'next\'");
	else
		return(FALSE);

	return(TRUE);
}

int
stateO(void)
{
	if (amatch("openres(")) {
		strpush("_sxb_openres(");
		state1(SC_STR, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
stateP(void)
{
	if (amatch("print")) {
		doPrint();
	}
	else if (pamatch("poke(")) {
		state2(SC_INT, SC_INT, SC_INT);
	}
	else if (pamatch("pokew(")) {
		state2(SC_INT, SC_INT, SC_INT);
	}
	else if (pamatch("pokel(")) {
		state2(SC_INT, SC_INT, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
stateQ(void)
{
	return(FALSE);
}

int
stateR(void)
{
	if (amatch("repeat"))		doRepeat();
	else if (amatch("return"))	doReturn();
	else if (pamatch("randomize(")) {
		state1(SC_INT, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
stateS(void)
{
	if (amatch("switch")) {
		doSwitch();
	}
	else if (pamatch("strlwr(")) {
		doTransStr(0);
		PutCode("%s;", strpop());
	}
	else if (pamatch("strnset(")) {
		doTransStr(2);
		PutCode("%s;", strpop());
	}
	else if (pamatch("strrev(")) {
		doTransStr(0);
		PutCode(";%s", strpop());
	}
	else if (pamatch("strset(")) {
		doTransStr(1);
		PutCode("%s;", strpop());
	}
	else if (pamatch("strupr(")) {
		doTransStr(0);
		PutCode("%s;", strpop());
	}
	else if (pamatch("srand(")) {
		state1(SC_INT, SC_INT);
	}
	else if (amatch("sendmes(")) {
		strpush("_sxb_sendmes(");
		state2(SC_INT, SC_STR, SC_INT);
	}
	else if (amatch("setenv(")) {
		strpush("_sxb_setenv(");
		state3(SC_STR, SC_INT, SC_STR, SC_INT);
	}
	else if (pamatch("set_reg(")) {
		state2(SC_INT, SC_INT, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
stateT(void)
{
	return(FALSE);
}

int
stateU(void)
{
	if (amatch("until")) {
		PutErrorE("Missing \'repeat\'");
	} else
		return(FALSE);

	return(TRUE);
}

int
stateV(void)
{
	if (pamatch("vanish("))	state1(SC_INT, SC_INT);
	else
		return(FALSE);

	return(TRUE);
}

int
stateW(void)
{
	if (amatch("while"))	doWhile();
	else
		return(FALSE);

	return(TRUE);
}

int
stateX(void)
{
	return(FALSE);
}

int
stateY(void)
{
	return(FALSE);
}

int
stateZ(void)
{
	return(FALSE);
}


/*
**�@���[�v���߂Ȃ�
*/
void
LoopStatement(char* key)
{
	indent++;
	while (1) {
		if (endOfLine()) {
			if (GetNewLine() == FALSE)
				PutError("Unexpected end of file error");
			PutCode("\n");
		}
		do {
			if (amatch(key)) goto ret;
			parseStatement();
			if (amatch(key)) goto ret;
		} while (amatch(":") && !endOfLine());
		if (!endOfLine()) SynErr();
	}
ret:
	indent--;
}

/*
 * ������̏���
 */
void
doSubsutitute(int isGlobal, SYMTBL* p)
{
	int	dim = p->dim;
	SCLASS	class = p->class;
	char* q;

	if (class == SC_STR)  dim--;
	TokenPtr += TokenLen(TokenPtr);
	if (dim == 0) {
		/* �P���ϐ��ւ̑�� */
		if (p->class == SC_STR) {
			if (amatch("[")) {
				/* sub[0] = 1�̂悤�Ȍ` */
				expression();
				ToInt1(lastClass);
				check("]");
				check("=");
				expression();	q = strpop();
				PutCode("%s[%s] = %s", p->name, strpop(), q);
			}
			else {
				check("=");
				expression();
				PutCode("strncpy(%s,%s,sizeof(%s))", p->name, strpop(), p->name);
			}
		}
		else {
			check("=");
			expression();
			PutCode("%s = %s", p->name, strpop());
			lastClass = ClassConvert(p->class, lastClass);
		}
	}
	else {
		/*�@�z��ϐ��ւ̑�� */
		if (famatch("=")) {
			/* sub = {0,1,2,3 ... } �̂悤�Ȍ` */
			int	i, cnt;
			cnt = GetIndexValueSize(p);
			check("=");
			check("{");
			for (i = 0; TRUE; i++) {
				expression();
				if (p->class == SC_STR) {
					PutCode("strncpy(%s[%d],%s,%d)", p->name, i, strpop(), p->size[p->dim - 1]);
				}
				else {
					PutCode("%s[%d] = %s", p->name, i, strpop());
				}
				if (i != cnt - 1)	PutCode(";\n");
				if (amatch(","))	continue;
				else			break;
			}
			check("}");
			if (i > cnt) PutError("�v�f�����������܂�");
		}
		else if (amatch("(")) {
			/* sub(0) = 1�̂悤�Ȍ` */
			DoIndexed(p);
			check("=");
			expression();
			q = strpop();
			if (p->class == SC_STR) {
				ToStr1(lastClass);
				PutCode("strncpy(%s,%s,%d)", strpop(), q, p->size[p->dim - 1]);
			}
			else {
				PutCode("%s = %s", strpop(), q);
			}
		}
		else
			SynErr();
	}
}

int
GetIndexValueSize(SYMTBL* p)
{
	int	cnt;
	int	dim = p->dim;

	if (dim == 1)
		cnt = p->size[0];
	else if (dim == 2)
		cnt = p->size[0] * p->size[1];
	else
		cnt = p->size[0] * p->size[1] * p->size[2];

	return(cnt);
}

/*
 * �������̏���
 */
void
doIf(void)
{
	expression();
	check("then");
	PutCode("if(%s) {", strpop());
	indent++;
	do { parseStatement(); } while (amatch(":") && !endOfLine());
	if (amatch("else")) {
		indent--;
		PutCode("} else {\n");
		indent++;
		do { parseStatement(); } while (amatch(":") && !endOfLine());
	}
	indent--;
	PutCode("}");
}


/*
 * �������������̏���
 */
void
doWhile(void)
{
	expression();
	PutCode("while(%s) {", strpop());
	amatch(":");
	LoopStatement("endwhile");
	PutCode("}\n");
}

void
doContinue(void)
{
	if (contLabel == -1)
		PutError("\'continue\' used outside loop");
	else
		PutCode("continue;\n");
}

void
doBreak(void)
{
	if (brkLabel == -1)
		PutError("\'break\' used outside loop");
	PutCode("break;\n");
}

/*
 *�@���������������̏���
 */
void
doRepeat(void)
{
	PutCode("do {");
	amatch(":");
	LoopStatement("until");
	expression();
	PutCode("} while (!(%s));", strpop());
}
/*
 *�@���������̏���
 */
void
doFor(void)
{
	SYMTBL* p;
	char* q;

	SkipSpace();
	if (p = SearchLoc(TokenPtr)) {
		TokenPtr += TokenLen(TokenPtr);
		if (p->dim != 0) PutError("Array variale cannot be used here.");
		if ((!p->class == SC_INT && p->class == SC_CHAR))
			PutError("Invalid variable type");
	}
	else if (p = SearchGlo(TokenPtr)) {
		TokenPtr += TokenLen(TokenPtr);
		if (p->dim != 0) PutError("Array variale cannot be used here.");
		if ((!p->class == SC_INT && p->class == SC_CHAR))
			PutError("Invalid variable type");
	}
	else {
		if (isalpha(*TokenPtr)) PutError("Undefined variable");
		else		       SynErr();
		return;
	}
	check("=");
	expression();
	check("to");
	expression();
	q = strpop();
	PutCode("for(%s = %s; %s <= %s ;%s++) {",
		p->name, strpop(), p->name, q, p->name);

	amatch(":");
	LoopStatement("next");
	PutCode("}");
}

/*
 *�@�������������̏���
 */
void
doPrint(void)
{
	char buff[MAX_BUFF_SIZE] = "\0";
	char s[MAX_BUFF_SIZE] = "\0";

	if (amatch("using")) {
		doUsing();
		return;
	}
	while (1) {
		char* fmt = NULL;
		if (endOfLine() || *TokenPtr == ':' || *TokenPtr == '}') {
			PutCode("_sxb_ltnl()\n");	/* ���s */
			break;
		}
		expression();
		strncpy(buff, strpop(), sizeof(buff));
		switch (lastClass) {
		case SC_CHAR:
			fmt = "%%c";
			break;
		case SC_INT:
			fmt = "%%d";
			break;
		case SC_FLOAT:
			fmt = "%%f";
			break;
		case SC_STR:
			// do NOT optimize printf("%s", "str") -> printf("str")
			// as the 'str' may contains escape characters.
			fmt = "%%s";
			break;
		}
		if (fmt == NULL) {
			printf("Unknwon Variable Type");
			break;
		}
		if (amatch(",")) {
			sprintf(s, "printf(\"%s\", %s);\n", fmt, buff);
			PutCode(s);
			continue;
		}
		else if (amatch(";")) {
			sprintf(s, "printf(\"%s\", %s);", fmt, buff);
			PutCode(s);
			if (endOfLine() || *TokenPtr == ':' || *TokenPtr == '}') break;
			continue;
		} else {
			sprintf(s, "printf(\"%s\\n\", %s);", fmt, buff);
			PutCode(s);
			break;
		}
	}
}
/*
** Parse "print using"
 */
void
doUsing(void)
{
	char* q;

	expression();
	doCast(SC_STR);
	check(";");
	expression();
	q = strpop();
	PutCode("sxb_using(%s, %s);\n", strpop(), q);
	if (lastClass == SC_STR) {
		PutCode("TODO - Print string\n");
	}
	else {
		ToFloat1(lastClass);
		PutCode("TODO - Print float\n");
	}
}

/*
** �s�̏I��肩�H
*/
int
endOfLine(void)
{
	if (feof(inputFp)) return(TRUE);

	SkipSpace();

	if ((*TokenPtr < ' ' && *TokenPtr != '\t')
		|| !strncmp(TokenPtr, "/*", 2) || !strncmp(TokenPtr, "��", 2)) {
		return(TRUE);
	}
	return(FALSE);
}

void
doReturn(void)
{
	if (inFunction == FALSE) {
		PutError("�s���Ȉʒu��return��������܂�");
		PutErrorE("\'return\' outside funciton");
	}
	if (endOfLine()) {
		if (retClass != SC_VOID) {
			PutError("�߂�l������܂���");
			PutErrorE("Missing return value");
		}
		PutCode("return;\n");
	}
	else {
		expression();
		doCast(retClass);
		PutCode("return %s;\n", strpop());
	}
}

/*
** �r�v�h�s�b�g���̏���
 */
void
doSwitch(void)
{
	SCLASS	class = casClass;

	expression();
	PutCode("switch(%s) {", strpop());

	casClass = lastClass;

	LoopStatement("endswitch");
	PutCode("}\n");
	casClass = class;
}

void
doCase(void)
{
	expression();
	PutCode("case %s:\n", strpop());

	if (casClass == SC_CHAR || casClass == SC_INT) {
		ToInt1(lastClass);
	}
	else if (casClass == SC_FLOAT) {
		ToFloat1(lastClass);
	}
	else if (casClass == SC_STR) {
		ToStr1(lastClass);
	}
	else
		SynErr();
}

void
doDefault(void)
{
	PutCode("default :\n");
}


/*
 * char, int, float�^�P���ϐ��̐錾
 */
void
DeclareVar(int isGlobal, SCLASS class)
{
	SYMTBL* p;

	do {
		SkipSpace();
		if (!isalpha(*TokenPtr)) SynErr();

		if (isGlobal) {
			p = DefGloVar(TokenPtr);
		}
		else {
			p = DefLocVar(TokenPtr);
		}
		PutCode(p->name);

		TokenPtr += TokenLen(TokenPtr);
		p->class = class;
		p->dim = 0;
		p->size[0] = p->size[1] = p->size[2] = 0;
		if (famatch("=")) {
			expression();
			PutCode(strpop());
		}
		else if (amatch("(")) {
			InitArray(isGlobal, class, p);
		}
	} while (famatch(","));
}

/*
 * str�^�P���ϐ��̐錾
 */
void
DeclareStr(int isGlobal)
{
	SYMTBL* p;

	do {
		SkipSpace();
		if (!isalpha(*TokenPtr)) SynErr();
		if (isGlobal)	p = DefGloVar(TokenPtr);
		else 			p = DefLocVar(TokenPtr);
		TokenPtr += TokenLen(TokenPtr);

		p->class = SC_STR;
		p->dim = 1;
		p->size[1] = p->size[2] = 0;
		p->size[3] = 32;
		if (amatch("(")) {
			InitStrArray(isGlobal, p);
		}
		else if (amatch("[")) {
			p->size[3] = GetConst();
			check("]");
			PutCode("%s[%d]", p->name, p->size[3]);
		}
		else {
			PutCode("%s[32]", p->name);
		}
		if (amatch("=")) {
			expression();
			ToStr1(lastClass);
			PutCode("= %s", strpop());
		}
	} while (famatch(","));
}

int
GetConst(void)
{
	aconst();
	if (lastClass != SC_INT)
		PutError("�����^�ɂ��ĉ�����");
	return(vali);
}

/*
** �z��ϐ��̐錾
*/
void
DeclareArray(int isGlobal)
{
	SCLASS	class = SC_INT;
	SYMTBL* p;

	if (amatch("int"))	 class = SC_INT;
	else if (amatch("char"))  class = SC_CHAR;
	else if (amatch("float")) class = SC_FLOAT;
	else if (amatch("str")) { DeclareStrArray(isGlobal); return; }

	do {
		SkipSpace();
		if (!isalpha(*TokenPtr)) SynErr();
		if (isGlobal) {
			p = DefGloVar(TokenPtr);
		}
		else {
			p = DefLocVar(TokenPtr);
		}

		TokenPtr += TokenLen(TokenPtr);
		check("(");
		InitArray(isGlobal, class, p);
	} while (famatch(","));
}

/*
** �Ȃɂ��֐��̕����������[�����ǁc�c
*/
void
InitArray(int isGlobal, SCLASS class, SYMTBL* p)
{
	p->class = class;
	p->dim = 0;
	p->size[0] = p->size[1] = p->size[2] = 0;
	do {
		p->size[p->dim] = GetConst();
		PutCode("[%d]", p->size[p->dim] + 1);
		if (p->dim++ >= 3)
			PutError("�S�����ȏ�̔z��͈����܂���");
	} while (amatch(","));
	check(")");

	if (p->dim == 0) SynErr();

	if (famatch("=")) SubArrayVar(isGlobal, p);
}

/*
** char, int, float �^�z��ϐ��錾�̏�����
*/
void
SubArrayVar(int isGlobal, SYMTBL* p)
{
	int	max = (p->size[0] + 1) * (p->size[1] + 1) * (p->size[2] + 1);

	fcheck("{");

	do {
		while (endOfLine()) {		/* �����s�ɓn��ꍇ */
			if (GetNewLine() == FALSE) SynErr();
		}
		if (max-- == 0) PutError("�������v�f���������܂�");
		expression();
		PutCode(strpop());
		if (p->class == SC_STR) SynErr();
	} while (famatch(","));
	while (endOfLine()) {		/* �����s�ɓn��ꍇ */
		if (GetNewLine() == FALSE) SynErr();
	}
	fcheck("}");
}

/*
** str�^�z��ϐ��̐錾
*/
void
DeclareStrArray(int isGlobal)
{
	SYMTBL* p;

	do {
		SkipSpace();
		if (!isalpha(*TokenPtr)) SynErr();
		if (isGlobal) {
			p = DefGloVar(TokenPtr);
		}
		else {
			p = DefLocVar(TokenPtr);
		}

		TokenPtr += TokenLen(TokenPtr);
		check("(");		/* �z��ϐ� */
		InitStrArray(isGlobal, p);
	} while (amatch(","));
}

/*
** �Ȃɂ��֐��̕����������[�����ǁc�c
*/
void
InitStrArray(int isGlobal, SYMTBL* p)
{
	p->class = SC_STR;
	p->dim = 0;
	p->size[1] = p->size[2] = 0;
	p->size[3] = 32;

	PutCode(p->name);
	do {
		p->size[p->dim] = GetConst();
		PutCode("[%d]", p->size[p->dim] + 1);
		if (p->dim++ >= 2)
			PutError("�R�����ȏ�̔z��͈����܂���");
	} while (amatch(","));
	check(")");
	if (amatch("[")) {
		p->size[p->dim] = GetConst();
		check("]");
		fprintf(outputFp, "[%d]", p->size[p->dim] + 1);
	}
	else {
		fprintf(outputFp, "[33]");
		p->size[p->dim] = 32;
	}
	if (p->dim++ < 1) SynErr();
	if (famatch("=")) SubArrayStr(isGlobal, p);
}


/*
** str�^�z��ϐ��錾�̏�����
*/
void
SubArrayStr(int isGlobal, SYMTBL* p)
{
	int	max;

	if (p->dim == 2) max = p->size[0] + 1;
	else		max = (p->size[0] + 1) * (p->size[1] + 1);

	fcheck("{");

	do {
		while (endOfLine()) {		/* �����s�ɓn��ꍇ */
			if (GetNewLine() == FALSE) SynErr();
			PutCode("\n");
		}
		if (max-- == 0) PutError("�������v�f���������܂�");
		expression();
		ToStr1(lastClass);
		PutCode(strpop());
	} while (famatch(","));
	if (endOfLine()) {		/* �����s�ɓn��ꍇ */
		if (GetNewLine() == FALSE) SynErr();
		PutCode("\n");
	}
	fcheck("}");
}

/*
 * Parse Function definition
 */
int
parseFunction(void)
{
	FNCTBL* p;
	LBLTBL* q;

	InitLocTbl();

	if (isFirstFunc) {
		PutCode("_sxb_end();\n"
			"}\n");
		isFirstFunc = FALSE;
		indent = 0;
	}
	if (inFunction)	SynErr();
	inFunction = TRUE;

	if (amatch("char"));
	else if (amatch("int"));
	else if (amatch("float"));
	else if (amatch("str"));

	SkipSpace();

	p = SearchFunc(TokenPtr);
	if (p == NULL) {
		PutError("parseFunction : Internal Error");
		return FALSE;
	}
	p->isDefined = TRUE;

	retClass = p->retClass;
	PutClass(p->retClass);
	PutCode(p->name);
	TokenPtr += TokenLen(TokenPtr);

	DoParam(p);

	PutCode("{\n");
	parseVariableDeclarations(FALSE);
	LoopStatement("endfunc");
	PutCode("}\n");
	q = SearchUndefLabel();
	if (q) PutError("Undefined Label: %d", q->name);

	inFunction = FALSE;
	return TRUE;
}

/*
** �������̏���
*/
void
DoParam(FNCTBL* f)
{
	SYMTBL* v;
	int		i;

	fcheck("(");

	if (f->pars == 0) PutCode("void");
	else {
		i = 0;
		do {
			if (!isalpha(*TokenPtr)) SynErr();
			PutClass(f->parClass[i]);
			v = DefLocVar(TokenPtr);
			v->class = SC_INT;
			v->dim = 0;

			TokenPtr += TokenLen(TokenPtr);
			PutCode(v->name);

			if (amatch(";")) {
				if (amatch("int"));
				else if (amatch("char"))		 v->class = SC_CHAR;
				else if (amatch("float"))	 v->class = SC_FLOAT;
				else if (amatch("str")) { v->class = SC_STR;	v->dim = 1; }
				else SynErr();
			}
			i++;
		} while (famatch(","));
		if (i != f->pars)	PutError("Internal Error - DoParam -");
	}
	fcheck(")");
}

void
PutClass(SCLASS s)
{
	switch (s) {
	case SC_VOID:	PutCode("void ");	break;
	case SC_CHAR:	PutCode("char ");	break;
	case SC_INT:	PutCode("int ");	break;
	case SC_FLOAT:PutCode("double ");	break;
	case SC_STR:	PutCode("char *");	break;
	default:
		fprintf(stderr, "Internal Error - PutClass(%d) -\n", (int)s);
		break;
	}
}
/*
** ���x���̒�`
*/
void
doLabel(void)
{
	LBLTBL* p;

	p = DefLabel(TokenPtr);
	TokenPtr += TokenLen(TokenPtr);
	check(":");

	PutCode("%s:", p->name);
}

#ifdef FIXME
/*
** �v���p�e�B�̐ݒ�
*/
void
doProp(void)
{
	bool	isarray = FALSE;
	char	itemname[100];
	char	propname[100];

	GetString(itemname);	/* Get Item Name */
	if (amatch("[")) {
		expression();
		ToInt1(lastClass);
		isarray = TRUE;
		check("]");
	}
	check(".");
	if (amatch("delete"));
	else if (amatch("active"))
		PutPropSet("_sxb_item_active", itemname, isarray, "", 0);
	else if (amatch("move")) {
		PutPropSet("_sxb_item4", itemname, isarray, "move", 4);
	}
	else if (amatch("line")) {
		PutPropSet("_sxb_item4", itemname, isarray, "line", 4);
	}
	else if (amatch("fill")) {
		PutPropSet("_sxb_item4", itemname, isarray, "fill", 4);
	}
	else if (amatch("box")) {
		PutPropSet("_sxb_item4", itemname, isarray, "box", 4);
	}
	else if (amatch("circle")) {
		PutPropSet("_sxb_item3", itemname, isarray, "circle", 3);
	}
	else if (amatch("new")) {
		PutPropSet("_sxb_item5", itemname, isarray, "new", 5);
	}
	else {
		GetString(propname);	/* �v���p�e�B���̓]�� */
		check("=");
		expression();
		if (lastClass == SC_STR) {
			PutPropSet2("_sxb_prop_setS", itemname, isarray, propname);
		}
		else if (lastClass == SC_FLOAT) {
			PutPropSet2("_sxb_prop_setF", itemname, isarray, propname);
		}
		else {
			PutPropSet2("_sxb_prop_set", itemname, isarray, propname);
		}
	}
}

void
PutPropSet(char* state, char* itemname, bool isarray, char* propname, int exps)
{
	if (exps > 0)	check("=");

	if (isarray) {
		PutCode("%s(\"%s\",1,%s,\"%s\"",
			state, itemname, strpop(), propname);
	}
	else {
		PutCode("%s(\"%s\",0,0,\"%s\"",
			state, itemname, propname);
	}
	while (exps-- > 0) {
		expression();
		PutCode(", %s", strpop());
		if (exps > 0)	check(",");
	}
	PutCode(")");
};

void
PutPropSet2(char* state, char* itemname, bool isarray, char* propname)
{
	char* p = strpop();

	if (isarray) {
		PutCode("%s(\"%s\",1,%s,\"%s\", %s)",
			state, itemname, strpop(), propname, p);
	}
	else {
		PutCode("%s(\"%s\",0,0,\"%s\", %s)",
			state, itemname, propname, p);
	}
};
#endif

void
GetString(char* p)
{
	while (isalnum(*TokenPtr)) *p++ = *TokenPtr++;
	*p = '\0';
}

/*
** INPUT�̏���
*/
void
doInput(void)
{
	SYMTBL* p;
	int	isGlobal = FALSE;
	int	dim;
	SCLASS	class;
	char buff[MAX_BUFF_SIZE] = "\0";
	char s[MAX_BUFF_SIZE] = "\0";

	SkipSpace();
 	expression();
	if (lastClass == SC_NONE) {
		printf("FIXME - No prompt string");
	}
	else {
		/* parse the prompt */
		ToStr1(lastClass);
		strncpy(buff, strpop(), sizeof(buff));
		sprintf(s, "printf(%s);\n", buff);
		PutCode(s);

		PutCode("printf(\"?\");\n");

		if (!amatch(";") && !amatch(",")) {
			PutError(" Either \";\" or \",\" expected");
		}
		SkipSpace();

		if (p = SearchLoc(TokenPtr)) {
			isGlobal = FALSE;
		}
		else if (p = SearchGlo(TokenPtr)) {
			isGlobal = TRUE;
		}
		else {
			PutError("���錾�̕ϐ��ł�");
		}
		TokenPtr += TokenLen(TokenPtr);
		doInput_Variable(p, isGlobal);
	}
}

void
doInput_Variable(SYMTBL * p, int isGlobal)
{
	int	dim = p->dim;
	int clazz = p->class;
	char* fmt;
	char buff[MAX_BUFF_SIZE] = "\0";
	char s[MAX_BUFF_SIZE] = "\0";

	if (clazz == SC_STR)  dim--;
	if (p->class == SC_CHAR) {
		fmt = "%c";
	} else if (p->class == SC_INT) {
		fmt = "%i";
	} else if (p->class == SC_FLOAT) {
		fmt = "%f";
	} else if (p->class == SC_STR) {
		fmt = "%s";
	}

#ifdef FIXME
	if (p->class == SC_STR && amatch("[")) {
		clazz = SC_CHAR;
		dim = 1;
		if (p->dim >= 2) {
			if (isGlobal) PutArrayCode(0x9c, p);
			else	     PutArrayCode(0x9b, p);
		}
		expression();
		check("]");
	} else if (dim != 0) {
		doSoeji(p);
	} else {
	}
#endif
	PutCode("scanf(\"%s\", &%s);\n", fmt, p->name);
}

/*
** �f�n�s�n���̏���
*/
void
doGoto(void)
{
	LBLTBL* p;

	SkipSpace();
	p = SearchLbl(TokenPtr);
	if (p == NULL) {
		p = RegLabel(TokenPtr);
		p->isDefined = FALSE;
	}
	PutCode("goto %s;\n", p->name);
	TokenPtr += TokenLen(TokenPtr);
}

// A special sttement without parenthess and optional parameter
void
doLocate(void)
{
	char* p1, *p2;

	expression();
	doCast(SC_INT);
	check(",");

	expression();
	doCast(SC_INT);

	if (amatch(",")) {
		expression();
		doCast(SC_INT);
		p1 = strpop();
		p2 = strpop();
		PutCode("locate_3(%s,%s,%s)\n", strpop(), p2, p1);		// FIXME - check func with 3 params.
	} else {
		p1 = strpop();
		PutCode("locate(%s,%s)\n", strpop(), p1);
	}
}

/*
** �֐��̌Ăяo������
*/
FNCTBL*
pcall(void)
{
	FNCTBL* p;
	int		paraCnt = 0;	/* �����̃J�E���^ */

	SkipSpace();
	p = SearchFunc(TokenPtr);
	if (p == NULL)
		PutError("���錾�̊֐��ł�");
	TokenPtr += TokenLen(TokenPtr);
	check("(");
	strpush("%s(", p->name);
	if (!amatch(")")) {
		while (1) {
			expression();
			doCast(p->parClass[paraCnt++]);
			sxb_strcat(strpop());
			if (amatch(",") == FALSE)	break;
			sxb_strcat(",");
		}
		check(")");
	}
	sxb_strcat(")");
	if (paraCnt > p->pars) PutError("�������������܂�");
	if (paraCnt < p->pars) PutError("���������Ȃ����܂�");

	return(p);
}
