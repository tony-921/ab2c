/*
	SX-BASIC Compiler
	the Evaluation of Expression Part

	Programmed By ISHIGAMI Tatsuya
	08/26/95

*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>
#include	<stdarg.h>

#include	"sxbasic.h"

int		vali;
double	valf;

char	strBuffer[STR_BUFFER_SIZE];
char* strStackPtr;

extern	char* TokenPtr;
extern	SCLASS	lastClass;
extern	char	prgBuff[];
extern	FILE* outputFp;
extern	int		indent;


volatile void
PutError(char* fmt, ...) {
	// FIXME :
	// place holder for the original Japanese string, which
	// doesn't display on Windows.
	char* p;
	va_list	marker;

	va_start(marker, fmt);

	fprintf(stderr, "\n%s\n", prgBuff);
	for (p = prgBuff; p < TokenPtr; p++) {
		if (*p == '\t')	fprintf(stderr, "\t");
		else			fprintf(stderr, " ");
	}
	fprintf(stderr, "^\n");
	vfprintf(stderr, fmt, marker);
	fprintf(stderr, "\n");

	exit(-1);
}

/*
** エラーメッセージを表示（コンパイル時）
*/
// FIXME : change back to PutError()
volatile void
PutErrorE(char* fmt, ...)
{
	char* p;
	va_list	marker;

	va_start(marker, fmt);

	fprintf(stderr, "\n%s\n", prgBuff);
	for (p = prgBuff; p < TokenPtr; p++) {
		if (*p == '\t')	fprintf(stderr, "\t");
		else			fprintf(stderr, " ");
	}
	fprintf(stderr, "^\n");
	vfprintf(stderr, fmt, marker);
	fprintf(stderr, "\n");

	exit(-1);
}

/*
** 文法エラー
*/
volatile void
SynErr(void)
{
	PutErrorE("Syntax Error");
}



/*
**　期待文字列を確認
*/
void
check(char* s)
{
	char	buf[50];

	if (!amatch(s)) {
		sprintf(buf, "keyword %s expected", s);
		PutErrorE(buf);
	}
}
void
fcheck(char* s)
{
	char	buf[50];

	if (!amatch(s)) {
		sprintf(buf, "%sがありません", s);
		PutError(buf);
		sprintf(buf, "Misisng %s", s);
		PutErrorE(buf);
	}
	PutCode(s);
}

/*
** <factor> の処理ルーチン
*/
void
factor(void)
{
	SYMTBL* p;
	char	c;

	if (aconst()) {
		if (lastClass == SC_CHAR || lastClass == SC_INT) {
			strpush("%d", vali);
		}
		else {
			strpush("%f", valf);
		}
	}
	else if (p = SearchLoc(TokenPtr)) {
		RefVariable(FALSE, p);
	}
	else if (p = SearchGlo(TokenPtr)) {
		RefVariable(TRUE, p);
	}
	else if (amatch("(")) {
		expression();
		check(")");
		strpush("(%s)", strpop());
	}
	else if (amatch("-")) {
		factor();
		strpush("-%s", strpop());
		if (lastClass == SC_STR) {
			PutError("変数の型が違います");
		}
	}
	else if (amatch("\"")) {
		lastClass = doString();
	}
	else if (efuncs()) {	/* 組み込み関数 */
	}
	else if (c = TokenPtr[TokenLen(TokenPtr)], c == '(') {
		FNCTBL* q;
		q = pcall();
		lastClass = q->retClass;
	}
	else if (c == '.' || c == '[') {
		RefProp();
	}
	else SynErr();
}

/*
 * 変数の値を参照する
 */
void
RefVariable(int isGlobal, SYMTBL* p)
{
	int	dim = p->dim;

	TokenPtr += TokenLen(TokenPtr);

	lastClass = p->class;
	if (lastClass == SC_STR)	dim--;

	if (dim == 0) {	/* simple variable (not an array) */
		strpush("%s", p->name);
		if (p->class == SC_STR && amatch("[")) {
			/* str[0]のような形　→　CHAR型として扱う */
			expression();
			check("]");
			sxb_strcat("[%s]", strpop());
			lastClass = SC_CHAR;
		}
	}
	else if (amatch("(")) {
		/* array variable */
		DoIndexed(p);
		lastClass = p->class;
		if (p->class == SC_STR && amatch("[")) {
			expression();
			check("]");
			sxb_strcat("[%s]", strpop());
			lastClass = SC_CHAR;
		}
	}
	else
		PutError("%sは配列変数です", p->name);
}

