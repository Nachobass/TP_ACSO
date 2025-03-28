.text
cmp X1, X2
ble foo
adds X12, X0, 100

bar:
HLT 0

foo:
cmp X1, X2
ble bar
adds X13, X0, 110
HLT 0
