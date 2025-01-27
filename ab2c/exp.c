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
extern	E_TYPE	lastType;
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
** •¶–@ƒGƒ‰[
*/
volatile void
SynErr(void)
{
	PutError("Syntax Error");
}



/*
**@Šú‘Ò•¶š—ñ‚ğŠm”F
*/
void
check(char* s)
{
	char	buf[50];

	if (!amatch(s)) {
		sprintf(buf, "keyword %s expected", s);
		PutError(buf);
	}
}

void
fcheck(char* s)
{
	char	buf[50];

	if (!amatch(s)) {
		sprintf(buf, "Misisng Keyword : %s", s);
		PutError(buf);
	}
	PutCode(s);
}

/*
** <factor> ‚Ìˆ—ƒ‹[ƒ`ƒ“
*/
void
factor(void)
{
	SYMTBL* p;
	char	c;

	if (aconst()) {
		if (lastType == ET_CHAR || lastType == ET_INT) {
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
		if (lastType == ET_STR) {
			PutError("•Ï”‚ÌŒ^‚ªˆá‚¢‚Ü‚·");
		}
	}
	else if (amatch("\"")) {
		lastType = doString();
	}
	else if (efuncs()) {	/* ‘g‚İ‚İŠÖ” */
	}
	else if (c = TokenPtr[TokenLen(TokenPtr)], c == '(') {
		FNCTBL* q;
		q = doFunctionCall();
		lastType = q->retType;
	}
	else SynErr();
}

/*
 * •Ï”‚Ì’l‚ğQÆ‚·‚é
 */
void
RefVariable(int isGlobal, SYMTBL* p)
{
	int	dim = p->dim;

	TokenPtr += TokenLen(TokenPtr);

	lastType = p->type;
	if (lastType == ET_STR)	dim--;

	if (dim == 0) {	/* simple variable (not an array) */
		strpush("%s", p->name);
		if (p->type == ET_STR && amatch("[")) {
			/* str[0]‚Ì‚æ‚¤‚ÈŒ`@¨@CHARŒ^‚Æ‚µ‚Äˆµ‚¤ */
			expression();
			check("]");
			sxb_strcat("[%s]", strpop());
			lastType = ET_CHAR;
		}
	}
	else if (amatch("(")) {
		/* array variable */
		DoIndexed(p);
		lastType = p->type;
		if (p->type == ET_STR && amatch("[")) {
			expression();
			check("]");
			sxb_strcat("[%s]", strpop());
			lastType = ET_CHAR;
		}
	}
	else
		PutError("%s‚Í”z—ñ•Ï”‚Å‚·", p->name);
}

/*
** (a1,a2,a3)‚Ìˆ—
*/
void
DoIndexed(SYMTBL* p)
{
	int	dim = p->dim;

	if (p->type == ET_STR) dim = p->dim - 1;

	/*	check("(");	‚Í–‘O‚És‚Á‚Ä‚¢‚é */

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
** –C^CCmod‚Ìˆ—ƒ‹[ƒ`ƒ“
*/
void
expression1(void)
{
	E_TYPE	typep;
	char* p;

	factor();
	typep = lastType;

	if (amatch("/*")) {
		TokenPtr -= 2;
		return;		/* ƒRƒƒ“ƒg‚Í^‚ÆŠÔˆá‚¦‚È‚¢‚æ‚¤‚ÉA‚Í‚¶‚¢‚Ä‚¨‚­ */
	}
	while (1) {
		if (amatch("*")) {
			factor();
			lastType = PickAlignedType(typep, lastType);
			p = strpop();
			strpush("%s * %s", strpop(), p);
		}
		else if (amatch("/")) {
			factor();
			lastType = PickAlignedType(typep, lastType);
			p = strpop();
			strpush("%s / %s", strpop(), p);
		}
		else break;
	}
}

/*
 * •Ï”‚ÌŒ^‚ğ‡‚¹‚éi”’lŒ^j
 */
E_TYPE
PickAlignedType(E_TYPE type1, E_TYPE type2)
{
	if (type1 == ET_CHAR || type1 == ET_INT) {
		if (type2 == ET_CHAR || type2 == ET_INT) {
			return(ET_INT);
		}
		else if (type2 == ET_FLOAT) {
			return(ET_FLOAT);
		}
	}
	else if (type1 == ET_FLOAT) {
		if (type2 == ET_CHAR || type2 == ET_INT) {
			return(ET_FLOAT);
		}
		else if (type2 == ET_FLOAT) {
			return(ET_FLOAT);
		}
	}
	PutError("Invalid variable type");
}

/*
 * •Ï”‚ÌŒ^‚ğ‡‚¹‚éi•¶š—ñŒ^j
 */
E_TYPE
PickAlignedTypeS(E_TYPE type1, E_TYPE type2)
{
	if (type1 == ET_STR) {
		if (type2 == ET_STR) {
			return(ET_STR);
		}
	}
	PutError("Invalid variable type");
}

/*
 * •‚“®¬”“_Œ^‚Å‚ ‚ê‚Î®”Œ^‚É‚·‚é
 */
void
ToInt1(E_TYPE type)
{
	if (type == ET_STR)
		PutError("•Ï”‚ÌŒ^‚ªˆá‚¢‚Ü‚·");
}

void
ToInt2(E_TYPE type1, E_TYPE type2)
{
	if (type1 == ET_STR || type2 == ET_STR)
		PutError("•Ï”‚ÌŒ^‚ªˆá‚¢‚Ü‚·");
}

/*
** ®”Œ^‚Å‚ ‚ê‚Î•‚“®¬”“_Œ^‚É‚·‚é
*/
void
ToFloat1(E_TYPE type)
{
	if (type == ET_STR)
		PutError("•Ï”‚ÌŒ^‚ªˆá‚¢‚Ü‚·");
}

void
ToFloat2(E_TYPE type1, E_TYPE type2)
{
	if (type1 == ET_STR || type2 == ET_STR)
		PutError("•Ï”‚ÌŒ^‚ªˆá‚¢‚Ü‚·");
}

void
ToStr1(E_TYPE type)
{
	if (type != ET_STR)
		PutError("•Ï”‚ÌŒ^‚ªˆá‚¢‚Ü‚·");
}

/*
** ®‚ÌƒLƒƒƒXƒeƒBƒ“ƒO‚ğs‚È‚¤
*/
void
doCast(E_TYPE type)
{
	if (type == ET_CHAR || type == ET_INT) ToInt1(lastType);
	else if (type == ET_FLOAT)		ToFloat1(lastType);
	else if (type == ET_STR)		ToStr1(lastType);
	else PutError("•Ï”‚ÌŒ^‚ªˆá‚¢‚Ü‚·");

	lastType = type;
}

/*
 * ‚Ìˆ—
 */
void
expression2(void)
{
	E_TYPE	typep;
	char* p;

	expression1();
	typep = lastType;
	while (amatch("\\")) {
		expression1();
		ToInt2(typep, lastType);
		lastType = ET_INT;
		p = strpop();
		strpush("%s %% %s", strpop(), p);
	}
}

/*
** mod‚Ìˆ—
 */
void
expression3(void)
{
	E_TYPE	typep;
	char* p;

	expression2();
	typep = lastType;
	while (amatch("mod ")) {
		expression2();
		lastType = PickAlignedType(typep, lastType);
		p = strpop();
		strpush("%s %% %s", strpop(), p);
	}
}


/*
** {C|‚Ìˆ—
*/
void
expression4(void)
{
	E_TYPE	typep;
	char* p;
	bool	isStrAddUsed = FALSE;

	expression3();
	typep = lastType;

	while (1) {
		if (amatch("+")) {
			if (lastType == ET_STR) {
				if (isStrAddUsed == FALSE) {
					strpush("_sxb_add(%s", strpop());
					isStrAddUsed = TRUE;
				}
				expression3();
				ToStr1(lastType);
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
			lastType = PickAlignedType(typep, lastType);
			p = strpop();
			strpush("%s - %s", strpop(), p);
		}
		else break;
	}
	if (lastType == ET_STR && isStrAddUsed)
		strpush("%s,-1)", strpop());
}

/*
 * shr, shl‚Ìˆ—
 */
void
expression5(void)
{
	E_TYPE	typep;
	char* p;

	expression4();
	typep = lastType;

	while (1) {
		if (amatch("shr ")) {
			expression4();
			ToInt2(typep, lastType);
			p = strpop();
			strpush("%s >> %s", strpop(), p);
			lastType = ET_INT;
		}
		else if (amatch("shl ")) {
			expression4();
			ToInt2(typep, lastType);
			p = strpop();
			strpush("%s << %s", strpop(), p);
			lastType = ET_INT;
		}
		else break;
	}
}

/*
** ŠÖŒW‰‰Z‚Ìˆ—
*/
void
expression6(void)
{
	E_TYPE	typep;
	char* p;

	expression5();
	typep = lastType;

	if (amatch("<=")) {
		expression5();
		p = strpop();
		if (typep == ET_STR && lastType == ET_STR) {
			strpush("sxb_strcmpLE(%s, %s)", strpop(), p);
		}
		else {
			lastType = PickAlignedType(typep, lastType);
			strpush("%s <= %s", strpop(), p);
		}
	}
	else if (amatch(">=")) {
		expression5();
		p = strpop();
		if (typep == ET_STR && lastType == ET_STR) {
			strpush("_sxb_strcmpGE(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s >= %s", strpop(), p);
			lastType = PickAlignedType(typep, lastType);
		}
	}
	else if (amatch("!=") || amatch("<>")) {
		expression5();
		p = strpop();
		if (typep == ET_STR && lastType == ET_STR) {
			strpush("_sxb_strcmpNE(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s != %s", strpop(), p);
			lastType = PickAlignedType(typep, lastType);
		}
	}
	else if (amatch("<")) {
		expression5();
		p = strpop();
		if (typep == ET_STR && lastType == ET_STR) {
			strpush("_sxb_strcmpLT(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s < %s", strpop(), p);
			lastType = PickAlignedType(typep, lastType);
		}
	}
	else if (amatch(">")) {
		expression5();
		p = strpop();
		if (typep == ET_STR && lastType == ET_STR) {
			strpush("_sxb_strcmpGT(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s > %s", strpop(), p);
			lastType = PickAlignedType(typep, lastType);
		}
	}
	else if (amatch("==") | amatch("=")) {
		expression5();
		p = strpop();
		if (typep == ET_STR && lastType == ET_STR) {
			strpush("_sxb_strcmpEQ(%s,%s)", strpop(), p);
		}
		else {
			strpush("%s == %s", strpop(), p);
		}
	}
	else	return;
	lastType = ET_INT;
}

/*
 * ‚m‚n‚s‚Ìˆ—
 */
void
expression7(void)
{
	if (amatch("not")) {
		if (isalnum(*TokenPtr)) {	/* nota‚Í•Ï”–¼ */
			TokenPtr -= 3;
		}
		else {
			expression7();
			ToInt1(lastType);
			strpush("!(%s)", strpop());
			lastType = ET_INT;
			return;
		}
	}
	expression6();
}

/*
 * ‚`‚m‚c‚Ìˆ—
 */
void
expression8(void)
{
	E_TYPE	typep;
	char* p;

	expression7();
	typep = lastType;

	while (amatch("and")) {
		if (isalnum(*TokenPtr)) {	/* anda‚Í•Ï”–¼ */
			TokenPtr -= 3;
			break;
		}
		expression7();
		ToInt2(typep, lastType);
		p = strpop();
		strpush("(%s) & (%s)", strpop(), p);
		lastType = ET_INT;
	}
}

/*
 * ‚n‚q‚Ìˆ—
 */
void
expression9(void)
{
	E_TYPE	typep;
	char* p;

	expression8();
	typep = lastType;

	while (amatch("or")) {
		if (isalnum(*TokenPtr)) {	/* ora‚Í•Ï”–¼ */
			TokenPtr -= 2;
			break;
		}
		expression8();
		ToInt2(typep, lastType);
		p = strpop();
		strpush("(%s) | (%s)", strpop(), p);
		lastType = ET_INT;
	}
}

/*
 * ®‚Ìˆ—
 */
void
expression(void)
{
	E_TYPE	typep;
	char* p;

	expression9();
	typep = lastType;

	while (amatch("xor")) {
		if (isalnum(*TokenPtr)) {	/* xora‚Í•Ï”–¼ */
			TokenPtr -= 3;
			break;
		}
		expression9();
		ToInt2(typep, lastType);
		p = strpop();
		strpush("%s ^ %s", strpop(), p);
		lastType = ET_INT;
	}
}


/*
 *@•¶š—ñ‚Ìˆ—
 */
E_TYPE
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
	*p++ = *TokenPtr++;		/* "‚ğƒXƒLƒbƒv */
	*p++ = '\0';
	strpush("%s", tmpBuff);	/* ""‚Æ‚©%d‚Æ‚©‚ğ’Ê‚·‚½‚ß */
	return(ET_STR);
}

void
SkipSpace(void)
{
	while (*TokenPtr == ' ' || *TokenPtr == '\t') {
		TokenPtr++;
	}
}

/*
** Advance the token pointer upon matching the keyword
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
** ’è”‚ğæ‚è‚Ş
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
	lastType = ET_INT;
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
	lastType = ET_INT;
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
	lastType = ET_INT;
}

void
GetChrNum(void)
{
	vali = *TokenPtr++;
	check("\'");
	lastType = ET_INT;
}

void
GetDecNum(void)
{
	char* ptr = TokenPtr;

	vali = 0;
	while (isdigit(*TokenPtr)) {
		vali = vali * 10 + (*TokenPtr++ - '0');
	}
	if (amatch(".")) {	/* •‚“®¬”“_Œ^ */
		lastType = ET_FLOAT;
		valf = atof(ptr);
		while (isdigit(*TokenPtr)) TokenPtr++;
		if (amatch("e+") || amatch("E+") || amatch("e-") || amatch("E-"))
			while (isdigit(*TokenPtr)) TokenPtr++;
		vali = 0;
	}
	else if (amatch("e") || amatch("E")) {	/* •‚“®¬”“_Œ^ */
		lastType = ET_FLOAT;
		valf = atof(ptr);
		if (!amatch("-")) amatch("+");
		while (isdigit(*TokenPtr)) TokenPtr++;
		vali = 0;
	}
	else if (amatch("#")) {	/* •‚“®¬”“_Œ^ */
		lastType = ET_FLOAT;
		valf = (double)vali;
		vali = 0;
	}
	else {			/* ®”Œ^ */
		lastType = ET_INT;
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
