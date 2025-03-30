// Caso 1: Probar desplazamiento en ambas direcciones

movz X0, 0x00000010  // X0 = 0x10 (16 en decimal)
lsl X1, X0, 2       // X1 = X0 << 2, X1 = 0x40 (64 en decimal)
lsr X2, X1, 3       // X2 = X1 >> 3, X2 = 0x8 (8 en decimal)
HLT 0

// Caso 2: Probar grandes desplazamientos combinados

movz X0, 0xFFFFFFFFFFFFFFFF  // X0 = 0xFFFFFFFFFFFFFFFF (valor máximo de 64 bits)
lsl X1, X0, 2               // X1 = X0 << 2, X1 = 0xFFFFFFFFFFFFFF00
lsr X2, X1, 4               // X2 = X1 >> 4, X2 = 0x0FFFFFFFFFFFFFF
HLT 0
