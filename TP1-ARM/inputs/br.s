.section .text
.global _start

_start:
    // Cargar el valor base 0x00400000 usando desplazamientos
    MOV X30, #0x4000         // Cargar 0x4000
    LSL X30, X30, #8         // X30 = 0x400000 (desplazamos 8 bits)
    
    // Sumar el offset de la instrucción return_point
    ADD X30, X30, #16        // X30 = 0x00400000 + 16 (ajustar si es necesario)
    
    B funcion                // Salta a "funcion"

return_point:
    // Esto se ejecuta cuando regresamos de la función
    ADDS X1, X0, #1          // X1 = 1
    HLT #0

// Función que se salta
funcion:
    ADDS X2, X0, #2          // X2 = 2 (se ejecuta si el salto funcionó)
    BR X30                   // Volver a la dirección guardada en X30
