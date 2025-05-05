.text
cmp X30, XZR
bge foo
adds X10, X0, 80

bar:
HLT 0

foo:
cmp X30, XZR
bge bar
adds X11, X0, 90
HLT 0
