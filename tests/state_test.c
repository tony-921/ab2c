#include	<stdio.h>
#include	<string.h>
#include	"ab2c_run.h"
#include	"ab2c_var.h"
int a=1;
int b=2;
char ch;
int fp=3;
int na[21];
char	s[32];
char	prompt[32];

void _sxb_start(void) {
	exit(-1);
	_sxb_fclose(fp);
	_sxb_fcloseall();
	_sxb_fdelete(fp);
	_sxb_fputc(97,fp);
	_sxb_frename("oldfile.txt","newfile.txt");
	_sxb_fseek(fp,1,2);
	_sxb_fwrite(na, sizeof(na), 1,fp);
	_sxb_randomize(123);
	_sxb_srand(123);
	printf("%d\n", a);
	printf("%s\n", "Hello");
	printf("?");
	scanf("%i", &a);
	
	printf("input int value");
	scanf("%i", &a);
	
	printf(_sxb_add(s, "string",-1));
	scanf("%s", &s);
	
};