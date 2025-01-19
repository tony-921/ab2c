   10 /* BMP load
   20 int x,y,n,t
   30 int fn
   40 char R,G,B,k
   50 char buf(256)
   60 /*
   70 screen 1,1,1,1
   71 apage(2):vpage(15)
   80 /*
   90 print time$
  100 fn=fopen("bg4.bmp","r")
  110 fp=fseek(fn,&H36,0)
  120 /*
  125   e=fread(buf,16*4,fn)
  130 for n=0 to 15
  140   B=buf(n*4) shr 3
  150   R=buf(n*4+2)shr 3
  160   G=buf(n*4+1)shr 3
  170   palet(n,rgb(R,G,B))
  190 next
  200 for y=0 to 511
  210   e=fread(buf,256,fn)
  220   for x=0 to 255
  230     k=buf(x)
  240     pset(x*2,511-y,k shr 4)
  245     pset(x*2+1,511-y,k and &B1111)
  250   next
  260 next
  270 /*
  280 fcloseall()
  290 print time$
  300 input n
