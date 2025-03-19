#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"  // nose si se puede


void update_flags(uint64_t result) {
    // Z: Zero flag
    if (result == 0) {
        CURRENT_STATE.FLAGS.Z = 1;
    } else {
        CURRENT_STATE.FLAGS.Z = 0;
    }

    // N: Negative flag
    if (result < 0) {
        CURRENT_STATE.FLAGS.N = 1;
    } else {
        CURRENT_STATE.FLAGS.N = 0;
    }

    // C: Carry flag
    if (result < 0) {
        CURRENT_STATE.FLAGS.C = 1;
    } else {
        CURRENT_STATE.FLAGS.C = 0;
    }

    // V: Overflow flag
    if (result < 0) {
        CURRENT_STATE.FLAGS.V = 1;
    } else {
        CURRENT_STATE.FLAGS.V = 0;
    }
}

void adds_extended(int d, int n, uint32_t imm12, int shift) {
    uint64_t operand1;
    if (n == 31) {
        operand1 = CURRENT_STATE.REGS[31]; // uso el registro especial XZR/WZR.
    } else {
        operand1 = CURRENT_STATE.REGS[n];
    }

    uint64_t imm;
    if (shift == 1) {
        imm = imm12 << 12; // desplazo 12 bits a la izquierda.
    } else {
        imm = imm12; // uso el valor inmediato sin cambios.
    }

    uint64_t result = operand1 + imm;

    update_flags(result);

    CURRENT_STATE.REGS[d] = result;
}


/* tengo que tener en cuenta el decode stage y el execute stage*/
void process_instruction() {
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     * */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    uint32_t opcode = (instruction >> 21) & 0b1111111;
    uint32_t rd = (instruction >> 7) & 0b11111;
    uint32_t rn = (instruction >> 12) & 0b11111;
    uint32_t rm = instruction & 0b11111;
    uint32_t imm12 = instruction & 0b111111111111;
    uint32_t shift = (instruction >> 22) & 0b11;

    switch (opcode)
    {
    case 0b1000101:
        adds_extended(rd, rn, imm12, shift);
        break;
    case 0b110001:
        adds_immediate(rd, rn, imm12, shift);
        break;
    case 0b1101011:
        subs_extended(rd, rn, imm12, shift);
        break;
    case 0b111001:
        subs_immediate(rd, rn, imm12, shift);
        break;
    case 0b1101001001:
        hlt();
        break;
    // case 0b1101011:
    //     cmp_extended(rd, rn, imm12, shift);
    //     break;
    // case 0b111001:  
    //     cmp_immediate(rd, rn, imm12, shift);
    //     break;
    case 0b11100100:
        ands_shifted_register(rd, rn, imm12, shift);
        break;
    case 0b1001010:
        eor_shifted_register(rd, rn, imm12, shift);
        break;
    case 0b10101010:
        orr_shifted_register(rd, rn, imm12, shift);
        break;
    case 0b101:
        b();
        break;
    case 0b1101011000011111000000:
        br();
        break;

    }
}

/*
ADDS (Extended Register, Immediate):
Immediate: adds X0, X1, 3 (descripción: X0 = X1 + 3, luego updatear flags)
El caso de shift == 01 se debe implementar, osea moviendo el imm12, 12 bits a la izquierda.
Tambien shift 00, pero no el ReservedValue
Extended Register: adds X0 = X1, X2 (descripción: X0 = X1 + X2, luego updatear flags)

Cuando compilamos la instrucción ADDS Xd, Xn, Xm el bit 21 de output es 0, no 1

Cuando hacemos el extended no tenemos que implementar la parte de extended y amount (O sea, pueden asumir que
es 0). Solo implementen ADDS Xd, Xn, Xm.

Para immediate no hay que implementar Reserved Value, pero si los otros dos.
*/









// int compares_ints( const void *a, const void *b)
// {
//     return ( *(int*)a - *(int*)b );
// }


/*    OP CODES
ADDS extended                 0 1 0 1 0 1 1 0 0 1
ADDS immediate                0 1 1 0 0 0 1
SUBS Extended Register        1 1 0 1 0 1 1 0 0 1
SUBS Immediate                1 1 1 0 0 0 1
HLT                           0x6a2         1 1 0 1 0 1 0 0 0 1 0 
CMP Extended Register         1 1 0 1 0 1 1 0 0 1 
CMP Immediate                 1 1 1 0 0 0 1
ANDS (shifted register)       1 1 1 0 0 1 0 0
EOR (shifted register)        1 0 0 1 0 1 0
ORR (shifted register)        0 1 0 1 0 1 0
B                             0 0 0 1 0 1 
BR                            1 1 0 1 0 1 1 0 0 0 0 1 1 1 1 1 0 0 0 0 0 0 
BEQ                           NO LO ENCONTRE
BNE                           NO LO ENCONTRE
BGT                           NO LO ENCONTRE
BLT                           NO LO ENCONTRE
BGE                           NO LO ENCONTRE
BLE                           NO LO ENCONTRE
LSL                           1 0 1 0 0 1 1 0 
LSR                           0 0 1 1 0 1 0 1 1 0 
STUR                          
STURB                          
STURH                         
LDUR                          
LDURH                         
LDURB                          
MOVZ                    
ADD extended
ADD immediate
MUL
CBZ
CBNZ

*/


/*
void adds_extended(int d, int n, uint32_t imm12, int shift) {
    uint64_t operand1;
    // determino el valor del operando 1 basado en el registro fuente (n).
    if (n == 31) {
        operand1 = CURRENT_STATE.REGS[31]; // uso el registro especial XZR/WZR.
    } else {
        operand1 = CURRENT_STATE.REGS[n];
    }

    // calculo el valor inmediato con o sin desplazamiento.
    uint64_t imm;
    if (shift == 1) {
        imm = imm12 << 12; // desplazo 12 bits a la izquierda.
    } else {
        imm = imm12; // uso el valor inmediato sin cambios.
    }

    // realizo la suma
    uint64_t result = operand1 + imm;

    // Actualizar los flags del procesador según el resultado.
    update_flags(result);

    // guardar el resultado en el registro destino (d).
    CURRENT_STATE.REGS[d] = result;
}

*/