/*
** (a1,a2,a3)の処理
*/
void
DoIndexed(SYMTBL* p)
{
	int	dim = p->dim;

	if (p->class == SC_STR) dim = p->dim - 1;

	/*	check("(");	は事前に行っている */

	strpush(p->name);
	do {
		expression();
		sxb_strcat("[%s]", strpop());
		dim--;
		if (dim < 0) PutError("Too many elements");
	} while (amatch(","));
	check(")");
	if (dim > 0) PutError("Too few elements");
}

/*
** プロパティの値を参照する
*/
void
RefProp(void)
{
	char	itemname[100];
	char	propname[100];
	bool	isarray = FALSE;

	GetString(itemname);	/* Get Item Name */
	if (amatch("[")) {
		isarray = TRUE;
		expression();
		ToInt1(lastClass);
		check("]");
	}
	check(".");
	if (amatch("caption")) {
		if (isarray)	strpush("_sxb_ref_propS(\"%s\",1, %s, \"caption\")", itemname, strpop());
		else		strpush("_sxb_ref_propS(\"%s\",0, 0,  \"caption\")", itemname);
		lastClass = SC_STR;
	}
	else if (amatch("page")) {
		if (isarray)	strpush("_sxb_ref_propS(\"%s\",1, %s, \"page\")", itemname, strpop());
		else		strpush("_sxb_ref_propS(\"%s\",0, 0,  \"page\")", itemname);
		lastClass = SC_STR;
	}
	else {
		GetString(propname);
		if (isarray)	strpush("_sxb_ref_prop(\"%s\",1, %s, \"%s\")", itemname, strpop(), propname);
		else		strpush("_sxb_ref_prop(\"%s\",0,  0, \"%s\")", itemname, propname);
		lastClass = SC_INT;
	}
}

/*
** ＊，／，￥，modの処理ルーチン
*/
void
expression1(void)
{
	SCLASS	classp;
	char* p;

	factor();
	classp = lastClass;

	if (amatch("/*")) {
		TokenPtr -= 2;
		return;		/* コメントは／と間違えないように、はじいておく */
	}
	while (1) {
		if (amatch("*")) {
			factor();
			lastClass = ClassConvert(classp, lastClass);
			p = strpop();
			strpush("%s * %s", strpop(), p);
		}
		else if (amatch("/")) {
			factor();
			lastClass = ClassConvert(classp, lastClass);
			p = strpop();
			strpush("%s / %s", strpop(), p);
		}
		else break;
	}
}

/*
 * 変数の型を合せる（数値型）
 */
SCLASS
ClassConvert(SCLASS class1, SCLASS class2)
{
	if (class1 == SC_CHAR || class1 == SC_INT) {
		if (class2 == SC_CHAR || class2 == SC_INT) {
			return(SC_INT);
		}
		else if (class2 == SC_FLOAT) {
			return(SC_FLOAT);
		}
	}
	else if (class1 == SC_FLOAT) {
		if (class2 == SC_CHAR || class2 == SC_INT) {
			return(SC_FLOAT);
		}
		else if (class2 == SC_FLOAT) {
			return(SC_FLOAT);
		}
	}
	PutError("Invalid variable type");
}

/*
 * 変数の型を合せる（文字列型）
 */
SCLASS
ClassConvertS(SCLASS class1, SCLASS class2)
{
	if (class1 == SC_STR) {
		if (class2 == SC_STR) {
			return(SC_STR);
		}
	}
	PutError("Invalid variable type");
}

/*
 * 浮動小数点型であれば整数型にする
 */
void
ToInt1(SCLASS class)
{
	if (class == SC_STR)
		PutError("変数の型が違います");
}

void
ToInt2(SCLASS class1, SCLASS class2)
{
	if (class1 == SC_STR || class2 == SC_STR)
		PutError("変数の型が違います");
}

/*
** 整数型であれば浮動小数点型にする
*/
void
ToFloat1(SCLASS class)
{
	if (class == SC_STR)
		PutError("変数の型が違います");
}
void
ToFloat2(SCLASS class1, SCLASS class2)
{
	if (class1 == SC_STR || class2 == SC_STR)
		PutError("変数の型が違います");
}

