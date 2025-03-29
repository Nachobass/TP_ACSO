.text
// Test de operaciones ADD, ADDS, SUBS con diferentes modos de direccionamiento

// --- ADDS Immediate con Shift = 0 ---
movz    X0, 5              // X0 = 5
adds    X1, X0, #3         // X1 = X0 + 3 = 5 + 3 = 8 (sin shift)

// --- ADDS Immediate con Shift = 1 ---
adds    X2, X0, #3, LSL #1 // X2 = X0 + (3 << 1) = 5 + 6 = 11

// --- ADDS Extended ---
movz    X3, 10
movz    X4, 2
adds    X5, X3, X4, UXTB   // X5 = X3 + X4 (extendido como byte)

// --- ADD Immediate con Shift = 0 ---
add     X6, X0, #4         // X6 = X0 + 4 = 5 + 4 = 9 (sin shift)

// --- ADD Immediate con Shift = 1 ---
add     X7, X0, #4, LSL #1 // X7 = X0 + (4 << 1) = 5 + 8 = 13

// --- ADD Extended ---
movz    X8, 7
movz    X9, 3
add     X10, X8, X9, SXTB  // X10 = X8 + X9 (extendido como byte con signo)

// --- SUBS Immediate sin Shift ---
movz    X11, 15
subs    X12, X11, #5       // X12 = X11 - 5 = 15 - 5 = 10

// --- SUBS Immediate con Shift ---
subs    X13, X11, #2, LSL #2  // X13 = X11 - (2 << 2) = 15 - 8 = 7

// --- SUBS Extended ---
movz    X14, 20
movz    X15, 4
subs    X16, X14, X15, UXTB  // X16 = X14 - X15 (extendido como byte)

HLT    0
