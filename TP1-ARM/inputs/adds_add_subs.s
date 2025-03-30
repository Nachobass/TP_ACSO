.text

// ADDS Immediate con Shift = 0
movz    X0, 5              // X0 = 5
adds    X1, X0, #3         // X1 = X0 + 3 = 5 + 3 = 8 (sin shift)

// ADDS Immediate con Shift = 12
adds    X2, X0, #3, LSL #12  // X2 = X0 + (3 << 12) = 5 + 12288

// ADDS Extended
movz    X3, 10
movz    X4, 2
adds    X5, X3, X4, UXTB   // X5 = X3 + (X4 extendido como byte)

// ADD Immediate con Shift = 0
add     X6, X0, #4         // X6 = X0 + 4 = 5 + 4 = 9 (sin shift)

// ADD Immediate con Shift = 12 ---
add     X7, X0, #4, LSL #12 // X7 = X0 + (4 << 12) = 5 + 16384

// ADD Extended
movz    X8, 7
movz    X9, 3
add     X10, X8, X9  // X10 = X8 + X9

// SUBS Immediate sin Shift
movz    X11, 15
subs    X12, X11, #5       // X12 = X11 - 5 = 15 - 5 = 10

// SUBS Immediate con Shift = 12
subs    X13, X11, #2, LSL #12  // X13 = X11 - (2 << 12) = 15 - 8192

// SUBS Extended
movz    X14, 20
movz    X15, 4
subs    X16, X14, X15, UXTB  // X16 = X14 - (X15 extendido como byte)

HLT    0
