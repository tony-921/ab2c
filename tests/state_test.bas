10 int a=1
20 int b=2
30 char ch
40 int fp=3
50 dim int na(20)
60 str s
70 str prompt

100 exit(-1)

200 fclose(fp)
210 fcloseall()
220 fdelete(fp)
230 fputc('a', fp)
240 frename("oldfile.txt", "newfile.txt")
250 fseek(fp, 1, 2)
260 fwrite(na, 1, fp)

300 randomize(123)

400 srand(123)

500 ? a
510 ? "Hello"

600 input a
610 input "input int value", a
620 input s + "string", s



