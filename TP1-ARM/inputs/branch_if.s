.text
// Prueba de CBZ (Comparar con cero y saltar)
movz   X1, 8
cbz    X1, B1           // No salta, porque X1 != 0
adds   X2, X0, 10       // Se ejecuta, X1 no es cero

B1:
// Comparación BLT (Branch if Less Than)
movz   X3, 20
movz   X4, 25
cmp    X3, X4
blt    B2               // Salta porque 20 < 25
adds   X5, X0, 12       // No se ejecuta

B2:
// Comparación BEQ (Branch if Equal)
movz   X6, 15
movz   X7, 15
cmp    X6, X7
beq    B3               // Siempre se cumple (X6 == X7)
adds   X8, X0, 14       // No se ejecuta

B3:
// Comparación BGT (Branch if Greater Than)
movz   X9, 40
movz   X10, 30
cmp    X9, X10
bgt    B4               // Salta porque 40 > 30
adds   X11, X0, 16      // No se ejecuta

B4:
// Comparación BGE (Branch if Greater or Equal)
movz   X12, 18
movz   X13, 18
cmp    X12, X13
bge    B5               // Salta porque 18 >= 18
adds   X14, X0, 18      // No se ejecuta

B5:
// Comparación BLE (Branch if Less or Equal)
movz   X15, 8
movz   X16, 12
cmp    X15, X16
ble    B6               // Salta porque 8 <= 12
adds   X17, X0, 20      // No se ejecuta

B6:
// Prueba de CBNZ (Comparar con cero y saltar si NO es cero)
movz   X18, 4
cbnz   X18, B7          // Salta porque X18 != 0
adds   X19, X0, 22      // No se ejecuta

B7:
// Comparación BNE (Branch if Not Equal)
movz   X20, 10
movz   X21, 15
cmp    X20, X21
bne    B8               // Salta porque 10 != 15
adds   X22, X0, 24      // No se ejecuta

B8:
b      B9               // Salta a B9
adds   X23, X0, 26      // Nunca se ejecuta

B9:
HLT    0                // Finaliza la ejecución