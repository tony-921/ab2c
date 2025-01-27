   10 int i
   20 dim int na(20)
   30 str s = "Hello world. how are you ?"
   40 int fp, fp2
   45 fdelete("test.bin")
   50 fp = fopen("test.bin", "c")
   60 for i = 0 to 10
   70 fputc(asc("a"), fp)
   80 next
   90 for i = 0 to 20
  100   na(i) = i
  110 next
  120 fwrite(na, 20, fp)
  130 fwrites(s, fp)
  140 fclose(fp)
  150 /*
  160 fp2 = fopen("test.bin", "r")
  170 for i = 0 to 10
  180   print chr$(fgetc(fp2));" ";
  190 next
  200 print
  210 /*
  220 fread(na, 20, fp2)
  230 fclose(fp2)
  240 for i = 0 to 20
  250   print hex$(na(i));" ";
  260 next
  