/*
	SX-BASIC to C converter (Functions)
	Programmed By ISHIGAMI Tatsuya
	02/20/1994	First published
	01/20/2025	Updated for Oh!X Reiwa anniversary.

*/

#include	<stdio.h>
#include	<string.h>
#include	<time.h>
#include	<setjmp.h>    

#include	"sxbasic.h"

extern	char* TokenPtr;
extern	SCLASS	lastClass;
extern	FILE* outputFp;

int
efuncs(void)
{
	int		ret = FALSE;

	SkipSpace();

	ret = ParseRegularFunctions();
	if (ret) return TRUE;

	ret = ParseSpecialFunctions();
	return(ret);
}


/*
 * Parse functions with special translation rules.
 * 
 */
int
ParseSpecialFunctions(void)
{
	if (amatch("date$")) {
		strpush("dataS()");
		lastClass = SC_STR;
	} else if (amatch("day$")) {
		strpush("dayS()");
		lastClass = SC_STR;
	} else if (pamatch("ecvt(")) {
		doEcvt();
	} else if (pamatch("fcvt(")) {
		doEcvt();
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
	else if (amatch("inkey$(0)")) {
		strpush("_sxb_inkey0()");
		lastClass = SC_STR;
	}
	else if (amatch("inkey$")) {
		strpush("_sxb_inkey()");
		lastClass = SC_STR;
	}
	else if (pamatch("pi(")) {
		if (amatch(")")) {
			sxb_strcat("1)");
			lastClass = SC_FLOAT;
		} else {
			func1(SC_FLOAT, SC_FLOAT);
		}
	}
	else if (pamatch("strlwr(")) {
		doTransStrFunc(0); 
	}
	else if (pamatch("strnset("))	doTransStrFunc(2);
	else if (pamatch("strrev(")) 	doTransStrFunc(0);
	else if (pamatch("strset(")) 	doTransStrFunc(1);
	else if (pamatch("strupr(")) 	doTransStrFunc(0);
	else if (amatch("time$")) {
		strpush("timeS()");
		lastClass = SC_STR;
	} else
		return(FALSE);

	return(TRUE);
}




// regular functions
DEF_STATEMENT funcDefinitions[] = {

DEF_FUNC1("abs(", "abs(", SC_FLOAT, SC_FLOAT),
DEF_FUNC1("asc(", "asc(", SC_INT, SC_STR),
DEF_FUNC1("atoi(", "atoi(", SC_INT, SC_STR),
DEF_FUNC1("atof(", "atof(", SC_FLOAT, SC_STR),
DEF_FUNC1("atan(", "atan(", SC_FLOAT, SC_FLOAT),

DEF_FUNC1("bin$(", "_sxb_bin(", SC_STR, SC_INT),

DEF_FUNC1("chr$(", "_sxb_chrS(", SC_STR, SC_INT),
DEF_FUNC1("cos(",  "cos(",		SC_FLOAT, SC_FLOAT),

DEF_FUNC1("dskf(",	"dskf(",	SC_INT, SC_CHAR),
// DEF_FUNC0("date$",	"dateS()",	SC_STR),
// DEF_FUNC0("days$",	"days()",	SC_STR),
DEF_FUNC1("exp(",	"exp(",		SC_FLOAT, SC_FLOAT),

DEF_FUNC1("fix(",	"fix(",	SC_FLOAT, SC_FLOAT),
//		DEF_FUNC1("fcvt(")) doEcvt();
DEF_FUNC1("fclose(", "fclose(", SC_INT, SC_INT),
DEF_FUNC0("fcloseall(", "flocseall(", SC_INT),

//	'fread', 'fwrie' and 'freads' have the designated functions
DEF_FUNC1("fdelete(", "TSDeleteP(", SC_INT, SC_STR),
DEF_FUNC1("feof(", "feof(", SC_INT, SC_INT),
DEF_FUNC1("fgetc(", "_sxb_fgetc(", SC_INT, SC_INT),
DEF_FUNC2("fopen(", "_sxb_fopen(", SC_INT, SC_STR, SC_STR),
DEF_FUNC2("fputc(", "_sxb_fputc(", SC_INT, SC_CHAR, SC_INT),
DEF_FUNC2("frename(","_sxb_frename(", SC_INT, SC_STR, SC_STR),
DEF_FUNC3("fseek(", "_sxb_fseek(",SC_INT, SC_INT, SC_INT, SC_INT),
DEF_FUNC2("fwrites(","_sxb_fwrites(", SC_INT, SC_STR, SC_INT),

DEF_FUNC2("gcvt(", "gcvt(", SC_STR, SC_FLOAT, SC_INT),

DEF_FUNC1("hex$(", "_sxb_hex(", SC_STR, SC_INT),

// inkey$
DEF_FUNC1("int(", "int(", SC_INT, SC_FLOAT),
DEF_FUNC1("itoa(", "itoa(", SC_STR, SC_INT),
DEF_FUNC3("instr(", "instr(",SC_INT, SC_INT, SC_STR, SC_STR),
DEF_FUNC1("isalnum(", "isalnum(", SC_INT, SC_CHAR),
DEF_FUNC1("isalpha(", "isalpha(", SC_INT, SC_CHAR),
DEF_FUNC1("isascii(", "isascii(", SC_INT, SC_CHAR),
DEF_FUNC1("iscntrl(", "iscntrl(", SC_INT, SC_CHAR),
DEF_FUNC1("isdigit(", "isdigit(", SC_INT, SC_CHAR),
DEF_FUNC1("isgraph(", "isgraph(", SC_INT, SC_CHAR),
DEF_FUNC1("islower(", "islower(", SC_INT, SC_CHAR),
DEF_FUNC1("isprint(", "isprint(", SC_INT, SC_CHAR),
DEF_FUNC1("ispunct(", "ispunct(", SC_INT, SC_CHAR),
DEF_FUNC1("isspace(", "isspace(", SC_INT, SC_CHAR),
DEF_FUNC1("isupper(", "isupper(", SC_INT, SC_CHAR),
DEF_FUNC1("isxdigit(", "isxdigit(", SC_INT, SC_CHAR),

DEF_FUNC1("len(", "len(", SC_INT, SC_STR),
DEF_FUNC1("log(", "log(", SC_FLOAT, SC_FLOAT),
DEF_FUNC2("left$(", "left$(", SC_STR, SC_STR, SC_INT),

DEF_FUNC3("mid$(", "_sxb_mid(", SC_STR, SC_STR, SC_INT, SC_INT),
DEF_FUNC1("mirror$(", "_sxb_mirror(", SC_STR, SC_STR),
DEF_FUNC1("mouse(", "mouse(", SC_CHAR, SC_INT),

DEF_FUNC1("oct$(", "_sxb_oct(", SC_STR, SC_INT),
DEF_FUNC2("pow(", "pow(", SC_FLOAT, SC_FLOAT, SC_FLOAT),

DEF_FUNC0("rnd(", "rnd(", SC_FLOAT),
DEF_FUNC0("rand(", "rand(", SC_INT),
DEF_FUNC2("right$(", "_sxb_rightS(", SC_STR, SC_STR, SC_INT),

//
DEF_FUNC1("str$(", "_sxb_str(", SC_STR, SC_FLOAT),
DEF_FUNC1("sgn(", "sgn(", SC_FLOAT, SC_FLOAT),
DEF_FUNC1("sin(", "sin(",SC_FLOAT, SC_FLOAT),
DEF_FUNC1("sqr(", "sqr(", SC_FLOAT, SC_FLOAT),
DEF_FUNC1("space$(", "_sxb_spaceS(", SC_STR, SC_INT),
DEF_FUNC2("strchr(", "strchr(", SC_INT, SC_STR, SC_CHAR),
DEF_FUNC2("strcspn(", "strcspn(", SC_INT, SC_STR, SC_STR),
DEF_FUNC2("string$(", "_sxb_stringS(", SC_STR, SC_INT, SC_STR),
DEF_FUNC1("strlen(", "strlen(", SC_INT, SC_STR),
DEF_FUNC2("strrchr(", "strrchr(", SC_INT, SC_STR, SC_CHAR),
DEF_FUNC2("strspn(", "strspn(", SC_INT, SC_STR, SC_STR),
DEF_FUNC2("strtok(", "strtok(", SC_STR, SC_STR, SC_STR),

DEF_FUNC1("toascii(", "toascii(", SC_INT, SC_CHAR),
DEF_FUNC1("tolower(", "tolower(", SC_INT, SC_CHAR),
DEF_FUNC1("toupper(", "toupper(", SC_INT, SC_CHAR),
DEF_FUNC1("tan(", "tan(", SC_FLOAT, SC_FLOAT),

DEF_FUNC1("val(", "val(", SC_FLOAT, SC_STR)
};


int
ParseRegularFunctions(void) {
	int	i;
	int totalFunctions = sizeof(funcDefinitions) / sizeof(DEF_STATEMENT);
	for (i = 0; i < totalFunctions; i++) {
		DEF_STATEMENT* f = &funcDefinitions[i];
		if (amatch(f->b_name)) {
			strpush(f->c_name);
			if (f->numParams == 0) 	func0(f->retClass);
			if (f->numParams == 1) 	func1(f->retClass, f->param1);
			if (f->numParams == 2) 	func2(f->retClass, f->param1, f->param2);
			if (f->numParams == 3)	func3(f->retClass, f->param1, f->param2, f->param3);
			if (f->numParams > 3)	PutError("Internal Error func_external %s %d", f->b_name, f->numParams);
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
	char *q1, *q2;

	expression();
	ToFloat1(lastClass);
	check(",");
	expression();
	ToInt1(lastClass);

	for (i = 0; i < 2; i++) {
		check(",");
		SkipSpace();
		if (p[i] = SearchLoc(TokenPtr)); 
		else if (p[i] = SearchGlo(TokenPtr));
		else
			PutError("未宣言の変数です !!");

		if (p[i]->class != SC_INT || p[i]->dim != 0)
			PutError("変数の型が違います");
		TokenPtr += TokenLen(TokenPtr);
	}
	check(")");
	q1 = strpop();
	q2 = strpop();
	strpush("%s%s, %s, \&%s, \&%s)", strpop(), q2, q1, p[0]->name, p[1]->name);
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
		sxb_strcat(" ,%s", strpop());
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
** parse a function with 1 paramter
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
** Parse a function with 3 parameters
*/
void
func3(SCLASS ret, SCLASS par1, SCLASS par2, SCLASS par3)
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

	if (p->class == SC_STR)  PutError("数値型の配列を指定して下さい");
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
