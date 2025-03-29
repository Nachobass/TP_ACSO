.text
mov X1, 0x1000
lsl X1, X1, 16       // X1 = 0x100000000
mov X10, 0x1234      // Valor de prueba (16 bits)

sturh W10, [X1, 0x0] // Almacena los 16 bits menos significativos de X10 en memoria
sturh W10, [X1, 0x2] // Almacena nuevamente para comprobar lectura múltiple

ldurh W13, [X1, 0x0] // Carga 16 bits desde la dirección X1 a W13
ldurh W14, [X1, 0x2] // Carga 16 bits desde la dirección X1 + 2 a W14

HLT 0
