#include	<stdio.h>
#include	<string.h>
#include	"ab2c_run.h"
#include	"ab2c_var.h"
int x,y,k;
int fn,fp,e;
int R,G,B,I,g0;
char buf[1537];

void _sxb_start(void) {
	printf("%s\n", timeS());
	fn = _sxb_fopen("test01.bmp","r");
	fp = _sxb_fseek(fn,54,0);
	for(y = 0; y <= 512 ;y++) {
		e = _sxb_fread(buf, sizeof(buf), 512 * 3,fn);
		for(x = 0; x <= 511 ;x++) {
			k = x * 3;
			B = (buf[k] >> 3) << 1;
			g0 = buf[k + 1];
			G = (g0 >> 3) << 11;
			R = (buf[k + 2] >> 3) << 6;
			I = ((g0) & (4)) == 4;
		}
	}
	_sxb_fcloseall();
	printf("%s\n", timeS());
	printf("?");
	scanf("%i", &e);
	
	;
};