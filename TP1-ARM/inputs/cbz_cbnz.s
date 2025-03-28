.text
// CBZ debe saltar
mov X1, 0
cbz X1, salto1
mov X5, 42  // No debe ejecutarse si salta

salto1:
// CBZ no debe saltar
mov X2, 5
cbz X2, salto2
mov X6, 99  // Debe ejecutarse

salto2:
// CBNZ debe saltar
mov X3, 7
cbnz X3, salto3
mov X7, 123  // No debe ejecutarse si salta

salto3:
// CBNZ no debe saltar
mov X4, 0
cbnz X4, fin
mov X8, 200  // Debe ejecutarse

fin:
HLT 0
