.text
// Caso 1: Desplazamiento pequeño 

movz X0, 0x10         // X0 = 0x10
lsl X1, X0, 2        // X1 = X0 << 2, X1 = 0x40
HLT 0

//Caso 2: Desplazamiento grande 

movz X0, 0x10         // X0 = 0x10
lsl X1, X0, 60       // X1 = X0 << 60, X1 = 0x0
HLT 0

// Caso 3: Desplazamiento que da como resultado 0 (por ejemplo, desplazando todo el valor)

movz X0, 0x10  // X0 = 0x10 (16 en decimal)
lsl X1, X0, 4        // X1 = X0 << 4, X1 = 0x100
lsl X2, X0, 8        // X2 = X0 << 8, X2 = 0x1000
lsl X3, X0, 16       // X3 = X0 << 16, X3 = 0x10000
lsl X4, X0, 32       // X4 = X0 << 32, X4 = 0x100000000
HLT 0

// Caso 4: Desplazamiento de un valor muy pequeño

movz X0, 0x1  // X0 = 0x01 (1 en decimal)
lsl X1, X0, 1        // X1 = X0 << 1, X1 = 0x02
lsl X2, X0, 31       // X2 = X0 << 31, X2 = 0x80000000
HLT 0