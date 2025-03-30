.text 

adds X0, X10, #1
adds X1, X11, #11
cmp X0, X1          // flag_n = 1
adds X3, XZR, X1    // Solo para visualizar que CMP se ejecuta
cmp X1, X0          // flag_n = 0
adds X4, X12, #1    // x4 = 0x1
cmp X0, X4          // flag_n = 0 y flag_z = 1

HLT #0