.text
movz X19, #40
cmp X19, #20
movz X21, #30
cmp X21, #30
movz X22, #80
movz X23, #80
cmp X22, X23
movz X24, #90
movz X25, #50
cmp X24, X25

// Verificación de XZR (X31)
movz X26, #0
cmp XZR, X26  // 0 - 0  válido
cmp XZR, X25  // 0 - X25  válido (X25 tiene 50)

HLT 0
