.text
.global _start

_start:

//BEQ: Salta si X10 == X11 (Probamos con positivo y cero)
    movz    X10, 0
    movz    X11, 0
    cmp     X10, X11
    beq     case_equal_zero
    adds    X12, X0, #99    // No se ejecuta

case_equal_zero:
    adds    X12, X0, #1     // Se ejecuta

//BNE: Salta si X13 != X14 (Probamos con números negativos)
    movz    X13, 0xFFFF    // -1 en complemento a dos
    movz    X14, 2
    cmp     X13, X14
    bne     case_not_equal
    adds    X15, X0, #88   // No se ejecuta

case_not_equal:
    adds    X15, X0, #2    // Se ejecuta

//BGT: Salta si X16 > X17 (Probamos con cero y un número positivo)
    movz    X16, 10
    movz    X17, 0
    cmp     X16, X17
    bgt     case_greater
    adds    X18, X0, #77   // No se ejecuta

case_greater:
    adds    X18, X0, #3    // Se ejecuta

// BLT: Salta si X19 < X20 (usamos valores positivos en lugar de negativos)
    movz    X19, 10    // X19 = 10
    movz    X20, 15    // X20 = 15
    cmp     X19, X20
    blt     case_less
    adds    X21, X0, #66   // No se ejecuta
case_less:
    adds    X21, X0, #99   // Se ejecuta

//BGE: Salta si X22 >= X23 (Probamos con dos números iguales, grandes)
    movz    X22, 5000
    movz    X23, 5000
    cmp     X22, X23
    bge     case_greater_equal
    adds    X24, X0, #55   // No se ejecuta

case_greater_equal:
    adds    X24, X0, #5    // Se ejecuta

//BLE: Salta si X25 <= X26 (Probamos con cero y negativo)
    movz    X25, 0
    movz    X26, 0xFFFF    // -1 en complemento a dos
    cmp     X25, X26
    ble     case_less_equal
    adds    X27, X0, #44   // No se ejecuta

case_less_equal:
    adds    X27, X0, #6    // Se ejecuta

//Comprobación de múltiples condiciones
    movz    X28, 7
    movz    X29, 10
    cmp     X28, X29
    blt     nested_case_1   

    adds    X30, X0, #33   // No se ejecuta

nested_case_1:
    cmp     X29, X28
    bgt     nested_case_2   

    adds    X1, X0, #22   // No se ejecuta

nested_case_2:
    adds    X1, X0, #11   // Se ejecuta

//Branch incondicional
    b       jump_case
    adds    X2, X0, #15   // No se ejecuta

jump_case:
    adds    X2, X0, #20   // Se ejecuta

    HLT     0
