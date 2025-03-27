// easy

mov X2, 0x00000040        // Cargar el valor 0x40 en X2
lsr X3, X2, 2             // Desplazar X2 hacia la derecha 2 bits
HLT 0                      // Terminar ejecución


// Caso 1: Desplazamiento pequeño (por ejemplo, 2 bits)

mov X0, 0x00000010  // X0 = 0x10 (16 en decimal)
lsr X1, X0, 2       // X1 = X0 >> 2, X1 = 0x4 (4 en decimal)
HLT 0

// Caso 2: Desplazamiento grande (por ejemplo, 60 bits)

mov X0, 0x00000010  // X0 = 0x10 (16 en decimal)
lsr X1, X0, 60      // X1 = X0 >> 60, X1 = 0x0 (underflow)
HLT 0

// Caso 3: Desplazamiento que da como resultado 0 (por ejemplo, desplazando todo el valor)

mov X0, 0x00000010  // X0 = 0x10 (16 en decimal)
lsr X1, X0, 4       // X1 = X0 >> 4, X1 = 0x1 (1 en decimal)
lsr X2, X0, 8       // X2 = X0 >> 8, X2 = 0x0 (0 en decimal)
HLT 0

// Caso 4: Desplazamiento de un valor muy pequeño

mov X0, 0x00000001  // X0 = 0x01 (1 en decimal)
lsr X1, X0, 1       // X1 = X0 >> 1, X1 = 0x0 (0 en decimal)
lsr X2, X0, 0       // X2 = X0 >> 0, X2 = 0x1 (1 en decimal)
HLT 0
