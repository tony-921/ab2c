/*
	SX-BASIC	コンパイラ本体
	Programmed By ISHIGAMI Tatsuya
	08/26/95

*/

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<setjmp.h>

#include	"sxbasic.h"


char* TokenPtr;
SCLASS	casClass;			/* swict文で使われた変数の型 */
SCLASS	retClass;			/* 関数の戻り値のタイプ */

int		indent = 0;			/* Indent depth of Output File */
bool	inFunction;			/* 関数の中か */
bool	isFirstFunc;		/* 最初の関数		*/
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

	if (isFirstFunc) {	/* 関数が一つもなかった時 */
		if (p = SearchUndefLabel()) {
			PutErrorE("Undefined label");
		}
		PutCode("\n};");
	}
	if (inFunction) {
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
	// Pass2();		/* FIXME - put code to arrange items */

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
	ret = state_external();
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
** LINPUTの処理
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
		PutError("未宣言の変数です");
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



/*
**　ループ命令など
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
 * 代入文の処理
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
		/* 単純変数への代入 */
		if (p->class == SC_STR) {
			if (amatch("[")) {
				/* sub[0] = 1のような形 */
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
		/*　配列変数への代入 */
		if (famatch("=")) {
			/* sub = {0,1,2,3 ... } のような形 */
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
			if (i > cnt) PutError("要素数が多すぎます");
		}
		else if (amatch("(")) {
			/* sub(0) = 1のような形 */
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
 * ｉｆ文の処理
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
 * ｗｈｉｌｅ文の処理
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
 *　ｒｅｐｅａｔ文の処理
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
 *　ｆｏｒ文の処理
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
 *　ｐｒｉｎｔ文の処理
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
			PutCode("_sxb_ltnl()\n");	/* 改行 */
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
** 行の終わりか？
*/
int
endOfLine(void)
{
	if (feof(inputFp)) return(TRUE);

	SkipSpace();

	if ((*TokenPtr < ' ' && *TokenPtr != '\t')
		|| !strncmp(TokenPtr, "/*", 2) || !strncmp(TokenPtr, "▼", 2)) {
		return(TRUE);
	}
	return(FALSE);
}

void
doReturn(void)
{
	if (inFunction == FALSE) {
		PutError("\'return\' without a function");
	}
	if (endOfLine()) {
		if (retClass != SC_VOID) {
			PutError("Missing a return value");
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
** ＳＷＩＴＣＨ文の処理
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
 * char, int, float型単純変数の宣言
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
 * str型単純変数の宣言
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
		PutError("整数型にして下さい");
	return(vali);
}

/*
** Declare varriable array
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
** なにか関数の分け方が半端だけど……
*/
void
InitArray(int isGlobal, SCLASS class, SYMTBL* p)
{
	p->class = class;
	p->dim = 0;
	p->size[0] = p->size[1] = p->size[2] = 0;

	PutCode("%s %s", TypeToStr(p->class), p->name);

	do {
		p->size[p->dim] = GetConst();
		PutCode("[%d]", p->size[p->dim] + 1);
		if (p->dim++ >= 3)
			PutError("４次元以上の配列は扱えません");
	} while (amatch(","));
	check(")");

	if (p->dim == 0) SynErr();

	if (famatch("=")) SubArrayVar(isGlobal, p);
}

/*
** char, int, float 型配列変数宣言の初期化
*/
void
SubArrayVar(int isGlobal, SYMTBL* p)
{
	int	max = (p->size[0] + 1) * (p->size[1] + 1) * (p->size[2] + 1);

	fcheck("{");

	do {
		while (endOfLine()) {		/* 複数行に渡る場合 */
			if (GetNewLine() == FALSE) SynErr();
		}
		if (max-- == 0) PutError("初期化要素が多すぎます");
		expression();
		PutCode(strpop());
		if (p->class == SC_STR) SynErr();
	} while (famatch(","));
	while (endOfLine()) {		/* 複数行に渡る場合 */
		if (GetNewLine() == FALSE) SynErr();
	}
	fcheck("}");
}

/*
** str型配列変数の宣言
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
		check("(");		/* 配列変数 */
		InitStrArray(isGlobal, p);
	} while (amatch(","));
}

/*
** なにか関数の分け方が半端だけど……
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
			PutError("３次元以上の配列は扱えません");
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
** str型配列変数宣言の初期化
*/
void
SubArrayStr(int isGlobal, SYMTBL* p)
{
	int	max;

	if (p->dim == 2) max = p->size[0] + 1;
	else		max = (p->size[0] + 1) * (p->size[1] + 1);

	fcheck("{");

	do {
		while (endOfLine()) {		/* 複数行に渡る場合 */
			if (GetNewLine() == FALSE) SynErr();
			PutCode("\n");
		}
		if (max-- == 0) PutError("初期化要素が多すぎます");
		expression();
		ToStr1(lastClass);
		PutCode(strpop());
	} while (famatch(","));
	if (endOfLine()) {		/* 複数行に渡る場合 */
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
	PrintType(p->retClass);
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
** 仮引数の処理
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
			PrintType(f->parClass[i]);
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
PrintType(SCLASS t)
{
	char* s = TypeToStr(t);
	PutCode("%s ", s);
}

/*
** ラベルの定義
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

void
GetString(char* p)
{
	while (isalnum(*TokenPtr)) *p++ = *TokenPtr++;
	*p = '\0';
}

/*
** INPUTの処理
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
			PutError("未宣言の変数です");
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
** ＧＯＴＯ文の処理
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

// A special statement without parenthess and optional parameter
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
** 関数の呼び出し処理
*/
FNCTBL*
pcall(void)
{
	FNCTBL* p;
	int		paraCnt = 0;	/* 引数のカウンタ */

	SkipSpace();
	p = SearchFunc(TokenPtr);
	if (p == NULL)
		PutError("未宣言の関数です");
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
	if (paraCnt > p->pars) PutError("引数が多すぎます");
	if (paraCnt < p->pars) PutError("引数が少なすぎます");

	return(p);
}

// Music functions
DEF_STATEMENT statementDefinitions[] = {
	// music functions
	STATEMENT2("m_alloc(", "m_alloc(", SC_INT, SC_INT),
	STATEMENT2("m_assign(", "m_assign(", SC_INT, SC_INT),
	STATEMENT0("m_cont(", "m_cont("),
	STATEMENT0("m_init(", "m_init("),
	STATEMENT0("m_play(", "m_play("),
	STATEMENT0("m_stop(", "m_stop("),
	STATEMENT1("m_sysch(", "m_sysch(", SC_STR),
	STATEMENT2("m_trk(", "m_trk(", SC_INT, SC_STR),
};

int
state_external(void) {
	int	i;
	int totalFunctions = sizeof(statementDefinitions) / sizeof(DEF_STATEMENT);
	for (i = 0; i < totalFunctions; i++) {
		DEF_STATEMENT* f = &statementDefinitions[i];
		if (amatch(f->b_name)) {
			strpush(f->c_name);
			if (f->numParams == 0) {
				state0(f->retClass);
			}
			if (f->numParams == 1) 		state1(f->retClass, f->param1);
			if (f->numParams == 2) 		state2(f->retClass, f->param1, f->param2);
			if (f->numParams == 3)		state3(f->retClass, f->param1, f->param2, f->param3);
			if (f->numParams > 3)		PutError("Internal Error state_external %s %d", f->b_name, f->numParams);
			return TRUE;
		}
	}
	return FALSE;
}
