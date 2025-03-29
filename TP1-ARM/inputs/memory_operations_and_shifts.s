.text
movz    X1, 0           // Dirección base en memoria
movz    X2, 0xabcd      // Valor a almacenar
stur    X2, [X1, 0x10]  // Almacena X2 en memoria (offset 0x10)
ldur    X3, [X1, 0x10]  // Carga desde memoria a X3

// Verificación: X3 debería ser igual a X2
cmp     X3, X2
bne     error           // Si no son iguales, salta a error

// Prueba de almacenamiento/carga de byte
movz    W4, 0xef        // Cargar un byte específico
sturb   W4, [X1, 0x20]  // Almacenar solo el byte menos significativo
ldurb   W5, [X1, 0x20]  // Cargarlo de vuelta

cmp     W5, W4
bne     error

// Prueba de almacenamiento/carga de medio-word (16 bits)
movz    W6, 0x1234
sturh   W6, [X1, 0x30]  // Almacenar 16 bits
ldurh   W7, [X1, 0x30]  // Cargar 16 bits

cmp     W7, W6
bne     error

// Operaciones de desplazamiento (shift)
lsl     X8, X3, 2       // X8 = X3 << 2
lsr     X9, X3, 1       // X9 = X3 >> 1

// Verificación de comparación entre X8 y X9
cmp     X8, X9
bge     skip2           // Si X8 >= X9, salta

adds    X10, X0, 1      // Si X8 < X9, suma 1 a X10

skip2:
cbnz    X5, end2        // Si X5 no es cero, salta a end2
adds    X11, X0, 2      // Si X5 es cero, suma 2 a X11

end2:
hlt     0               // Fin del programa

error:
movz    X12, 0xdead     // Indicar error con un valor reconocible
hlt     0