void
ToStr1(SCLASS class)
{
	if (class != SC_STR)
		PutError("変数の型が違います");
}

/*
** 式のキャスティングを行なう
*/
void
doCast(SCLASS class)
{
	if (class == SC_CHAR || class == SC_INT) ToInt1(lastClass);
	else if (class == SC_FLOAT)		ToFloat1(lastClass);
	else if (class == SC_STR)		ToStr1(lastClass);
	else PutError("変数の型が違います");

	lastClass = class;
}

/*
 * ￥の処理
 */
void
expression2(void)
{
	SCLASS	classp;
	char* p;

	expression1();
	classp = lastClass;
	while (amatch("\\")) {
		expression1();
		ToInt2(classp, lastClass);
		lastClass = SC_INT;
		p = strpop();
		strpush("%s %% %s", strpop(), p);
	}
}

/*
** modの処理
 */
void
expression3(void)
{
	SCLASS	classp;
	char* p;

	expression2();
	classp = lastClass;
	while (amatch("mod ")) {
		expression2();
		lastClass = ClassConvert(classp, lastClass);
		p = strpop();
		strpush("%s %% %s", strpop(), p);
	}
}


/*
** ＋，－の処理
*/
void
expression4(void)
{
	SCLASS	classp;
	char* p;
	bool	isStrAddUsed = FALSE;

	expression3();
	classp = lastClass;

	while (1) {
		if (amatch("+")) {
			if (lastClass == SC_STR) {
				if (isStrAddUsed == FALSE) {
					strpush("_sxb_add(%s", strpop());
					isStrAddUsed = TRUE;
				}
				expression3();
				ToStr1(lastClass);
				p = strpop();
				strpush("%s, %s", strpop(), p);
			}
			else {
				expression3();
				p = strpop();
				strpush("%s + %s", strpop(), p);
			}
		}
		else if (amatch("-")) {
			expression3();
			lastClass = ClassConvert(classp, lastClass);
			p = strpop();
			strpush("%s - %s", strpop(), p);
		}
		else break;
	}
	if (lastClass == SC_STR && isStrAddUsed)
		strpush("%s,-1)", strpop());
}

/*
 * shr, shlの処理
 */
void
expression5(void)
{
	SCLASS	classp;
	char* p;

	expression4();
	classp = lastClass;

	while (1) {
		if (amatch("shr ")) {
			expression4();
			ToInt2(classp, lastClass);
			p = strpop();
			strpush("%s >> %s", strpop(), p);
			lastClass = SC_INT;
		}
		else if (amatch("shl ")) {
			expression4();
			ToInt2(classp, lastClass);
			p = strpop();
			strpush("%s << %s", strpop(), p);
			lastClass = SC_INT;
		}
		else break;
	}
}

/*
** 関係演算の処理
*/
void
expression6(void)
{
	SCLASS	classp;
	char* p;

	expression5();
	classp = lastClass;

	if (amatch("<=")) {
		expression5();
		p = strpop();
		if (classp == SC_STR && lastClass == SC_STR) {
			strpush("sxb_strcmpLE(%s, %s)", strpop(), p);
		}
		else {
			lastClass = ClassConvert(classp, lastClass);
			strpush("%s <= %s", strpop(), p);
		}
	}
	else if (amatch(">=")) {
		expression5();
		p = strpop();
		if (classp == SC_STR && lastClass == SC_STR) {
			strpush("_sxb_strcmpGE(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s >= %s", strpop(), p);
			lastClass = ClassConvert(classp, lastClass);
		}
	}
	else 	if (amatch("!=") || amatch("<>")) {
		expression5();
		p = strpop();
		if (classp == SC_STR && lastClass == SC_STR) {
			strpush("_sxb_strcmpNE(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s != %s", strpop(), p);
			lastClass = ClassConvert(classp, lastClass);
		}
	}
	else if (amatch("<")) {
		expression5();
		p = strpop();
		if (classp == SC_STR && lastClass == SC_STR) {
			strpush("_sxb_strcmpLT(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s < %s", strpop(), p);
			lastClass = ClassConvert(classp, lastClass);
		}
	}
	else 	if (amatch(">")) {
		expression5();
		p = strpop();
		if (classp == SC_STR && lastClass == SC_STR) {
			strpush("_sxb_strcmpGT(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s > %s", strpop(), p);
			lastClass = ClassConvert(classp, lastClass);
		}
	}
	else 	if (amatch("==") | amatch("=")) {
		expression5();
		p = strpop();
		if (classp == SC_STR && lastClass == SC_STR) {
			strpush("_sxb_strcmpEQ(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s == %s", strpop(), p);
		}
	}
	else	return;
	lastClass = SC_INT;
}

