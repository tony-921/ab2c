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
extern	E_TYPE	lastType;
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
		lastType = ET_STR;
	} else if (amatch("day$")) {
		strpush("dayS()");
		lastType = ET_STR;
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
		lastType = ET_INT;
	}
	else if (amatch("inkey$(0)")) {
		strpush("_sxb_inkey0()");
		lastType = ET_STR;
	}
	else if (amatch("inkey$")) {
		strpush("_sxb_inkey()");
		lastType = ET_STR;
	}
	else if (pamatch("pi(")) {
		if (amatch(")")) {
			sxb_strcat("1)");
			lastType = ET_FLOAT;
		} else {
			func1(ET_FLOAT, ET_FLOAT);
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
		lastType = ET_STR;
	} else
		return(FALSE);

	return(TRUE);
}




// regular functions
DEF_FUNCTIONS funcDefinitions[] = {

DEF_FUNC1("abs(", "abs(", ET_FLOAT, ET_FLOAT),
DEF_FUNC1("asc(", "asc(", ET_INT, ET_STR),
DEF_FUNC1("atoi(", "atoi(", ET_INT, ET_STR),
DEF_FUNC1("atof(", "atof(", ET_FLOAT, ET_STR),
DEF_FUNC1("atan(", "atan(", ET_FLOAT, ET_FLOAT),

DEF_FUNC1("bin$(", "_sxb_bin(", ET_STR, ET_INT),

DEF_FUNC1("chr$(", "_sxb_chrS(", ET_STR, ET_INT),
DEF_FUNC1("cos(",  "cos(",		ET_FLOAT, ET_FLOAT),

DEF_FUNC1("dskf(",	"dskf(",	ET_INT, ET_CHAR),
// DEF_FUNC0("date$",	"dateS()",	ET_STR),
// DEF_FUNC0("days$",	"days()",	ET_STR),
DEF_FUNC1("exp(",	"exp(",		ET_FLOAT, ET_FLOAT),

DEF_FUNC1("fix(",	"fix(",	ET_FLOAT, ET_FLOAT),
//		DEF_FUNC1("fcvt(")) doEcvt();
DEF_FUNC1("fclose(", "fclose(", ET_INT, ET_INT),
DEF_FUNC0("fcloseall(", "flocseall(", ET_INT),

//	'fread', 'fwrie' and 'freads' have the designated functions
DEF_FUNC1("fdelete(", "TSDeleteP(", ET_INT, ET_STR),
DEF_FUNC1("feof(", "feof(", ET_INT, ET_INT),
DEF_FUNC1("fgetc(", "_sxb_fgetc(", ET_INT, ET_INT),
DEF_FUNC2("fopen(", "_sxb_fopen(", ET_INT, ET_STR, ET_STR),
DEF_FUNC2("fputc(", "_sxb_fputc(", ET_INT, ET_CHAR, ET_INT),
DEF_FUNC2("frename(","_sxb_frename(", ET_INT, ET_STR, ET_STR),
DEF_FUNC3("fseek(", "_sxb_fseek(",ET_INT, ET_INT, ET_INT, ET_INT),
DEF_FUNC2("fwrites(","_sxb_fwrites(", ET_INT, ET_STR, ET_INT),

DEF_FUNC2("gcvt(", "gcvt(", ET_STR, ET_FLOAT, ET_INT),

DEF_FUNC1("hex$(", "_sxb_hex(", ET_STR, ET_INT),

// inkey$
DEF_FUNC1("int(", "int(", ET_INT, ET_FLOAT),
DEF_FUNC1("itoa(", "itoa(", ET_STR, ET_INT),
DEF_FUNC3("instr(", "instr(",ET_INT, ET_INT, ET_STR, ET_STR),
DEF_FUNC1("isalnum(", "isalnum(", ET_INT, ET_CHAR),
DEF_FUNC1("isalpha(", "isalpha(", ET_INT, ET_CHAR),
DEF_FUNC1("isascii(", "isascii(", ET_INT, ET_CHAR),
DEF_FUNC1("iscntrl(", "iscntrl(", ET_INT, ET_CHAR),
DEF_FUNC1("isdigit(", "isdigit(", ET_INT, ET_CHAR),
DEF_FUNC1("isgraph(", "isgraph(", ET_INT, ET_CHAR),
DEF_FUNC1("islower(", "islower(", ET_INT, ET_CHAR),
DEF_FUNC1("isprint(", "isprint(", ET_INT, ET_CHAR),
DEF_FUNC1("ispunct(", "ispunct(", ET_INT, ET_CHAR),
DEF_FUNC1("isspace(", "isspace(", ET_INT, ET_CHAR),
DEF_FUNC1("isupper(", "isupper(", ET_INT, ET_CHAR),
DEF_FUNC1("isxdigit(", "isxdigit(", ET_INT, ET_CHAR),

DEF_FUNC1("len(", "len(", ET_INT, ET_STR),
DEF_FUNC1("log(", "log(", ET_FLOAT, ET_FLOAT),
DEF_FUNC2("left$(", "left$(", ET_STR, ET_STR, ET_INT),

DEF_FUNC3("mid$(", "_sxb_mid(", ET_STR, ET_STR, ET_INT, ET_INT),
DEF_FUNC1("mirror$(", "_sxb_mirror(", ET_STR, ET_STR),
DEF_FUNC1("mouse(", "mouse(", ET_CHAR, ET_INT),

DEF_FUNC1("oct$(", "_sxb_oct(", ET_STR, ET_INT),
DEF_FUNC2("pow(", "pow(", ET_FLOAT, ET_FLOAT, ET_FLOAT),

DEF_FUNC0("rnd(", "rnd(", ET_FLOAT),
DEF_FUNC0("rand(", "rand(", ET_INT),
DEF_FUNC2("right$(", "_sxb_rightS(", ET_STR, ET_STR, ET_INT),

//
DEF_FUNC1("str$(", "_sxb_str(", ET_STR, ET_FLOAT),
DEF_FUNC1("sgn(", "sgn(", ET_FLOAT, ET_FLOAT),
DEF_FUNC1("sin(", "sin(",ET_FLOAT, ET_FLOAT),
DEF_FUNC1("sqr(", "sqr(", ET_FLOAT, ET_FLOAT),
DEF_FUNC1("space$(", "_sxb_spaceS(", ET_STR, ET_INT),
DEF_FUNC2("strchr(", "strchr(", ET_INT, ET_STR, ET_CHAR),
DEF_FUNC2("strcspn(", "strcspn(", ET_INT, ET_STR, ET_STR),
DEF_FUNC2("string$(", "_sxb_stringS(", ET_STR, ET_INT, ET_STR),
DEF_FUNC1("strlen(", "strlen(", ET_INT, ET_STR),
DEF_FUNC2("strrchr(", "strrchr(", ET_INT, ET_STR, ET_CHAR),
DEF_FUNC2("strspn(", "strspn(", ET_INT, ET_STR, ET_STR),
DEF_FUNC2("strtok(", "strtok(", ET_STR, ET_STR, ET_STR),

DEF_FUNC1("toascii(", "toascii(", ET_INT, ET_CHAR),
DEF_FUNC1("tolower(", "tolower(", ET_INT, ET_CHAR),
DEF_FUNC1("toupper(", "toupper(", ET_INT, ET_CHAR),
DEF_FUNC1("tan(", "tan(", ET_FLOAT, ET_FLOAT),

DEF_FUNC1("val(", "val(", ET_FLOAT, ET_STR)
};


