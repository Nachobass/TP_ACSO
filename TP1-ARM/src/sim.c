// #include <stdio.h>
// #include <assert.h>
// #include <string.h>
// #include "shell.h"  // nose si se puede


// static void update_flags(int64_t result) {
//     NEXT_STATE.FLAG_Z = (result == 0);   
//     NEXT_STATE.FLAG_N = (result < 0);
// }

// void adds_immediate(int d, int n, uint32_t imm12, int shift) {
//     uint64_t operand1;
//     uint64_t imm;
    
//     if (n == 31) {
//         operand1 = CURRENT_STATE.REGS[31]; // uso el registro especial XZR/WZR.
//     } else {
//         operand1 = CURRENT_STATE.REGS[n];
//     }

//     printf("operand1: %ld\n", operand1);

//     // Solo se permiten shifts 0 y 1 según el manual de ARM
//     if (shift == 0) {
//         imm = imm12;  // LSL #0 (sin desplazamiento)
//     } else if (shift == 1) {
//         imm = imm12 << 12;  // LSL #12 (desplazamiento de 12 bits)
//     } else {
//         printf("Error: shift inválido (%d). Solo 0 y 1 están permitidos.\n", shift);
//         return;
//     }

//     uint64_t result = operand1 + imm;
//     update_flags(result);
//     CURRENT_STATE.REGS[d] = result;

//     // Depuracion 
//     printf("operand1: %lu\n", operand1);
//     printf("imm: %lu\n", imm);
//     printf("result: %lu\n", result);
//     printf("d: %d\n", d);
//     printf("n: %d\n", n);
//     printf("imm12: %u\n", imm12);
//     printf("shift: %u\n", shift);

//     // printf("result: %ld\n", result);
//     // printf("d: %d\n", d);
//     // printf("n: %d\n", n);
//     // printf("imm12: %d\n", imm12);
//     // printf("shift: %d\n", shift);
// }

// void ands_shifted_register(uint32_t instruction) {
//     uint8_t rd = (instruction >> 0) & 0x1F;  // Extraer Rd (registro destino)
//     uint8_t rn = (instruction >> 5) & 0x1F;  // Extraer Rn (primer operando)
//     uint8_t rm = (instruction >> 16) & 0x1F; // Extraer Rm (segundo operando)
    
//     uint64_t operand1 = CURRENT_STATE.REGS[rn];
//     uint64_t operand2 = CURRENT_STATE.REGS[rm];
//     uint64_t result = operand1 & operand2;
    
//     // Guardar el resultado en el registro destino
//     CURRENT_STATE.REGS[rd] = result;
    
//     // Actualizar las banderas N y Z
//     NEXT_STATE.FLAG_N = (result >> 63) & 1; // Si el bit más significativo es 1, N = 1
//     NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Si el resultado es 0, Z = 1
    
//     // Avanzar el PC
//     NEXT_STATE.PC = CURRENT_STATE.PC + 4;
// }

// /* tengo que tener en cuenta el decode stage y el execute stage*/
// void process_instruction() {
//     /* execute one instruction here. You should use CURRENT_STATE and modify
//      * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
//      * access memory. 
//      * */
//     uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
//     uint32_t opcode = (instruction >> 24) & 0xFF;
//     uint32_t rd = (instruction >> 7 ) & 0b11111;
//     uint32_t rn = (instruction >> 12) & 0b11111;
//     uint32_t rm = instruction & 0b11111;
//     uint32_t imm12 = instruction & 0b111111111111;
//     uint32_t shift = (instruction >> 22) & 0b11;
//     printf("instruction: %x\n", instruction);
//     printf("opcode: %x\n", opcode);
//     switch (opcode)
//     {
//     // case 0xb1:
//     //     adds_extended(rd, rn, imm12, shift);
//     //     break;
//     case 0b10110001:
//         adds_immediate(rd, rn, imm12, shift);
//     case 0b11100100:
//         // ands_shifted_register(rd, rn, imm12, shift);
//         ands_shifted_register(instruction);
//         break;
//     }
//     NEXT_STATE = CURRENT_STATE;
// }

// /*
// ADDS (Extended Register, Immediate):
// Immediate: adds X0, X1, 3 (descripción: X0 = X1 + 3, luego updatear flags)
// El caso de shift == 01 se debe implementar, osea moviendo el imm12, 12 bits a la izquierda.
// Tambien shift 00, pero no el ReservedValue
// Extended Register: adds X0 = X1, X2 (descripción: X0 = X1 + X2, luego updatear flags)

// Cuando compilamos la instrucción ADDS Xd, Xn, Xm el bit 21 de output es 0, no 1