/*
 * ＮＯＴの処理
 */
void
expression7(void)
{
	if (amatch("not")) {
		if (isalnum(*TokenPtr)) {	/* notaは変数名 */
			TokenPtr -= 3;
		}
		else {
			expression7();
			ToInt1(lastClass);
			strpush("!(%s)", strpop());
			lastClass = SC_INT;
			return;
		}
	}
	expression6();
}

/*
 * ＡＮＤの処理
 */
void
expression8(void)
{
	SCLASS	classp;
	char* p;

	expression7();
	classp = lastClass;

	while (amatch("and")) {
		if (isalnum(*TokenPtr)) {	/* andaは変数名 */
			TokenPtr -= 3;
			break;
		}
		expression7();
		ToInt2(classp, lastClass);
		p = strpop();
		strpush("(%s) & (%s)", strpop(), p);
		lastClass = SC_INT;
	}
}

/*
 * ＯＲの処理
 */
void
expression9(void)
{
	SCLASS	classp;
	char* p;

	expression8();
	classp = lastClass;

	while (amatch("or")) {
		if (isalnum(*TokenPtr)) {	/* oraは変数名 */
			TokenPtr -= 2;
			break;
		}
		expression8();
		ToInt2(classp, lastClass);
		p = strpop();
		strpush("(%s) | (%s)", strpop(), p);
		lastClass = SC_INT;
	}
}

/*
 * 式の処理
 */
void
expression(void)
{
	SCLASS	classp;
	char* p;

	expression9();
	classp = lastClass;

	while (amatch("xor")) {
		if (isalnum(*TokenPtr)) {	/* xoraは変数名 */
			TokenPtr -= 3;
			break;
		}
		expression9();
		ToInt2(classp, lastClass);
		p = strpop();
		strpush("%s ^ %s", strpop(), p);
		lastClass = SC_INT;
	}
}


/*
 *　文字列の処理
 */
SCLASS
doString(void)
{
	char	tmpBuff[1000];
	char* p = tmpBuff;

	*p++ = '\"';
	while (*TokenPtr != '\"') {
		if (*(unsigned char*)TokenPtr < ' ' && *TokenPtr != '\t') {
			SynErr();
		}
		*p++ = *TokenPtr++;
	}
	*p++ = *TokenPtr++;		/* "をスキップ */
	*p++ = '\0';
	strpush("%s", tmpBuff);	/* ""とか%dとかを通すため */
	return(SC_STR);
}

void
SkipSpace(void)
{
	while (*TokenPtr == ' ' || *TokenPtr == '\t') {
		TokenPtr++;
	}
}

/*
** Advance the token pointer upon maching the keyword
*/
int
amatch(char* s)
{
	char	c, * p;

	SkipSpace();
	p = TokenPtr;
	while (*s) {
		c = *p++;
		if (c == '\t') c = ' ';
		if (*s++ != c) return(FALSE);
	}
	TokenPtr = p;
	return(TRUE);
}

int
famatch(char* s)
{
	if (amatch(s)) {
		PutCode(s);
		return(TRUE);
	}
	return(FALSE);
}

int
pamatch(char* s)
{
	if (amatch(s)) {
		strpush(s);
		return(TRUE);
	}
	return(FALSE);
}

/*
** 定数を取り込む
*/
int
aconst(void)
{
	SkipSpace();

	if (isdigit(*TokenPtr)) {
		GetDecNum();
		return(TRUE);
	}
	else if (amatch("&h") || amatch("&H")) {
		GetHexNum();
		return(TRUE);
	}
	else if (amatch("&b") || amatch("&B")) {
		GetBinNum();
		return(TRUE);
	}
	else if (amatch("&o") || amatch("&O")) {
		GetOctNum();
		return(TRUE);
	}
	else if (amatch("\'")) {
		GetChrNum();
		return(TRUE);
	}
	return(FALSE);
}

