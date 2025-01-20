10 /* BMP load
20 int x,y,k
30 int fn,fp,e
40 int R,G,B,I,g0
50 dim char buf(1536)
60 /*
70 /* screen 1,3,1,1
80 /*
90 print time$
100 fn=fopen("test01.bmp","r")
110 fp=fseek(fn,&H36,0)
120 /*
130 for y=0 to 512
140   e=fread(buf,512*3,fn)
150   for x=0 to 511
160     k=x*3
170     B=(buf(k) shr 3) shl 1
180     g0=buf(k+1)
190     G=(g0 shr 3) shl 11
200     R=(buf(k+2) shr 3) shl 6
210     I=(g0 and 4)=4
220     /* pset(x,512-y,G+R+B-I)
230   next
240 next
250 /*
260 fcloseall()
270 print time$
280 input e
 