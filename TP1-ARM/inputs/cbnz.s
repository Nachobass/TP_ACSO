.text
movz X3, 1
cbnz X3, foo
adds X16, X0, 140

bar:
HLT 0

foo:
cbnz X3, bar
adds X17, X0, 150
HLT 0
