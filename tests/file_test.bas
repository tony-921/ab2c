10 dim int na(10), nb(10)
20 int fp1, fp2
30 int i
40 for i = 0 to 10
50   na(i) = i
60 next
70 /* write to a file
80 fp1 = fopen("filetest.bin", "c")
90 fwrite(na, 10, fp1)
100 fclose(fp1)
110 /* read from the file
120 fp2 = fopen("filetest.bin", "r")
130 fread(nb, 10, fp2)
140 fclose(fp2)
150 /*
160 for i = 0 to 10
170   print nb(i)
180 next
