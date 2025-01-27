#include	<stdio.h>
#include	<string.h>
#include	"ab2c_run.h"
int i;
int na[21];
char	s[32]= "Hello world. how are you ?";
int fp,fp2;

void _sxb_start(void) {
	remove("test.bin");
	fp = _sxb_fopen("test.bin","c");
	for(i = 0; i <= 10 ;i++) {
		_sxb_fputc(asc("a"),fp);
	}
	for(i = 0; i <= 20 ;i++) {
		na[i] = i;
	}
	_sxb_fwrite(na, sizeof(na), 20,fp);
	_sxb_fwrites(s,fp);
	_sxb_fclose(fp);
	fp2 = _sxb_fopen("test.bin","r");
	for(i = 0; i <= 10 ;i++) {
		printf("%s", _sxb_chrS(_sxb_fgetc(fp2)));printf("%s", " ");
	}
	putchar('\n')
	
	_sxb_fread(na, sizeof(na), 20,fp2);
	_sxb_fclose(fp2);
	for(i = 0; i <= 20 ;i++) {
		printf("%s", _sxb_hex(na[i]));printf("%s", " ");
	}
	;
};