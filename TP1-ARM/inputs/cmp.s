.text
mov X19, #40
cmp X19, #20
mov X21, #30
cmp X21, #30
mov X22, #80
mov X23, #80
cmp X22, X23
mov X24, #90
mov X25, #50
cmp X24, X25

// Verificación de XZR
cmp X31, #10        // 0 - 0
cmp X31, #0         // 0 - 10 (debería establecer el flag de negativo)

HLT 0
