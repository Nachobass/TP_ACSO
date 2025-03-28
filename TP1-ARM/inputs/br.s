.text

mov x1, jump_target    // Cargar la dirección de salto en X1
br x1                  // Saltar a la dirección en X1
mov x3, #7             // Código que no se ejecutará


jump_target:
mov x0, #42            // Código que se ejecuta después del salto
mov x8, #93            // syscall: exit
svc #0                 // Salir del programa
