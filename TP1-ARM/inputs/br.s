.section .text
.global _start

_start:
    // Simulamos una llamada a función
    BL funcion           // Salta y guarda PC+4 en X30

    // Esto se ejecuta cuando regresamos de la función
    ADDS X5, X4, #7      // X1 = 1
    HLT #0

// Función que se salta
funcion:
    ADDS X6, X4, #2      // X2 = 2 (se ejecuta si BL funcionó)
    BR X30               // Volver a la instrucción después del BL