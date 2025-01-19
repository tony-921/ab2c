/*
	I-BASIC	コンパイラ本体（組み込み関数）
	Programmed By ISHIGAMI Tatsuya
	02/20/94

*/

#include	<stdio.h>
#include	<string.h>
#include	<time.h>
#include	<setjmp.h>    

#include	"sxbasic.h"

// #define	STRN	0x5354524e

extern	char* TokenPtr;
extern	SCLASS	lastClass;
extern	FILE* outputFp;

int
efuncs(void)
{
	int		ret = FALSE;

	SkipSpace();

	ret = func_external();
	if (ret) return TRUE;

	switch (*TokenPtr) {
	case 'a':
		ret = funcA();
		break;
	case 'b':
		ret = funcB();
		break;
	case 'c':
		ret = funcC();
		break;
	case 'd':
		ret = funcD();
		break;
	case 'e':
		ret = funcE();
		break;
	case 'f':
		ret = funcF();
		break;
	case 'g':
		ret = funcG();
		break;
	case 'h':
		ret = funcH();
		break;
	case 'i':
		ret = funcI();
		break;
	case 'j':
		ret = funcJ();
		break;
	case 'k':
		ret = funcK();
		break;
	case 'l':
		ret = funcL();
		break;
	case 'm':
		ret = funcM();
		break;
	case 'n':
		ret = funcN();
		break;
	case 'o':
		ret = funcO();
		break;
	case 'p':
		ret = funcP();
		break;
	case 'q':
		ret = funcQ();
		break;
	case 'r':
		ret = funcR();
		break;
	case 's':
		ret = funcS();
		break;
	case 't':
		ret = funcT();
		break;
	case 'u':
		ret = funcU();
		break;
	case 'v':
		ret = funcV();
		break;
	case 'w':
		ret = funcW();
		break;
	case 'x':
		ret = funcX();
		break;
	case 'y':
		ret = funcY();
		break;
	case 'z':
		ret = funcZ();
		break;
	}
	return(ret);
}

int
funcA(void)
{
	if (pamatch("asc(")) 		func1(SC_INT, SC_STR);
	else if (pamatch("atof("))	func1(SC_FLOAT, SC_STR);
	else if (pamatch("atoi("))	func1(SC_INT, SC_STR);
	else if (pamatch("abs(")) 	func1(SC_FLOAT, SC_FLOAT);
	else if (pamatch("atan(")) 	func1(SC_FLOAT, SC_FLOAT);
	else	return(FALSE);

	return(TRUE);
}

