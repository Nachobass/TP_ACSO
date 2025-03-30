.text
movz X1, 0x1000
lsl X1, X1, 16       // X1 = 0x100000000

movz X10, 0x1234      // Primer valor de prueba (16 bits)
movz X11, 0xABCD      // Segundo valor de prueba (16 bits)

sturh W10, [X1, 0x0] // Almacena 0x1234 en la dirección X1
sturh W11, [X1, 0x2] // Almacena 0xABCD en la dirección X1 + 2

ldurh W13, [X1, 0x0] // Carga 16 bits desde X1 a W13 (esperado: 0x1234)
ldurh W14, [X1, 0x2] // Carga 16 bits desde X1 + 2 a W14 (esperado: 0xABCD)

HLT 0

