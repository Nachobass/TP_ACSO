.section .text
.global _start

_start:
    // Calculamos la dirección de return_point sumando un offset al PC
    MOV X30, X30          // Asegurar que X30 no tenga basura (opcional)
    ADD X30, X30, #0x00400012     // X30 = dirección de return_point (ajustar el offset según sea necesario)
    B funcion             // Salta a "funcion"

return_point:
    // Esto se ejecuta cuando regresamos de la función
    ADDS X1, X0, #1       // X1 = 1
    HLT #0

// Función que se salta
funcion:
    ADDS X2, X0, #2       // X2 = 2 (se ejecuta si el salto funcionó)
    BR X30                // Volver a la dirección guardada en X30
