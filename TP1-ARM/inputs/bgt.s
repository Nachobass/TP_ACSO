.text
cmp X26, X27
bgt foo
adds X6, X0, 40

bar:
HLT 0

foo:
cmp X26, X27
bgt bar
adds X7, X0, 50
HLT 0
