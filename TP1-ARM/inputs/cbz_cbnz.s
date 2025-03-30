.text
// CBZ debe saltar
movz X1, 0
cbz X1, salto1
movz X5, 42  // No debe ejecutarse si salta

salto1:
// CBZ no debe saltar
movz X2, 5
cbz X2, salto2
movz X6, 99  // Debe ejecutarse

salto2:
// CBNZ debe saltar
movz X3, 7
cbnz X3, salto3
movz X7, 123  // No debe ejecutarse si salta

salto3:
// CBNZ no debe saltar
movz X4, 0
cbnz X4, fin
movz X8, 200  // Debe ejecutarse

fin:
HLT 0
