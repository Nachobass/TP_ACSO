.section .text
.global _start

_start:
    // Guardamos la dirección de retorno manualmente
    ADR X30, return_point  // X30 = dirección de "return_point"
    B funcion              // Salta a "funcion"

return_point:
    // Esto se ejecuta cuando regresamos de la función
    ADDS X1, X0, #1        // X1 = 1
    HLT #0

// Función que se salta
funcion:
    ADDS X2, X0, #2        // X2 = 2 (se ejecuta si B funcionó)
    BR X30                 // Volver a la dirección guardada en X30
