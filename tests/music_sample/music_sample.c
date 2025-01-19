#include	<stdio.h>
#include	<string.h>
#include	"ab2c_run.h"
#include	"ab2c_var.h"
int	M_CHANGE(void );
int	MML_IN(void );
int ai;
char	as[32],bs[32],cs[32],s1[50];

void _sxb_start(void) {
	strncpy(s1," âπêFïœçX[H] ââët[P] í‚é~[S] çƒäJ[C] èIóπ=[E] :",sizeof(s1));
	m_init();
	m_sysch("OPM");
	_sxb_cls(); // FIXME
	M_CHANGE();MML_IN();m_play();
	do {
		locate(0,1)
		printf("%s", _sxb_chrS(5));printf("%s", s1);printf("");
		printf("?");
		scanf("%s", &cs);
		
		if((_sxb_strcmpEQ(cs,"H")) | (_sxb_strcmpEQ(cs,"h"))) {
				M_CHANGE();MML_IN();m_play();}
		if((_sxb_strcmpEQ(cs,"P")) | (_sxb_strcmpEQ(cs,"p"))) {
				m_play();}
		if((_sxb_strcmpEQ(cs,"S")) | (_sxb_strcmpEQ(cs,"s"))) {
				m_stop();}
		if((_sxb_strcmpEQ(cs,"C")) | (_sxb_strcmpEQ(cs,"c"))) {
				m_cont();}
		if((_sxb_strcmpEQ(cs,"E")) | (_sxb_strcmpEQ(cs,"e"))) {
				m_stop();}
	} while (!((_sxb_strcmpEQ(cs,"e")) | (_sxb_strcmpEQ(cs,"E"))));
	_sxb_end();
	_sxb_end();
}
int M_CHANGE(void){
	for(ai = 1; ai <= 5 ;ai++) {
		m_alloc(ai,2000);
		m_assign(ai,ai);
	}
	locate(0,0)
	printf("%s", _sxb_chrS(5));
	scanf("%s", &as);
	
	printf("%s", _sxb_chrS(5));
	scanf("%s", &bs);
	
}

int MML_IN(void){
	m_trk(1,_sxb_add("q7 116 04 v14 t", bs, "", as,-1));
	m_trk(2,_sxb_add("97 116 4 v10 t", bs, "", as,-1));
	m_trk(3,_sxb_add("97 116 04 v10 t", bs, "6", as,-1));
	m_trk(4,_sxb_add("q7 116 03 v15 t", bs, "645",-1));
	m_trk(5,_sxb_add("q7 116 od v15 t", bs, "@47",-1));
	m_trk(1,"<c&d e8g8g8a8 g8e8c8.d e8e8d8c8 d4.");
	m_trk(2,"e&f g8<c8c8d8 c8>g8g8.f g8g8f8e8 f4.");
	m_trk(3,"Ñs8 r8 >c8g8e8g8 c8g8e8g8 c8g8e8g8 >b8<g8d8");
	m_trk(1,"c&d e8g8g8.a g8e8c8.d e8e8d8d8 c4.");
	m_trk(2,"e&f g8<c8c8.d c8g8g8.f g8g8f8f8 e4.");
	m_trk(3,"g8 c8g8e8g8 c8g8e8g8 c8g8d8g8 c8g8c8");
	m_trk(1,"c&d e8g8g8.a g8e8c8.d e8e8d8c8d4.");
	m_trk(2,"e&f g8<c8c8.d c8>g8g8.f g8g8f8e8 f4.");
	m_trk(3,"r8 c8g8e8g8 c8g8e8g8c8g8e8g8 >b8<g8d8");
	m_trk(1,"c&d e8g8g8a8 g8.ec8.d e8e8d8.d c4r4");
	m_trk(2,"e&f g8<c8c8d8 c8.>gg8.f g8g8f8.f e4r4");
	m_trk(3,"g8 c8g8e8g8 c8g8e8g8 c8g8d8g8 c8c8d8e8");
	m_trk(1,"14f4 a8a4a8 g8.ge8c8 d4r8");
	m_trk(2,"c4c4  f8f4f8 e8.eg8g8 f4r8");
	m_trk(3,"f8c8f8c8 c8c8d8f8 e8g8f8e8d8>b8<c8");
	m_trk(1,"c&d e8g8g8.a g8e8c8d8 e8e8d8.dc4r8");
	m_trk(2,"e&f g8<c8c8.d c8>g8g8a8 <c8c8>b8.b <c4r8");
	m_trk(3,"d8 c8g8e8g8 c8g8e8f8 g8g8f8.f e8g8c8r8");
	for(ai = 0; ai <= 23 ;ai++) {
		m_trk(4,"c8r8ccr8");m_trk(5,"r8c8r8c8");
	}
}
