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

	ret = ParseRegularFunctions(TRUE, TRUE);
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

DEF_FUNC1("abs(", "abs(",			ET_FLOAT, ET_FLOAT),
DEF_FUNC1("asc(", "_sxb_ascS(",		ET_INT, ET_STR),
DEF_FUNC1("atoi(", "atoi(",			ET_INT, ET_STR),
DEF_FUNC1("atof(", "atof(",			ET_FLOAT, ET_STR),
DEF_FUNC1("atan(", "atan(",			ET_FLOAT, ET_FLOAT),

DEF_FUNC1("bin$(", "_sxb_bin(",		ET_STR, ET_INT),

DEF_FUNC1("chr$(", "_sxb_chrS(",	ET_STR, ET_INT),
DEF_FUNC1("cos(",  "cos(",			ET_FLOAT, ET_FLOAT),

DEF_FUNC1("dskf(",	"dskf(",		ET_INT, ET_CHAR),
// DEF_FUNC0("date$",	"dateS()",	ET_STR),
// DEF_FUNC0("days$",	"days()",	ET_STR),
DEF_FUNC1("exit(", "b_exit(",		ET_NONE, ET_INT),

DEF_FUNC1("exp(",	"exp(",			ET_FLOAT, ET_FLOAT),

DEF_FUNC1("fix(",	"fix(",			ET_FLOAT, ET_FLOAT),
//		DEF_FUNC1("fcvt(")) doEcvt();
DEF_FUNC1("fclose(", "fclose(", 	ET_INT, ET_INT),
DEF_FUNC0("fcloseall(", "flocseall(", ET_INT),

//	'fread', 'fwrie' and 'freads' have the designated functions
DEF_FUNC1("fdelete(", "remove(",	ET_INT, ET_STR),
DEF_FUNC1("feof(", "feof(", 		ET_INT, ET_INT),
DEF_FUNC1("fgetc(", "_sxb_fgetc(",	ET_INT, ET_INT),
DEF_FUNC2("fopen(", "_sxb_fopen(",	ET_INT, ET_STR, ET_STR),
DEF_FUNC2("fputc(", "_sxb_fputc(",	ET_INT, ET_CHAR, ET_INT),
DEF_FUNC2("frename(","_sxb_frename(", ET_INT, ET_STR, ET_STR),
DEF_FUNC3("fseek(", "_sxb_fseek(",	ET_INT, ET_INT, ET_INT, ET_INT),
DEF_FUNC2("fwrites(","_sxb_fwrites(", ET_INT, ET_STR, ET_INT),

DEF_FUNC2("gcvt(", "gcvt(", 		ET_STR, ET_FLOAT, ET_INT),

DEF_FUNC1("hex$(", "_sxb_hexS(", 	ET_STR, ET_INT),

// inkey$
DEF_FUNC1("int(", "int(", 			ET_INT, ET_FLOAT),
DEF_FUNC1("itoa(", "itoa(", 		ET_STR, ET_INT),
DEF_FUNC3("instr(", "instr(",		ET_INT, ET_INT, ET_STR, ET_STR),
DEF_FUNC1("isalnum(", "isalnum(", 	ET_INT, ET_CHAR),
DEF_FUNC1("isalpha(", "isalpha(", 	ET_INT, ET_CHAR),
DEF_FUNC1("isascii(", "isascii(", 	ET_INT, ET_CHAR),
DEF_FUNC1("iscntrl(", "iscntrl(", 	ET_INT, ET_CHAR),
DEF_FUNC1("isdigit(", "isdigit(", 	ET_INT, ET_CHAR),
DEF_FUNC1("isgraph(", "isgraph(", 	ET_INT, ET_CHAR),
DEF_FUNC1("islower(", "islower(", 	ET_INT, ET_CHAR),
DEF_FUNC1("isprint(", "isprint(", 	ET_INT, ET_CHAR),
DEF_FUNC1("ispunct(", "ispunct(", 	ET_INT, ET_CHAR),
DEF_FUNC1("isspace(", "isspace(", 	ET_INT, ET_CHAR),
DEF_FUNC1("isupper(", "isupper(", 	ET_INT, ET_CHAR),
DEF_FUNC1("isxdigit(", "isxdigit(", ET_INT, ET_CHAR),

DEF_FUNC1("len(", "len(", 			ET_INT, ET_STR),
DEF_FUNC1("log(", "log(", 			ET_FLOAT, ET_FLOAT),
DEF_FUNC2("left$(", "_sxb_leftS(", 	ET_STR, ET_STR, ET_INT),

