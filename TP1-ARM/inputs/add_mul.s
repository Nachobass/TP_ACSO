.text
// Test para ADD Immediate
movz X1, 5
add X2, X1, 3

// Test para ADD Extended
movz X3, 4
movz X4, 2
add X5, X3, X4, LSL 1

// Test para MUL
movz X6, 3
movz X7, 7
mul X8, X6, X7

HLT 0
