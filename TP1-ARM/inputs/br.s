.section .text
.global _start

_start:
    // Cargar el valor base 0x00400000 usando desplazamientos
    movz X30, #0x4000        
    lsl X30, X30, #8        
    
    // Sumar el offset de la instrucción return_point
    add X30, X30, #16        // X30 = 0x00400000 + 16 (ajustar si es necesario)
    
    b funcion                

return_point:
    adds X1, X0, #1          
    HLT #0

// Función que se salta
funcion:
    adds X2, X0, #2          // (se ejecuta si el salto funciono)
    br X30                   // Volver a la dirección guardada en X30