void
GetOctNum(void)
{
	vali = 0;

	while ('0' <= *TokenPtr && *TokenPtr <= '7')
		vali = vali * 8 + (*TokenPtr++ - '0');
	lastClass = SC_INT;
}

void
GetBinNum(void)
{
	vali = 0;
	while (1) {
		if (amatch("0")) {
			vali = vali * 2;
		}
		else if (amatch("1")) {
			vali = vali * 2 + 1;
		}
		else
			break;
	}
	lastClass = SC_INT;
}

void
GetHexNum(void)
{
	vali = 0;
	while (isxdigit(*TokenPtr)) {
		if ('0' <= *TokenPtr && *TokenPtr <= '9')
			vali = vali * 16 + (*TokenPtr++ - '0');
		else if ('a' <= *TokenPtr && *TokenPtr <= 'f')
			vali = vali * 16 + (*TokenPtr++ - 'a' + 10);
		else if ('A' <= *TokenPtr && *TokenPtr <= 'F')
			vali = vali * 16 + (*TokenPtr++ - 'A' + 10);
	}
	lastClass = SC_INT;
}

void
GetChrNum(void)
{
	vali = *TokenPtr++;
	check("\'");
	lastClass = SC_INT;
}

void
GetDecNum(void)
{
	char* ptr = TokenPtr;

	vali = 0;
	while (isdigit(*TokenPtr)) {
		vali = vali * 10 + (*TokenPtr++ - '0');
	}
	if (amatch(".")) {	/* 浮動小数点型 */
		lastClass = SC_FLOAT;
		valf = atof(ptr);
		while (isdigit(*TokenPtr)) TokenPtr++;
		if (amatch("e+") || amatch("E+") || amatch("e-") || amatch("E-"))
			while (isdigit(*TokenPtr)) TokenPtr++;
		vali = 0;
	}
	else if (amatch("e") || amatch("E")) {	/* 浮動小数点型 */
		lastClass = SC_FLOAT;
		valf = atof(ptr);
		if (!amatch("-")) amatch("+");
		while (isdigit(*TokenPtr)) TokenPtr++;
		vali = 0;
	}
	else if (amatch("#")) {	/* 浮動小数点型 */
		lastClass = SC_FLOAT;
		valf = (double)vali;
		vali = 0;
	}
	else {			/* 整数型 */
		lastClass = SC_INT;
		valf = 0.0;
	}
}

char
* strpop(void)
{
	char* q, * p;

	if (strStackPtr <= strBuffer) {
		fprintf(stderr, "Internal Error : string stack empty\n");
		fprintf(stderr, "\n%s\n", prgBuff);
	}
	p = strBuffer;
	do {
		q = p;
		while (*p++);
	} while (p < strStackPtr);

	strStackPtr = q;
	return(q);
}

void
strpush(char* fmt, ...)
{
	va_list	marker;
	char	buff[1000];

	va_start(marker, fmt);
	vsprintf(buff, fmt, marker);
	va_end(marker);

	if (&strBuffer[STR_BUFFER_SIZE] <= &strStackPtr[strlen(buff)])
		PutError("Internal error : strStack overflow.\n");

	strcpy(strStackPtr, buff);
	while (*strStackPtr++);
}

void
sxb_strcat(char* fmt, ...)
{
	char* p;
	va_list	marker;
	char	buff[1000];

	va_start(marker, fmt);
	vsprintf(buff, fmt, marker);
	va_end(marker);

	p = strpop();

	if (&strBuffer[STR_BUFFER_SIZE] <= &strStackPtr[strlen(buff)])
		PutError("Internal error : strStack overflow.\n");

	strcat(p, buff);
	while (*strStackPtr++);
}

void
PutCode(char* fmt, ...)
{
	int		i;
	/* At first, indent must be 0, Don't have to initialize isFirst */
	static	bool	isFirstCh;
	va_list	marker;
	char	buff[1000];

	va_start(marker, fmt);

	for (i = 0; i < indent && isFirstCh; i++)
		fprintf(outputFp, "\t");

	vsprintf(buff, fmt, marker);
	fprintf(outputFp, "%s", buff);

	if (strstr(buff, "\n") != NULL)
		isFirstCh = TRUE;
	else
		isFirstCh = FALSE;

	va_end(marker);
}