DEF_FUNC3("mid$(", "_sxb_midS(", 	ET_STR, ET_STR, ET_INT, ET_INT),
DEF_FUNC1("mirror$(", "_sxb_mirrorS(", ET_STR, ET_STR),
DEF_FUNC1("mouse(", "mouse(", ET_CHAR, ET_INT),

DEF_FUNC1("oct$(", "_sxb_octS(", 	ET_STR, ET_INT),
DEF_FUNC2("pow(", "pow(", 			ET_FLOAT, ET_FLOAT, ET_FLOAT),

DEF_FUNC0("rnd(", "rnd(", 			ET_FLOAT),
DEF_FUNC0("rand(", "rand(", 		ET_INT),
DEF_FUNC2("right$(", "_sxb_rightS(", ET_STR, ET_STR, ET_INT),
DEF_FUNC1("randomize(", "randomize(", ET_NONE, ET_INT),

DEF_FUNC1("str$(", "_sxb_str(", 	ET_STR, ET_FLOAT),
DEF_FUNC1("sgn(", "sgn(", 			ET_FLOAT, ET_FLOAT),
DEF_FUNC1("sin(", "sin(",			ET_FLOAT, ET_FLOAT),
DEF_FUNC1("sqr(", "sqr(", 			ET_FLOAT, ET_FLOAT),
DEF_FUNC1("space$(", "_sxb_spaceS(", ET_STR, ET_INT),
DEF_FUNC1("srand(", "srand(",		ET_NONE, ET_INT),
DEF_FUNC2("strchr(", "strchr(", 	ET_INT, ET_STR, ET_CHAR),
DEF_FUNC2("strcspn(", "strcspn(", 	ET_INT, ET_STR, ET_STR),
DEF_FUNC2("string$(", "_sxb_stringS(", ET_STR, ET_INT, ET_STR),
DEF_FUNC1("strlen(", "strlen(", 	ET_INT, ET_STR),
DEF_FUNC2("strrchr(", "strrchr(",	ET_INT, ET_STR, ET_CHAR),
DEF_FUNC2("strspn(", "strspn(",		ET_INT, ET_STR, ET_STR),
DEF_FUNC2("strtok(", "strtok(", 	ET_STR, ET_STR, ET_STR),

DEF_FUNC1("toascii(", "toascii(",	ET_INT, ET_CHAR),
DEF_FUNC1("tolower(", "tolower(",	ET_INT, ET_CHAR),
DEF_FUNC1("toupper(", "toupper(",	ET_INT, ET_CHAR),
DEF_FUNC1("tan(", "tan(",			ET_FLOAT, ET_FLOAT),

DEF_FUNC1("val(", "val(", ET_FLOAT, ET_STR),

// image functions
DEF_FUNC1("img_color(", "img_color(",	ET_NONE, ET_CHAR),
DEF_FUNC5("img_home(",	"img_home(",	ET_NONE, ET_INT, ET_INT, ET_CHAR, ET_INT, ET_INT),
DEF_FUNC6("img_ht(",	"img_ht(",		ET_NONE, ET_INT, ET_INT, ET_INT, ET_INT, ET_CHAR, ET_CHAR),
DEF_FUNC1("img_load(",	"img_load(",	ET_INT, ET_STR),				// FXIME: 2nd, 3rd, 4th parameters optional
DEF_FUNC1("img_pos(",	"img_pos(",		ET_NONE, ET_CHAR),
// DEF_FUNC2("img_put(",	"img_put(",	ET_INT, ET_CHAR, ET_INT),		// FIXME: array at 5th parameter
DEF_FUNC1("img_save(",	"img_save(",	ET_INT, ET_STR),				// FIXME: 2nd, 3rd parameters optional
DEF_FUNC3("img_scrn(",	"img_scrn(",	ET_INT, ET_INT, ET_INT, ET_INT),
// DEF_FUNC2("img_set(",	"img_set(",	ET_INT, ET_CHAR, ET_INT),
DEF_FUNC1("img_still(", "img_still(",	ET_NONE, ET_INT),
DEF_FUNC1("v_cut(",		"v_cut(",		ET_NONE, ET_CHAR),


// music functions
DEF_FUNC2("m_alloc(", "m_alloc(", ET_INT, ET_CHAR, ET_INT),
DEF_FUNC2("m_assign(", "m_assign(", ET_INT, ET_CHAR, ET_CHAR),
DEF_FUNC0("m_cont(", "m_cont(", 	ET_INT),		// FIXME : Shall take variable number of parameters.
DEF_FUNC1("m_free(", "m_free(", 	ET_INT, ET_CHAR),
DEF_FUNC0("m_init(", "m_init(",		ET_NONE),
DEF_FUNC0("m_play(", "m_play(", 	ET_INT),		// FIXME : Shall take variable number of paramters.
DEF_FUNC1("m_stat(", "m_stat(",		ET_INT, ET_INT),
DEF_FUNC0("m_stop(", "m_stop(",		ET_INT),		// FIXME : Shall take variable number of paramters.
DEF_FUNC1("m_sysch(", "m_sysch(",	ET_INT, ET_STR),
DEF_FUNC1("m_tempo(", "m_trk(",		ET_INT, ET_CHAR),
DEF_FUNC2("m_trk(", "m_trk(",		ET_INT, ET_INT, ET_STR),

};


