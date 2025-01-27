#include	<stdio.h>
#include	<string.h>
#include	"ab2c_run.h"
int na[11],int nb[11];
int fp1,fp2;
int i;

void _sxb_start(void) {
	for(i = 0; i <= 10 ;i++) {
		na[i] = i;
	}
	fp1 = _sxb_fopen("filetest.bin","c");
	_sxb_fwrite(na, sizeof(na), 10,fp1);
	_sxb_fclose(fp1);
	fp2 = _sxb_fopen("filetest.bin","r");
	_sxb_fread(nb, sizeof(nb), 10,fp2);
	_sxb_fclose(fp2);
	for(i = 0; i <= 10 ;i++) {
		printf("%d\n", nb[i]);
	}
};