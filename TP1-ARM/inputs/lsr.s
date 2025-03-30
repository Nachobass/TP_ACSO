movz X2, 0x00000040        // Cargar el valor 0x40 en X2
lsr X3, X2, 2              // Desplazar X2 hacia la derecha 2 bits
HLT 0                      // Terminar ejecución


// Caso 1: Desplazamiento pequeño (por ejemplo, 2 bits)

movz X0, 0x00000010  // X0 = 0x10
lsr X1, X0, 2       // X1 = X0 >> 2, X1 = 0x4
HLT 0

// Caso 2: Desplazamiento grande (por ejemplo, 60 bits)

movz X0, 0x00000010  // X0 = 0x10
lsr X1, X0, 60      // X1 = X0 >> 60, X1 = 0x0
HLT 0

// Caso 3: Desplazamiento que da como resultado 0 (por ejemplo, desplazando todo el valor)

movz X0, 0x00000010  // X0 = 0x10
lsr X1, X0, 4       // X1 = X0 >> 4, X1 = 0x1
lsr X2, X0, 8       // X2 = X0 >> 8, X2 = 0x0
HLT 0

// Caso 4: Desplazamiento de un valor muy pequeño

movz X0, 0x00000001  // X0 = 0x01
lsr X1, X0, 1       // X1 = X0 >> 1, X1 = 0x0
lsr X2, X0, 0       // X2 = X0 >> 0, X2 = 0x1
HLT 0
