.text
cmp X24, X25
bne foo
adds X4, X0, 20

bar:
HLT 0

foo:
cmp X24, X25
bne bar
adds X5, X0, 30
HLT 0
