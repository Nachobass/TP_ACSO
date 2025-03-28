// Caso 1: Desplazamiento pequeño (por ejemplo, 2 bits)

mov X0, 0x10         // X0 = 0x10 (16 en decimal)
lsl X1, X0, 2        // X1 = X0 << 2, X1 = 0x40 (64 en decimal)
HLT 0

//Caso 2: Desplazamiento grande (por ejemplo, 60 bits)

mov X0, 0x10         // X0 = 0x10 (16 en decimal)
lsl X1, X0, 60       // X1 = X0 << 60, X1 = 0x0 (overflow)
HLT 0

// Caso 3: Desplazamiento que da como resultado 0 (por ejemplo, desplazando todo el valor)

mov X0, 0x10  // X0 = 0x10 (16 en decimal)
lsl X1, X0, 4        // X1 = X0 << 4, X1 = 0x100 (256 en decimal)
lsl X2, X0, 8        // X2 = X0 << 8, X2 = 0x1000 (4096 en decimal)
lsl X3, X0, 16       // X3 = X0 << 16, X3 = 0x10000 (65536 en decimal)
lsl X4, X0, 32       // X4 = X0 << 32, X4 = 0x100000000 (en 64 bits)
HLT 0

// Caso 4: Desplazamiento de un valor muy pequeño

mov X0, 0x1  // X0 = 0x01 (1 en decimal)
lsl X1, X0, 1        // X1 = X0 << 1, X1 = 0x02 (2 en decimal)
lsl X2, X0, 31       // X2 = X0 << 31, X2 = 0x80000000 (2147483648 en decimal)
HLT 0