// Cuando hacemos el extended no tenemos que implementar la parte de extended y amount (O sea, pueden asumir que
// es 0). Solo implementen ADDS Xd, Xn, Xm.

// Para immediate no hay que implementar Reserved Value, pero si los otros dos.
// */









// // int compares_ints( const void *a, const void *b)
// // {
// //     return ( *(int*)a - *(int*)b );
// // }


// /*    OP CODES
// ADDS extended                 0 1 0 1 0 1 1 0 0 1
// ADDS immediate                0 1 1 0 0 0 1
// SUBS Extended Register        1 1 0 1 0 1 1 0 0 1
// SUBS Immediate                1 1 1 0 0 0 1
// HLT                           0x6a2         1 1 0 1 0 1 0 0 0 1 0 
// CMP Extended Register         1 1 0 1 0 1 1 0 0 1 
// CMP Immediate                 1 1 1 0 0 0 1
// ANDS (shifted register)       1 1 1 0 0 1 0 0
// EOR (shifted register)        1 0 0 1 0 1 0
// ORR (shifted register)        0 1 0 1 0 1 0
// B                             0 0 0 1 0 1 
// BR                            1 1 0 1 0 1 1 0 0 0 0 1 1 1 1 1 0 0 0 0 0 0 
// BEQ                           NO LO ENCONTRE
// BNE                           NO LO ENCONTRE
// BGT                           NO LO ENCONTRE
// BLT                           NO LO ENCONTRE
// BGE                           NO LO ENCONTRE
// BLE                           NO LO ENCONTRE
// LSL                           1 0 1 0 0 1 1 0 
// LSR                           0 0 1 1 0 1 0 1 1 0 
// STUR                          
// STURB                          
// STURH                         
// LDUR                          
// LDURH                         
// LDURB                          
// MOVZ                    
// ADD extended
// ADD immediate
// MUL
// CBZ
// CBNZ

// */


// /* a
// void adds_extended(int d, int n, uint32_t imm12, int shift) {
//     uint64_t operand1;
//     // determino el valor del operando 1 basado en el registro fuente (n).
//     if (n == 31) {
//         operand1 = CURRENT_STATE.REGS[31]; // uso el registro especial XZR/WZR.
//     } else {
//         operand1 = CURRENT_STATE.REGS[n];
//     }

//     // calculo el valor inmediato con o sin desplazamiento.
//     uint64_t imm;
//     if (shift == 1) {
//         imm = imm12 << 12; // desplazo 12 bits a la izquierda.
//     } else {
//         imm = imm12; // uso el valor inmediato sin cambios.
//     }

//     // realizo la suma
//     uint64_t result = operand1 + imm;

//     // Actualizar los flags del procesador según el resultado.
//     update_flags(result);

//     // guardar el resultado en el registro destino (d).
//     CURRENT_STATE.REGS[d] = result;
// }

// */












/* EN LIMPIO  */



#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"  // nose si se puede


static void update_flags(int64_t result) {
    NEXT_STATE.FLAG_Z = (result == 0);   
    NEXT_STATE.FLAG_N = (result < 0);
}

void adds_immediate(int d, int n, uint32_t imm12, int shift);

void process_instruction() {
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    NEXT_STATE = CURRENT_STATE;

    uint32_t opCode8 = (instruction >> 24) & 0xFF;

    if( opCode8 == 0xB1 ){          // ADDS IMMEDIATE
        uint32_t shift = (instruction >> 22) & 0x1;
        uint32_t imm12 = instruction & 0xFFF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        adds_immediate(rd, rn, imm12, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);

    }

    // if( opCode8 == )

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}



void adds_immediate(int d, int n, uint32_t imm12, int shift) {
    uint64_t operand1;
    uint64_t imm;
    
    if (n == 31) {
        operand1 = CURRENT_STATE.REGS[31]; // uso el registro especial XZR/WZR.
    } else {
        operand1 = CURRENT_STATE.REGS[n];
    }

    printf("operand1: %llu\n", operand1);

    // Solo se permiten shifts 0 y 1 según el manual de ARM
    if (shift == 0) {
        imm = imm12;  // LSL #0 (sin desplazamiento)
    } else if (shift == 1) {
        imm = imm12 << 12;  // LSL #12 (desplazamiento de 12 bits)
    } else {
        printf("Error: shift inválido (%d). Solo 0 y 1 están permitidos.\n", shift);
        return;
    }

    uint64_t result = operand1 + imm;
    update_flags(result);
    NEXT_STATE.REGS[d] = result;


    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("imm: %llu\n", imm);
    printf("result: %llu\n", result);
    printf("d: %d\n", d);
    printf("n: %d\n", n);
    printf("imm12: %d\n", imm12);
    printf("shift: %d\n", shift);
}