int
ParseRegularFunctions(void) {
	int	i;
	int totalFunctions = sizeof(funcDefinitions) / sizeof(DEF_FUNCTIONS);
	for (i = 0; i < totalFunctions; i++) {
		DEF_FUNCTIONS* f = &funcDefinitions[i];
		if (amatch(f->b_name)) {
			strpush(f->c_name);
			if (f->numParams == 0) 	func0(f->retType);
			if (f->numParams == 1) 	func1(f->retType, f->param1);
			if (f->numParams == 2) 	func2(f->retType, f->param1, f->param2);
			if (f->numParams == 3)	func3(f->retType, f->param1, f->param2, f->param3);
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
	ToFloat1(lastType);
	check(",");
	expression();
	ToInt1(lastType);

	for (i = 0; i < 2; i++) {
		check(",");
		SkipSpace();
		if (p[i] = SearchLoc(TokenPtr)); 
		else if (p[i] = SearchGlo(TokenPtr));
		else
			PutError("未宣言の変数です !!");

		if (p[i]->type != ET_INT || p[i]->dim != 0)
			PutError("変数の型が違います");
		TokenPtr += TokenLen(TokenPtr);
	}
	check(")");
	q1 = strpop();
	q2 = strpop();
	strpush("%s%s, %s, \&%s, \&%s)", strpop(), q2, q1, p[0]->name, p[1]->name);
	lastType = ET_STR;
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

	if (p->type != ET_STR)	PutError("変数の型が違います");

	if (p->dim >= 2) {
		check("(");
		DoIndexed(p);
	}

	while (exps--) {
		check(",");
		expression();
		ToInt1(lastType);
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
	lastType = ET_STR;
}

/*
** function with 0 parameter.
*/
void
func0( E_TYPE ret)
{
	check(")");
	sxb_strcat(")");

	lastType = ret;
}

/*
** parse a function with 1 paramter
*/
void
func1(E_TYPE ret, E_TYPE param1)
{
	expression();
	doCast(param1);
	check(")");
	sxb_strcat("%s)", strpop());

	lastType = ret;
}

/*
** parse a function with 2 paramters
*/
void
func2(E_TYPE ret, E_TYPE par1, E_TYPE par2)
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

	lastType = ret;
}

/*
** Parse a function with 3 parameters
*/
void
func3(E_TYPE ret, E_TYPE par1, E_TYPE par2, E_TYPE par3)
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
	lastType = ret;
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

	if (p->type == ET_STR)  PutError("数値型の配列を指定して下さい");
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
	ToInt1(lastType);
	check(",");
	expression();	/* fn */
	ToInt1(lastType);
	check(")");

	q = strpop();
	sxb_strcat(", sizeof(%s), %s,%s)", p->name, strpop(), q);
}