int
funcB(void)
{
	if (amatch("bin$(")) {
		func1(SC_STR, SC_INT);
		strpush("_sxb_bin(%s)", strpop());
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcC(void)
{
	if (amatch("chr$(")) {
		strpush("_sxb_chrS(");
		func1(SC_STR, SC_INT);
	} else if (pamatch("cos(")) 	func1(SC_FLOAT, SC_FLOAT);
	else
		return(FALSE);

	return(TRUE);
}

int
funcD(void)
{
	if (pamatch("dskf(")) {
		func1(SC_INT, SC_CHAR);
	} else if (amatch("date$")) {
		strpush("dataS()");
		lastClass = SC_STR;
	} else if (amatch("day$")) {
		strpush("dayS()");
		lastClass = SC_STR;
	} else
		return(FALSE);

	return(TRUE);
}

int
funcE(void)
{
	if (pamatch("exp(")) 			func1(SC_FLOAT, SC_FLOAT);
	else if (pamatch("ecvt(")) 	doEcvt();
	else
		return(FALSE);

	return(TRUE);
}

int
funcF(void)
{
	if (pamatch("fix(")) {
		func1(SC_FLOAT, SC_FLOAT);
	}
	else if (pamatch("fcvt(")) {
		doEcvt();
	}
	else if (pamatch("fclose(")) {
		func1(SC_INT, SC_INT);
	}
	else if (pamatch("fcloseall()")) {
		func0(SC_INT);
	}
	else if (amatch("fdelete(")) {
		strpush("TSDeleteP(");
		func1(SC_INT, SC_STR);
	}
	else if (amatch("feof(")) {
		strpush("_sxb_feof(");
		func1(SC_INT, SC_INT);
	}
	else if (amatch("fgetc(")) {
		strpush("_sxb_fgetc(");
		func1(SC_INT, SC_INT);
	}
	else if (amatch("fopen(")) {
		strpush("_sxb_fopen(");
		func2(SC_INT, SC_STR, SC_STR);
	}
	else if (amatch("fputc(")) {
		strpush("_sxb_fputc(");
		func2(SC_INT, SC_CHAR, SC_INT);
	}
	else if (amatch("frename(")) {
		strpush("_sxb_frename(");
		func2(SC_INT, SC_STR, SC_STR);
	}
	else if (amatch("fseek(")) {
		strpush("_sxb_fseek(");
		func3(SC_INT, SC_INT, SC_INT, SC_INT);
	}
	else if (amatch("fread(")) {
		strpush("_sxb_fread(");
		doFread();
	}
	else if (amatch("fwrite(")) {
		strpush("_sxb_fwrite(");
		doFread();
	}
	else if (amatch("freads(")) {
		strpush("_sxb_freads(");
		doTransStr(1);
		lastClass = SC_INT;
	}
	else if (amatch("fwrites(")) {
		strpush("_sxb_fwrites(");
		func2(SC_INT, SC_STR, SC_INT);
	}
	else if (pamatch("fock(")) {
		func1(SC_STR, SC_INT);
	}
	else if (pamatch("findtskn(")) {
		func2(SC_INT, SC_STR, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcG(void)
{
	if (pamatch("gcvt(")) {
		func2(SC_STR, SC_FLOAT, SC_INT);
	}
	else if (pamatch("getmes()")) {
		lastClass = SC_STR;
	}
	else if (amatch("getenv(")) {
		strpush("_sxb_getenv(");
		func2(SC_STR, SC_STR, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcH(void)
{
	if (amatch("hex$(")) {
		strpush("_sxb_hex(");
		func1(SC_STR, SC_INT);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcI(void)
{
	if (pamatch("int(")) 			func1(SC_INT, SC_FLOAT);
	else if (pamatch("itoa("))	func1(SC_STR, SC_INT);
	else if (pamatch("instr(")) 	func3(SC_INT, SC_STR, SC_STR, SC_INT);
	else if (pamatch("isalnum(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("isalpha(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("isascii(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("iscntrl(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("isdigit(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("isgraph(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("islower(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("isprint(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("ispunct(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("isspace(")) func1(SC_INT, SC_CHAR);
	else if (pamatch("isupper("))	func1(SC_INT, SC_CHAR);
	else if (pamatch("isxdigit(")) func1(SC_INT, SC_CHAR);
	else if (amatch("inkey$(0)")) {
		strpush("_sxb_inkey0()");
		lastClass = SC_STR;
	}
	else if (amatch("inkey$")) {
		strpush("_sxb_inkey()");
		lastClass = SC_STR;
	}
	else if (pamatch("iocs(")) {
		func1(SC_INT, SC_INT);
	}
	else if (amatch("inputbox$(")) {
		strpush("_sxb_inputboxS(");
		func1(SC_STR, SC_STR);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcJ(void)
{
	return(FALSE);
}

int
funcK(void)
{
	return(FALSE);
}

int
funcL(void)
{
	if (pamatch("log(")) 			func1(SC_FLOAT, SC_FLOAT);
	else if (amatch("len(")) {
		strpush("strlen(");
		func1(SC_INT, SC_STR);
	}
	else if (amatch("left$(")) {
		strpush("_sxb_leftS(");
		func2(SC_STR, SC_INT, SC_STR);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcM(void)
{
	if (amatch("mid$(")) {
		strpush("_sxb_midS(");
		func3(SC_STR, SC_INT, SC_INT, SC_STR);
	}
	else if (amatch("mirror$(")) {
		strpush("_sxb_mirrorS(");
		func1(SC_STR, SC_STR);
	}
	else if (amatch("mousex")) {
		strpush("_sxb_mousex()");
		lastClass = SC_INT;
	}
	else if (amatch("mousey")) {
		strpush("_sxb_mousey()");
		lastClass = SC_INT;
	}
	else if (amatch("mousel")) {
		strpush("_sxb_mousel()");
		lastClass = SC_INT;
	}
	else if (amatch("mouser")) {
		strpush("_sxb_mouser()");
		lastClass = SC_INT;
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcN(void)
{
	return(FALSE);
}

int
funcO(void)
{
	if (amatch("oct$(")) {
		strpush("_sxb_octS(");
		func1(SC_STR, SC_INT);
	}
	else if (amatch("openres(")) {
		strpush("_sxb_openres");
		func1(SC_INT, SC_STR);
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcP(void)
{
	if (pamatch("pi(")) {
		if (amatch(")")) {
			sxb_strcat("1)");
			lastClass = SC_FLOAT;
		}
		else
			func1(SC_FLOAT, SC_FLOAT);
	}
	else if (pamatch("pow(")) {
		func2(SC_FLOAT, SC_FLOAT, SC_FLOAT);
	}
	else if (pamatch("peek(")) {
		func1(SC_INT, SC_INT);
	}
	else if (pamatch("peekw(")) {
		func1(SC_INT, SC_INT);
	}
	else if (pamatch("peekl(")) {
		func1(SC_INT, SC_INT);
	}
	else if (amatch("path$")) {
		strpush("_sxb_pathS()");
		lastClass = SC_STR;
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcQ(void)
{
	return(FALSE);
}

int
funcR(void)
{
	if (pamatch("rand(")) 		func0(SC_INT);
	else if (pamatch("rnd(")) 	func0(SC_FLOAT);
	else if (amatch("right$(")) {
		strpush("_sxb_rightS(");
		func2(SC_STR, SC_INT, SC_STR);
	}
	else if (pamatch("ref_reg(")) func1(SC_INT, SC_INT);
	else
		return(FALSE);

	return(TRUE);
}

int
funcS(void)
{
	if (amatch("str$(")) {
		strpush("_sxb_str(");
		func1(SC_STR, SC_FLOAT);
	}
	else if (pamatch("sgn(")) 	func1(SC_FLOAT, SC_FLOAT);
	else if (pamatch("sin(")) 	func1(SC_FLOAT, SC_FLOAT);
	else if (pamatch("sqr(")) 	func1(SC_FLOAT, SC_FLOAT);
	else if (pamatch("space$("))	func1(SC_STR, SC_INT);
	else if (pamatch("strchr("))	func2(SC_INT, SC_STR, SC_CHAR);
	else if (pamatch("strcspn("))	func2(SC_INT, SC_STR, SC_STR);
	else if (pamatch("string$("))	func2(SC_STR, SC_INT, SC_STR);
	else if (pamatch("strlen(")) 	func1(SC_INT, SC_STR);
	else if (pamatch("strrchr("))	func2(SC_INT, SC_STR, SC_CHAR);
	else if (pamatch("strspn(")) 	func2(SC_INT, SC_STR, SC_STR);
	else if (pamatch("strtok(")) 	func2(SC_STR, SC_STR, SC_STR);
	else if (pamatch("strlwr("))		doTransStrFunc(0);
	else if (pamatch("strnset("))	doTransStrFunc(2);
	else if (pamatch("strrev(")) 	doTransStrFunc(0);
	else if (pamatch("strset(")) 	doTransStrFunc(1);
	else if (pamatch("strupr(")) 	doTransStrFunc(0);
	else if (amatch("setenv(")) {
		strpush("_sxb_setenv(");
		func3(SC_STR, SC_INT, SC_STR, SC_INT);
	}
	else if (amatch("shiftkeybit")) {
		strpush("_sxb_shiftkeybit()");
		lastClass = SC_INT;
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcT(void)
{
	if (pamatch("toascii(")) 	func1(SC_INT, SC_INT);
	else if (pamatch("tolower("))	func1(SC_INT, SC_INT);
	else if (pamatch("toupper("))	func1(SC_INT, SC_INT);
	else if (pamatch("tan(")) 	func1(SC_FLOAT, SC_FLOAT);
	else if (amatch("time$")) {
		strpush("timeS()");
		lastClass = SC_STR;
	}
	else if (amatch("taskid")) {
		strpush("taskid()");
		lastClass = SC_INT;
	}
	else
		return(FALSE);

	return(TRUE);
}

int
funcU(void)
{
	return(FALSE);
}

int
funcV(void)
{
	if (pamatch("val(")) 			func1(SC_FLOAT, SC_STR);
	else if (pamatch("varhdl("))	DoVarhdl();
	else
		return(FALSE);

	return(TRUE);
}

int
funcW(void)
{
	return(FALSE);
}

int
funcX(void)
{
	return(FALSE);
}

int
funcY(void)
{
	return(FALSE);
}

int
funcZ(void)
{
	return(FALSE);
}


/* External functions */

// Music functions
DEF_STATEMENT statementDefinitions[] = {
	STATEMENT2("m_alloc(", SC_INT, SC_INT),
	STATEMENT2("m_assign(", SC_INT, SC_INT),
	STATEMENT0("m_cont("),
	STATEMENT0("m_init("),
	STATEMENT0("m_play("),
	STATEMENT0("m_stop("),
	STATEMENT1("m_sysch(", SC_STR),
	STATEMENT2("m_trk(", SC_INT, SC_STR),
};

int
func_external(void) {
	int	i;
	int totalFunctions = sizeof(statementDefinitions) / sizeof(DEF_STATEMENT);
	for (i = 0; i < totalFunctions; i++) {
		DEF_STATEMENT* f = &statementDefinitions[i];
		if (pamatch(f->name)) {
			if (f->numParams == 0) {
				state0(f->retClass);
			}
			if (f->numParams == 1) 		state1(f->retClass, f->param1);
			if (f->numParams == 2) 		state2(f->retClass, f->param1, f->param2);
			if (f->numParams == 3)		state3(f->retClass, f->param1, f->param2, f->param3);
			if (f->numParams > 3)		PutError("Internal Error func_external %s %d", f->name, f->numParams);
			return TRUE;
		}
	}
	return FALSE;
}


/*
** ecvt(), fcvt()の処理
*/
void
doEcvt(void)
{
	int	i;
	SYMTBL* p[2];
	char* q;

	expression();
	ToFloat1(lastClass);
	check(",");
	expression();
	ToInt1(lastClass);

	for (i = 0; i < 2; i++) {
		check(",");
		if (p[i] = SearchLoc(TokenPtr));
		else if (p[i] = SearchGlo(TokenPtr));
		else
			PutError("未宣言の変数です");

		if (p[i]->class != SC_INT || p[i]->dim != 0)
			PutError("変数の型が違います");
		TokenPtr += TokenLen(TokenPtr);
	}
	check(")");
	q = strpop();
	strpush("%s,%s,%s,%s)", q, strpop(), p[0]->name, p[1]->name);
	lastClass = SC_STR;
}

/*
** 文字列変換（ステートメントの場合）
*/
void
doTransStr(int exps)
{
	SYMTBL* p;
	int		isGlobal;

	SkipSpace();
	if (p = SearchLoc(TokenPtr)) {
		isGlobal = FALSE;
	}
	else if (p = SearchGlo(TokenPtr)) {
		isGlobal = TRUE;
	}
	else PutError("未宣言の変数です");

	sxb_strcat("%s, sizeof(%s)", p->name, p->name);
	TokenPtr += TokenLen(TokenPtr);

	if (p->class != SC_STR)	PutError("変数の型が違います");

	if (p->dim >= 2) {
		check("(");
		DoIndexed(p);
	}

	while (exps--) {
		check(",");
		expression();
		ToInt1(lastClass);
		sxb_strcat(",%s", strpop());
	}
	check(")");
	sxb_strcat(")");
}

/*
** 文字列変換（関数の場合）
*/
void
doTransStrFunc(int exps)
{
	doTransStr(exps);
	lastClass = SC_STR;
}

/*
** function with 0 parameter.
*/
void
func0( SCLASS ret)
{
	check(")");
	sxb_strcat(")");

	lastClass = ret;
}

/*
** パラメータが一つの関数
*/
void
func1(SCLASS ret, SCLASS param1)
{
	expression();
	doCast(param1);
	check(")");
	sxb_strcat("%s)", strpop());

	lastClass = ret;
}

/*
** parse a function with 2 paramters
*/
void
func2(SCLASS ret, SCLASS par1, SCLASS par2)
{
	char* p;

	expression();
	doCast(par1);
	check(",");

	expression();
	doCast(par2);
	check(")");

	p = strpop();
	sxb_strcat("%s,%s)", strpop(), p);

	lastClass = ret;
}

/*
** パラメータが三つの関数
*/
void
func3(SCLASS par1, SCLASS par2, SCLASS par3, SCLASS ret)
{
	char	*p, *q;

	expression();
	doCast(par1);
	check(",");

	expression();
	doCast(par2);
	check(",");

	expression();
	doCast(par3);
	check(")");

	p = strpop();
	q = strpop();
	sxb_strcat("%s,%s,%s)", strpop(), q, p);
	lastClass = ret;
}

void
state0(SCLASS ret) {
	func0(ret);
	PutCode("%s;", strpop());
}
void
state1(SCLASS ret, SCLASS par1) {
	func1(ret, par1);
	PutCode("%s;", strpop());
}
void
state2(SCLASS ret, SCLASS par1, SCLASS par2)
{
	func2(ret, par1, par2);
	PutCode("%s;", strpop());
}
void
state3(SCLASS ret, SCLASS par1, SCLASS par2, SCLASS par3)
{
	func3(ret, par1, par2, par3);
	PutCode("%s;", strpop());
}

/*
** freadの処理
*/
void
doFread(void)
{
	SYMTBL* p;
	char* q;

	if (p = SearchLoc(TokenPtr));
	else if (p = SearchGlo(TokenPtr));
	else
		PutError("未宣言の変数です");

	TokenPtr += TokenLen(TokenPtr);
	sxb_strcat(p->name);

	if (p->class == SC_STR) PutError("数値型の配列を指定して下さい");
	if (p->dim == 0) PutError("１次元以上の配列を指定して下さい");
	else if (p->dim == 1);
	else {
		int	dim = p->dim;
		check("(");
		do {
			expression();
			sxb_strcat("[%s]", strpop());
			dim--;
		} while (amatch(","));
		if (dim > 1) PutError("要素が足りません");
		else if (dim < 0) PutError("要素が多すぎます");
		check(")");
	}

	check(",");
	expression();	/* size */
	ToInt1(lastClass);
	check(",");
	expression();	/* fn */
	ToInt1(lastClass);
	check(")");

	q = strpop();
	sxb_strcat(", sizeof(%s), %s,%s)", p->name, strpop(), q);
}


void
DoVarhdl(void)
{
	SYMTBL* p;
	int		isGlobal;

	if (p = SearchLoc(TokenPtr)) {
		isGlobal = FALSE;
	}
	else if (p = SearchGlo(TokenPtr)) {
		isGlobal = TRUE;
	}
	else PutError("未宣言の変数です");

	TokenPtr += TokenLen(TokenPtr);
	if (p->dim != 1) PutError("１次元の配列を指定して下さい");
	if (p->class != SC_INT)
		PutError("数値型の配列を指定して下さい");
	check(")");
	strpush("_sxb_varhdl(%s, sizeof(%d))", p->name, p->name);

	lastClass = SC_INT;
}
