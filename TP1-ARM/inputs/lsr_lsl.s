.text
// Caso 1: Probar desplazamiento en ambas direcciones

movz X0, 0x00000010  
lsl X1, X0, 2       
lsr X2, X1, 3       
HLT 0

// Caso 2: Probar grandes desplazamientos combinados

movz X0, 0xFFFFFFFFFFFFFFFF 
lsl X1, X0, 2              
lsr X2, X1, 4              
HLT 0
