  100 /* music sample "�����A�X�U���i!!" S.C.Foster
  110 int ai
  120 str as,bs,cs,s1[50]                /*�����ϐ��̐錾
  130 s1=" ���F�ύX[H] ���t[P] ��~[S] �ĊJ[C] �I��=[E] :"
  140 m_init() /*FM�����̏�����
  150 m_sysch("OPM") /*�����̃`�����l���ԍ��̊���
  160 cls
  170 M_CHANGE(): MML_IN():m_play()
  180 repeat
  190         locate 0,1:print chr$(5);s1;: input "", cs
  200         if cs="H" or cs="h" then { /*����H�Ȃ特�F�ύX
  210                 M_CHANGE():MML_IN() : m_play() }
  220         if cs="P" or cs="p" then { /*����P�Ȃ牉�t
  230                 m_play()}
  240         if cs="S" or cs="s" then { /*����S�Ȃ��~
  250                 m_stop()}
  260         if cs="C" or cs="c" then { /*���� C �Ȃ�ĊJ
  270                 m_cont()  }
  280         if cs="E" or cs="e" then { /*����E�Ȃ�I��
  290                 m_stop()}
  300 until cs="e" or cs="E"                 /*cs=E�Ȃ� repeat �I��
  310 end
  320 /*-------- ���F�ύX ----------
  330 func int M_CHANGE()
  340 for ai=1 to 5
  350         m_alloc(ai, 2000) /*�g���b�N�o�b�t�@�̊m��
  360         m_assign(ai,ai) /*�g���b�N�o�b�t�@�w�`�����l��������
  370 next
  380 locate 0,0: print chr$(5);
  390 linput"���F�ԍ��̎w��(1~200)-->?";as
  400 print chr$(5);
  410 linput "�e���|�̎w�� (20~300)-->?";bs
  420 endfunc
  430 /*---- MML �f�[�^�̊i�[-----
  440 func int MML_IN()
  450 m_trk(1,"q7 116 04 v14 t"+bs+""+as)
  460 m_trk(2,"97 116 4 v10 t"+bs+""+as)
  470 m_trk(3,"97 116 04 v10 t"+bs+"6"+as)
  480 m_trk(4,"q7 116 03 v15 t"+bs+"645")
  490 m_trk(5,"q7 116 od v15 t"+bs+"@47")
  500 m_trk(1,"<c&d e8g8g8a8 g8e8c8.d e8e8d8c8 d4.")
  510 m_trk(2,"e&f g8<c8c8d8 c8>g8g8.f g8g8f8e8 f4.")
  520 m_trk(3,"�s8 r8 >c8g8e8g8 c8g8e8g8 c8g8e8g8 >b8<g8d8")
  530 m_trk(1,"c&d e8g8g8.a g8e8c8.d e8e8d8d8 c4.")
  540 m_trk(2,"e&f g8<c8c8.d c8g8g8.f g8g8f8f8 e4.")
  550 m_trk(3,"g8 c8g8e8g8 c8g8e8g8 c8g8d8g8 c8g8c8")
  560 m_trk(1,"c&d e8g8g8.a g8e8c8.d e8e8d8c8d4.")
  570 m_trk(2, "e&f g8<c8c8.d c8>g8g8.f g8g8f8e8 f4.")
  580 m_trk(3,"r8 c8g8e8g8 c8g8e8g8c8g8e8g8 >b8<g8d8")
  590 m_trk(1,"c&d e8g8g8a8 g8.ec8.d e8e8d8.d c4r4")
  600 m_trk(2,"e&f g8<c8c8d8 c8.>gg8.f g8g8f8.f e4r4") 
  610 m_trk(3,"g8 c8g8e8g8 c8g8e8g8 c8g8d8g8 c8c8d8e8")
  620 m_trk(1,"14f4 a8a4a8 g8.ge8c8 d4r8")
  630 m_trk(2,"c4c4  f8f4f8 e8.eg8g8 f4r8")
  640 m_trk(3,"f8c8f8c8 c8c8d8f8 e8g8f8e8d8>b8<c8") 
  650 m_trk(1,"c&d e8g8g8.a g8e8c8d8 e8e8d8.dc4r8") 
  660 m_trk(2,"e&f g8<c8c8.d c8>g8g8a8 <c8c8>b8.b <c4r8")
  670 m_trk(3,"d8 c8g8e8g8 c8g8e8f8 g8g8f8.f e8g8c8r8")
  680 for ai=0 to 23
  690         m_trk(4,"c8r8ccr8"):m_trk(5,"r8c8r8c8")
  700 next 
  710 endfunc
