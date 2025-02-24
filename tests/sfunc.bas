  10 str s1, s2
  20 s1 = hello("Mike")
  30 s2 = hello("Mike") + howareyou()
  40 print s1
  50 print s2
  60 end
  100 func str hello(name;str)
  110 return("Hello " + name)
  130 endfunc
  140 /*
  200 func str howareyou()
  210   return("How are you?")
  220 endfunc
  