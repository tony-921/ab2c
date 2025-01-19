   10 /* BMP load
   20 int x,y,k
   30 int fn,fp,e,c
   40 int R,G,B,I
   50 char buf(512*3),g0:int tr(255),tg(255),tb(255)
   60 /*
   70 screen 1,3,1,1
   80 print time$
   90 for c=0 to 255
  100   tr(c)=(c shr 3) shl 11
  101   tb(c)=(c shr 3) shl 1
  102   tg(c)=((c shr 3) shl 6)-((c and &B100)=&B100)
  110 next
  120 /*
  130 fn=fopen("test01.bmp","r")
  140 fp=fseek(fn,&H36,0)
  150 /*
  160 for y=0 to 511
  170   e=fread(buf,512*3,fn)
  180   for x=0 to 511
  190     k=x*3
  250     pset(x,511-y,tb(buf(k))+tg(buf(k+2))+tr(buf(k+1)))
  260   next
  270 next
  280 /*
  290 fcloseall()
  300 print time$
  310 input c
 