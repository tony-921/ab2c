/*
	X-BASIC to C converter
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<setjmp.h>
#include	"sxbasic.h"


FILE	*inputFp, *outputFp;
SCLASS	lastClass;

bool	toSkipLineNumber = TRUE;

// FIXME
// #define TEST_FILE	"C:\\Users\\tatsu\\source\\repos\\ab2c\\tests\\music_sample\\music_sample"
#define TEST_FILE	"C:\\Users\\tatsu\\source\\repos\\ab2c\\tests\\func_test"

void
main(int argc, char *argv[])
{
	char	*p;
	char	fname[255];

	argc = 2;
	argv[0] = "ab2c.s";
	argv[1] = TEST_FILE ".bas";
	argv[2] = TEST_FILE ".c";

	if((argc == 2 && !strcmp(argv[1], "-h"))
	|| (argc == 2 && !strcmp(argv[1], "-H"))
	|| (argc != 2 && argc != 3))  {
		fprintf(stderr, "Usage :ab2c filename.\n");
		exit(-1);
	}
	strcpy(fname, argv[1]);
	if(strstr(fname, ".") == NULL)
		strcat(fname, ".bas");

	inputFp = fopen(fname, "r");
	if(inputFp == NULL) {
		fprintf(stderr, "Can not open %s\n", argv[1]);
		exit(-1);
	}
	if(argc == 2) {
		strcpy(fname, argv[1]);
		p = strstr(fname, ".");
		if(p)	strcpy(p, ".c");
		else	strcat(fname, ".c");
	} else {
		/* here,  argc == 2 */
		strcpy(fname, argv[2]);
	}
	outputFp = fopen(fname, "w");
	if(outputFp == NULL) {
		fprintf(stderr, "Can not open %s\n", fname);
		exit(-1);
	}

	Compile();

	fclose(inputFp);
	fclose(outputFp);
}

