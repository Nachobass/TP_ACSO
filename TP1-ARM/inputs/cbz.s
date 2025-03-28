.text
mov X1, 1
cbz X1, foo
adds X14, X0, 120

bar:
HLT 0

foo:
cbz X1, bar
adds X15, X0, 130
HLT 0
