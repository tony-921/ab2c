/*
	SX-BASIC	コンパイラ本体
	Programmed By ISHIGAMI Tatsuya
	08/26/1995	for SX-BASIC
	01/20/2025	for X-Basic to C converter

*/

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<setjmp.h>

#include	"sxbasic.h"

char* TokenPtr;
E_TYPE	casType;			/* swict文で使われた変数の型 */
E_TYPE	retType;			/* 関数の戻り値のタイプ */

int		indent = 0;			/* Indent depth of Output File */
bool	inFunction;			/* 関数の中か */
bool	isFirstFunc;		/* 最初の関数		*/
int		brkLabel;
int		contLabel;

extern	int		vali;
extern	E_TYPE	lastType;

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
	PutCode("void main(int b_argc, char *b_argv[]) {");
	indent++;
	do {
		PutCode("\n");
		do {
			parseStatement();
		} while (amatch(":") && !IsEndOfLine());
		if (!IsEndOfLine()) SynErr();
	} while (GetNewLine());

	if (isFirstFunc) {	/* 関数が一つもなかった時 */
		if (p = SearchUndefLabel()) {
			PutError("Undefined label");
		}
		PutCode("\n};");
	}
	if (inFunction) {
		PutError("Incomplete function");
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
	rewind(inputFp);

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
				DeclareVariable(isGlobal, ET_INT);
			else if (famatch("char ") || famatch("char\t"))
				DeclareVariable(isGlobal, ET_CHAR);
			else if (famatch("float ") || famatch("float\t"))
				DeclareVariable(isGlobal, ET_FLOAT);
			else if (amatch("str ") || amatch("str\t")) {
				PutCode("char\t");
				DeclareStr(isGlobal);
			}
			else {
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
	ret = ParseRegularFunctions(FALSE, FALSE);
	if (ret) return;

	ret = ParseSpecialStatement();
	if (ret) return;

	if (*TokenPtr == '{') {
		TokenPtr++;
		LoopStatement("}");
		return;
	}
	else if (isalpha(*TokenPtr)) {
		nextToken = &TokenPtr[TokenLen(TokenPtr)];
		if (*nextToken == ':')
			doLabelDefinition();
		else if (p = SearchLoc(TokenPtr))
			doSubsutitute(FALSE, p);
		else if (p = SearchGlo(TokenPtr))
			doSubsutitute(TRUE, p);
		else if (*nextToken == '(') {
			doFunctionCall();
			PutCode("%s", strpop());
		}
		else
			PutError("Undefined Variable Error");
	}
	else if (IsEndOfLine() == FALSE) {
		SynErr();
	}

	PutCode(";");
}


int
ParseSpecialStatement(void)
{
	if (amatch("break")) {
		doBreak();
	}
	else if (amatch("beep")) {
		PutCode("putchar('\07');");
	}
	else if (amatch("continue")) {
		doContinue();
	}
	else if (amatch("case")) {
		doCase();
	} 
	else if (amatch("cls")) {
		PutCode("printf(\"\\033[2J\")");
	} else if (amatch("default")) 			doDefault();
	else if (amatch("endfunc"))		PutError("endfunc without func");
	else if (amatch("endswitch"))PutError("endswitch without switch");
	else if (amatch("endwhile"))	PutError("endwhile without while");
	else if (amatch("end"))		PutCode("b_exit(0);");
	else if (amatch("for")) {
		doFor();
	}
	else if (amatch("func")) {
		parseFunction();
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
	else if (amatch("goto")) {
		doGoto();
	}
	else if (amatch("if")) {
		doIf();
	}
	else if (amatch("input")) {
		doInput();
	}
	else if (amatch("linput")) {
		doLinput();
	}
	else if (amatch("locate")) {
		doLocate();
	}
	else if (amatch("next")) {
		PutError("Missing \'for\' keyword before \'next\'");
	}
	else if (amatch("print")) {
		doPrint();
	}
	else if (amatch("repeat"))	doRepeat();
	else if (amatch("return"))	doReturn();
	else if (amatch("switch")) {
		doSwitch();
	} else if (pamatch("strlwr(")) {
		doTransStr(0);
		PutCode("%s;", strpop());
	} else if (pamatch("strnset(")) {
		doTransStr(2);
		PutCode("%s;", strpop());
	} else if (pamatch("strrev(")) {
		doTransStr(0);
		PutCode(";%s", strpop());
	} else if (pamatch("strset(")) {
		doTransStr(1);
		PutCode("%s;", strpop());
	} else if (pamatch("strupr(")) {
		doTransStr(0);
		PutCode("%s;", strpop());
	} else if (amatch("until")) {
		PutError("Missing \'repeat\'");
	} else if (amatch("while")) {
		doWhile();
	} else if (amatch("?")) {
		doPrint();
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

	doInput_Prompt();
	SkipSpace();

	if (p = SearchLoc(TokenPtr)) {
		isGlobal = FALSE;
	} else if (p = SearchGlo(TokenPtr)) {
		isGlobal = TRUE;
	}
	if (p == NULL) {
		PutError("Undefined variable");
	} else {
		if (p->type != ET_STR)	PutError("Invalid Variable Type");
		/* FIXME - Support variable string variable */
		if (p->dim != 1) PutError("Cannot use index variable here.");
		TokenPtr += TokenLen(TokenPtr);
		PutCode("scanf(\"%%s\", &%s);\n", p->name);
	}
}


/*
**　ループ命令など
*/
void
LoopStatement(char* key)
{
	indent++;
	while (1) {
		if (IsEndOfLine()) {
			if (GetNewLine() == FALSE)
				PutError("Unexpected end of file error");
			PutCode("\n");
		}
		do {
			if (amatch(key)) goto ret;
			parseStatement();
			if (amatch(key)) goto ret;
		} while (amatch(":") && !IsEndOfLine());
		if (!IsEndOfLine()) SynErr();
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
	E_TYPE	type = p->type;
	char* q;

	if (type == ET_STR)  dim--;
	TokenPtr += TokenLen(TokenPtr);
	if (dim == 0) {
		/* 単純変数への代入 */
		if (p->type == ET_STR) {
			if (amatch("[")) {
				/* sub[0] = 1のような形 */
				expression();
				ToInt1(lastType);
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
			lastType = PickAlignedType(p->type, lastType);
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
				if (p->type == ET_STR) {
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
			doArrayIndex(p);
			check("=");
			expression();
			q = strpop();
			if (p->type == ET_STR) {
				ToStr1(lastType);
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
	do { parseStatement(); } while (amatch(":") && !IsEndOfLine());
	if (amatch("else")) {
		indent--;
		PutCode("} else {\n");
		indent++;
		do { parseStatement(); } while (amatch(":") && !IsEndOfLine());
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
		if ((!p->type == ET_INT && p->type == ET_CHAR))
			PutError("Invalid variable type");
	}
	else if (p = SearchGlo(TokenPtr)) {
		TokenPtr += TokenLen(TokenPtr);
		if (p->dim != 0) PutError("Array variale cannot be used here.");
		if ((!p->type == ET_INT && p->type == ET_CHAR))
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
		if (IsEndOfLine() || *TokenPtr == ':' || *TokenPtr == '}') {
			PutCode("putchar('\\n');\n");
			break;
		}
		expression();
		strncpy(buff, strpop(), sizeof(buff));
		switch (lastType) {
		case ET_CHAR:
			fmt = "%%c";
			break;
		case ET_INT:
			fmt = "%%d";
			break;
		case ET_FLOAT:
			fmt = "%%f";
			break;
		case ET_STR:
			/* do NOT optimize printf("%s", "str") -> printf("str")
			   as the 'str' may contains escape characters. */
			fmt = "%%s";
			break;
		default:
			/* nothing to do with NONE and VOID */
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
			if (IsEndOfLine() || *TokenPtr == ':' || *TokenPtr == '}') break;
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
	doCast(ET_STR);
	check(";");
	expression();
	q = strpop();
	PutCode("sxb_using(%s, %s);\n", strpop(), q);
	if (lastType == ET_STR) {
		PutCode("TODO - Print string\n");
	}
	else {
		ToFloat1(lastType);
		PutCode("TODO - Print float\n");
	}
}

/*
** 行の終わりか？
*/
int
IsEndOfLine(void)
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
	if (IsEndOfLine()) {
		if (retType != ET_VOID) {
			PutError("Missing a return value");
		}
		PutCode("return;\n");
	}
	else {
		expression();
		doCast(retType);
		if (retType == ET_STR) {
			PutCode("strncpy(_str_buff, %s, sizeof(_str_buff));\n", strpop());
			PutCode("return &_str_buff;");
		} else {
			PutCode("return %s;\n", strpop());
		}
	}
}

/*
** ＳＷＩＴＣＨ文の処理
 */
void
doSwitch(void)
{
	E_TYPE	type = casType;

	expression();
	PutCode("switch(%s) {", strpop());

	casType = lastType;

	LoopStatement("endswitch");
	PutCode("}\n");
	casType = type;
}

void
doCase(void)
{
	expression();
	PutCode("case %s:\n", strpop());

	if (casType == ET_CHAR || casType == ET_INT) {
		ToInt1(lastType);
	}
	else if (casType == ET_FLOAT) {
		ToFloat1(lastType);
	}
	else if (casType == ET_STR) {
		ToStr1(lastType);
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
DeclareVariable(int isGlobal, E_TYPE type)
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
		p->type = type;
		p->dim = 0;
		p->size[0] = p->size[1] = p->size[2] = 0;
		if (famatch("=")) {
			expression();
			PutCode(strpop());
		}
		else if (amatch("(")) {
			InitArray(isGlobal, type, p);
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

		p->type = ET_STR;
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
			ToStr1(lastType);
			PutCode("= %s", strpop());
		}
	} while (famatch(","));
}

int
GetConst(void)
{
	aconst();
	if (lastType != ET_INT)
		PutError("整数型にして下さい");
	return(vali);
}

/*
** Declare varriable array
*/
void
DeclareArray(int isGlobal)
{
	E_TYPE	type = ET_INT;
	SYMTBL* p;

	if (amatch("int"))	 type = ET_INT;
	else if (amatch("char"))  type = ET_CHAR;
	else if (amatch("float")) type = ET_FLOAT;
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
		InitArray(isGlobal, type, p);
	} while (famatch(","));
}

/*
** なにか関数の分け方が半端だけど……
*/
void
InitArray(int isGlobal, E_TYPE type, SYMTBL* p)
{
	p->type = type;
	p->dim = 0;
	p->size[0] = p->size[1] = p->size[2] = 0;

	PutCode("%s %s", TypeToStr(p->type), p->name);

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
		while (IsEndOfLine()) {		/* 複数行に渡る場合 */
			if (GetNewLine() == FALSE) SynErr();
		}
		if (max-- == 0) PutError("初期化要素が多すぎます");
		expression();
		PutCode(strpop());
		if (p->type == ET_STR) SynErr();
	} while (famatch(","));
	while (IsEndOfLine()) {		/* 複数行に渡る場合 */
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
	p->type = ET_STR;
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
		while (IsEndOfLine()) {		/* 複数行に渡る場合 */
			if (GetNewLine() == FALSE) SynErr();
			PutCode("\n");
		}
		if (max-- == 0) PutError("初期化要素が多すぎます");
		expression();
		ToStr1(lastType);
		PutCode(strpop());
	} while (famatch(","));
	if (IsEndOfLine()) {		/* 複数行に渡る場合 */
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
		PutCode("b_exit(0);\n}\n");
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

	retType = p->retType;
	PrintType(p->retType);
	PutCode(p->name);
	TokenPtr += TokenLen(TokenPtr);

	DoParam(p);

	PutCode("{\n");
	if (p->retType == ET_STR) {
		PutCode("static char _str_buff[TEMP_STR_BUFF_SIZE];\n");
	}
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
			PrintType(f->parTypes[i]);
			v = DefLocVar(TokenPtr);
			v->type = ET_INT;
			v->dim = 0;

			TokenPtr += TokenLen(TokenPtr);
			PutCode(v->name);

			if (amatch(";")) {
				if (amatch("int"));
				else if (amatch("char"))	v->type = ET_CHAR;
				else if (amatch("float"))	v->type = ET_FLOAT;
				else if (amatch("str")) { v->type = ET_STR;	v->dim = 1; }
				else SynErr();
			}
			i++;
		} while (famatch(","));
		if (i != f->pars)	PutError("Internal Error - DoParam -");
	}
	fcheck(")");
}

void
PrintType(E_TYPE t)
{
	char* s = TypeToStr(t);
	PutCode("%s ", s);
}

/*
** ラベルの定義
*/
void
doLabelDefinition(void)
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

	doInput_Prompt();
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

int
doInput_Prompt(void)
{
	char* possiblePrompt = NULL;
	char buff[MAX_BUFF_SIZE] = "\0";
	char s[MAX_BUFF_SIZE] = "\0";

	SkipSpace();

	possiblePrompt = TokenPtr;
	expression();

	/* input <prompt>;<var> */
	if (amatch(",") || amatch(";")) {
		ToStr1(lastType);
		strncpy(buff, strpop(), sizeof(buff));
		sprintf(s, "printf(%s);\n", buff);
		PutCode(s);
		return(TRUE);
	}
	/* input <var> (without prompt) */
	/* Get rid of previous expr parse. */
	strpop();
	TokenPtr = possiblePrompt;

	PutCode("printf(\"?\");\n");

	return (FALSE);
}

void
doInput_Variable(SYMTBL * p, int isGlobal)
{
	int	dim = p->dim;
	int type = p->type;
	char* fmt = "";

	if (type == ET_STR)  dim--;
	if (p->type == ET_CHAR) {
		fmt = "%c";
	} else if (p->type == ET_INT) {
		fmt = "%i";
	} else if (p->type == ET_FLOAT) {
		fmt = "%f";
	} else if (p->type == ET_STR) {
		fmt = "%s";
	}

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

/* A special statement without parenthess and optional parameter */
void
doLocate(void)
{
	char* p1, *p2;

	expression();
	doCast(ET_INT);
	check(",");

	expression();
	doCast(ET_INT);

	if (amatch(",")) {
		expression();
		doCast(ET_INT);
		p1 = strpop();
		p2 = strpop();
		PutCode("locate(%s,%s);\n", strpop(), p2);
		PutCode("b_csw(%s);\n", p1);
	} else {
		p1 = strpop();
		PutCode("locate(%s,%s);\n", strpop(), p1);
	}
}

/*
** 関数の呼び出し処理
*/
FNCTBL*
doFunctionCall(void)
{
	FNCTBL* p;
	int		paraCnt = 0;	/* 引数のカウンタ */

	SkipSpace();
	p = SearchFunc(TokenPtr);
	if (p == NULL) {
		PutError("未宣言の関数です");
		return NULL;
	}
	TokenPtr += TokenLen(TokenPtr);
	check("(");
	strpush("%s(", p->name);
	if (!amatch(")")) {
		while (1) {
			expression();
			doCast(p->parTypes[paraCnt++]);
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