int
ParseRegularFunctions(bool forExpression, bool needReturnValue) {
	int	i;
	int totalFunctions = sizeof(funcDefinitions) / sizeof(DEF_FUNCTIONS);
	for (i = 0; i < totalFunctions; i++) {
		DEF_FUNCTIONS* f = &funcDefinitions[i];
		if (f->retType == ET_NONE) {
			if (forExpression) {
				continue;
			}
			if (needReturnValue) {
				PutError("A function/statement cannot be used inside the expression.");
			}
		}
		if (amatch(f->b_name)) {
			strpush(f->c_name);
			if (f->numParams == 0) 	func0(f->retType);
			if (f->numParams == 1) 	func1(f->retType, f->param1);
			if (f->numParams == 2) 	func2(f->retType, f->param1, f->param2);
			if (f->numParams == 3)	func3(f->retType, f->param1, f->param2, f->param3);
			if (f->numParams == 4)	func4(f->retType, f->param1, f->param2, f->param3, f->param4);
			if (f->numParams == 5)	func5(f->retType, f->param1, f->param2, f->param3, f->param4, f->param5);
			if (f->numParams == 6)	func6(f->retType, f->param1, f->param2, f->param3, f->param4, f->param5, f->param6);
			if (f->numParams > 6)	PutError("Internal Error ParseRegularFunctions %s %d", f->b_name, f->numParams);

			if (forExpression == FALSE) {
				PutCode("%s;", strpop());
			}
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
		doArrayIndex(p);
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
** parse a function with 0 parameter
*/
void
func0(E_TYPE ret)
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
func3(E_TYPE ret, E_TYPE p1, E_TYPE p2, E_TYPE p3)
{
	char	*p, *q;

	expression();	doCast(p1);		check(",");
	expression();	doCast(p2);		check(",");
	expression();	doCast(p3);		check(")");

	p = strpop();
	q = strpop();
	sxb_strcat("%s,%s,%s)", strpop(), q, p);
	lastType = ret;
}

/*
** Parse a function with 4 parameters
*/
void
func4(E_TYPE ret, E_TYPE p1, E_TYPE p2, E_TYPE p3, E_TYPE p4)
{
	char	*p, *q, *r;

	expression();	doCast(p1);	check(",");
	expression();	doCast(p2);	check(",");
	expression();	doCast(p3);	check(",");
	expression();	doCast(p4);	check(")");

	p = strpop();
	q = strpop();
	r = strpop();
	sxb_strcat("%s,%s,%s,%s)", strpop(), r, q, p);
	lastType = ret;
}

/*
** Parse a function with 5 parameters
*  (FIXME - Do more decent with variable number of parameters)
*/
void
func5(E_TYPE ret, E_TYPE p1, E_TYPE p2, E_TYPE p3, E_TYPE p4, E_TYPE p5)
{
	char* p, * q, * r, *s;

	expression();	doCast(p1);	check(",");
	expression();	doCast(p2);	check(",");
	expression();	doCast(p3);	check(",");
	expression();	doCast(p4);	check(",");
	expression();	doCast(p5);	check(")");

	p = strpop();
	q = strpop();
	r = strpop();
	s = strpop();
	sxb_strcat("%s,%s,%s,%s,%s)", strpop(), s, r, q, p);
	lastType = ret;
}

/*
** Parse a function with 5 parameters
*  (FIXME - Do more decent with variable number of parameters)
*/
void
func6(E_TYPE ret, E_TYPE p1, E_TYPE p2, E_TYPE p3, E_TYPE p4, E_TYPE p5, E_TYPE p6)
{
	char* p, * q, * r, * s, *t;

	expression();	doCast(p1);	check(",");
	expression();	doCast(p2);	check(",");
	expression();	doCast(p3);	check(",");
	expression();	doCast(p4);	check(",");
	expression();	doCast(p5);	check(",");
	expression();	doCast(p6);	check(")");

	p = strpop();
	q = strpop();
	r = strpop();
	s = strpop();
	t = strpop();
	sxb_strcat("%s,%s,%s,%s,%s,%s)", strpop(), t, s, r, q, p);
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
