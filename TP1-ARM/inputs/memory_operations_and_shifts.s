.text
movz    X1, 0          
movz    X2, 0xabcd     
stur    X2, [X1, 0x10]  
ldur    X3, [X1, 0x10]  

// Verificación: X3 debería ser igual a X2
cmp     X3, X2
bne     error           // Si no son iguales, salta a error

// Prueba de almacenamiento/carga de byte
movz    W4, 0xef       
sturb   W4, [X1, 0x20]  
ldurb   W5, [X1, 0x20]  

cmp     W5, W4
bne     error

// Prueba de almacenamiento/carga de medio-word 
movz    W6, 0x1234
sturh   W6, [X1, 0x30]  
ldurh   W7, [X1, 0x30] 

cmp     W7, W6
bne     error

// Operaciones de desplazamiento (shift)
lsl     X8, X3, 2       
lsr     X9, X3, 1      

// Verificación de comparación entre X8 y X9
cmp     X8, X9
bge     skip2           

adds    X10, X0, 1      

skip2:
cbnz    X5, end2        
adds    X11, X0, 2      

end2:
hlt     0              

error:
movz    X12, 0xdead     // Indicar error con un valor reconocible
hlt     0
