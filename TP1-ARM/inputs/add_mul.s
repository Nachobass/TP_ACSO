.text
// Test para ADD Immediate
mov X1, 5
add X2, X1, 3  // X2 = X1 + 3

// Test para ADD Extended
mov X3, 4
mov X4, 2
add X5, X3, X4, LSL 1  // X5 = X3 + (X4 << 1) = 4 + (2 * 2) = 8     RARO ME DA 6

// Test para MUL
mov X6, 3
mov X7, 7
mul X8, X6, X7  // X8 = X6 * X7 = 3 * 7 = 21        RARO ME DA 15

HLT 0
