#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "shell.h"  // nose si se puede


void mem_write_64(uint64_t address, uint64_t value) {
    // Escribir la parte baja (32 bits menos significativos)
    mem_write_32(address, (uint32_t)(value & 0xFFFFFFFF));

    // Escribir la parte alta (32 bits más significativos)
    mem_write_32(address + 4, (uint32_t)((value >> 32) & 0xFFFFFFFF));
}

uint64_t mem_read_64(uint64_t address) {
    // Leer la parte baja (32 bits menos significativos)
    uint32_t low = mem_read_32(address);

    // Leer la parte alta (32 bits más significativos)
    uint32_t high = mem_read_32(address + 4);

    // Combinar las dos partes en un valor de 64 bits
    uint64_t value = ((uint64_t)high << 32) | low;

    return value;
}

void mem_write_16(uint64_t address, uint16_t value) {
    // Calcular la dirección base alineada a 4 bytes
    uint64_t aligned_address = address & ~0x3;

    // Leer los 32 bits actuales desde la dirección alineada
    uint32_t current_value = mem_read_32(aligned_address);

    // Calcular el desplazamiento de los 16 bits dentro de los 32 bits
    int halfword_offset = (address & 0x3) >> 1; // 0 para los bits menos significativos, 1 para los más significativos

    // Actualizar solo los 16 bits correspondientes
    current_value &= ~(0xFFFF << (halfword_offset * 16)); // Limpiar los 16 bits objetivo
    current_value |= (value << (halfword_offset * 16));   // Escribir el nuevo valor

    // Escribir los 32 bits actualizados de vuelta en la memoria
    mem_write_32(aligned_address, current_value);
}

uint16_t mem_read_16(uint64_t address) {
    // Calcular la dirección base alineada a 4 bytes
    uint64_t aligned_address = address & ~0x3;

    // Leer los 32 bits actuales desde la dirección alineada
    uint32_t current_value = mem_read_32(aligned_address);

    // Calcular el desplazamiento de los 16 bits dentro de los 32 bits
    int halfword_offset = (address & 0x3) >> 1; // 0 para los bits menos significativos, 1 para los más significativos

    // Extraer los 16 bits correspondientes
    uint16_t value = (current_value >> (halfword_offset * 16)) & 0xFFFF;

    return value;
}

void mem_write_8(uint64_t address, uint8_t value) {
    // Calcular la dirección base alineada a 4 bytes
    uint64_t aligned_address = address & ~0x3;

    // Leer los 32 bits actuales desde la dirección alineada
    uint32_t current_value = mem_read_32(aligned_address);

    // Calcular el desplazamiento del byte dentro de los 32 bits
    int byte_offset = address & 0x3;

    // Actualizar solo el byte correspondiente
    current_value &= ~(0xFF << (byte_offset * 8)); // Limpiar el byte objetivo
    current_value |= (value << (byte_offset * 8)); // Escribir el nuevo valor

    // Escribir los 32 bits actualizados de vuelta en la memoria
    mem_write_32(aligned_address, current_value);
}

uint8_t mem_read_8(uint64_t address) {
    // Calcular la dirección base alineada a 4 bytes
    uint64_t aligned_address = address & ~0x3;

    // Leer los 32 bits actuales desde la dirección alineada
    uint32_t current_value = mem_read_32(aligned_address);

    // Calcular el desplazamiento del byte dentro de los 32 bits
    int byte_offset = address & 0x3;

    // Extraer el byte correspondiente
    uint8_t value = (current_value >> (byte_offset * 8)) & 0xFF;

    return value;
}

static void update_flags(int64_t result) {
    NEXT_STATE.FLAG_Z = (result == 0);   
    NEXT_STATE.FLAG_N = (result < 0);
}

void adds_immediate(int rd, int rn, uint32_t imm12, int shift);
void adds_extended(int rd, int rn, int imm3, int option, int rm);
void subs_immediate(int rd, int rn, uint32_t imm12, int shift);
void subs_extended(int rd, int rn, int imm3, int option, int rm);
void hlt();
void cmp_immediate(int rd, int rn, uint32_t imm12, int shift);
void cmp_extended(int rd, int rn, int imm3, int option, int rm);
void ands_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift);
void eor_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift);
void orr_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift);
void execute_b(uint32_t imm26);
void execute_br(uint8_t rn);
void execute_b_cond(uint32_t imm19, uint8_t condition);
void lsl_immediate(int rd, int rn, uint32_t imms, uint32_t immr, int n);
void lsr_immediate(int rd, int rn, uint32_t imms, uint32_t immr, int n);
void stur(int rt, int rn, int imm9, int size);            //(hay dos ceros entre Rn y imm9, son para el escalado del offset)
void sturb(int rt, int rn, int imm9, int size);           //(hay dos ceros entre Rn y imm9, son para el escalado del offset)
void sturh(int rt, int rn, int imm9, int size);           //(hay dos ceros entre Rn y imm9, son para el escalado del offset)
void ldur(int rt, int rn, int imm9, int size);            //(hay dos ceros entre Rn y imm9, son para el escalado del offset)
void ldurh(int rt, int rn, int imm9, int size);           //(hay dos ceros entre Rn y imm9, son para el escalado del offset)
void ldurb(int rt, int rn, int imm9, int size);           //(hay dos ceros entre Rn y imm9, son para el escalado del offset)
void movz(int rd, int imm16, int hw);
void add_immediate(int rd, int rn, uint32_t imm12, int shift);
void add_extended(int rd, int rn, int imm3, int option, int rm);
void mul(int rd, int rn, int rm);
void cbz(int rt, int imm19);
void cbnz(int rt, int imm19);

// ME PARECE QUE NO EXISTE MEM_WRITE 8,16, 64

void process_instruction() {
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    NEXT_STATE = CURRENT_STATE;

    uint32_t opCode8 = (instruction >> 24) & 0xFF;
    uint32_t opCode9 = (instruction >> 23) & 0x1FF;
    uint32_t opCode11 = (instruction >> 21) & 0x7FF;
    uint32_t opCode6 = (instruction >> 26) & 0x3F;
    uint32_t opCode22 = (instruction >> 10) & 0x3FFFFF;

    printf("instruction: %x\n", instruction);
    printf("opcode8: %x\n", opCode8);
    printf("opcode_extended: %x\n", opCode11);

    if( opCode8 == 0xB1 ){              //                      ADDS IMMEDIATE
        uint32_t shift = (instruction >> 22) & 0x1;
        // uint32_t imm12 = instruction & 0xFFF;
        uint32_t imm12 = (instruction >> 10) & 0xFFF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        adds_immediate(rd, rn, imm12, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);

    }

    else if( opCode11 == 0x558){        //                      ADDS EXTENDED 0b10101011000
        uint32_t rm = (instruction >> 16) & 0xF;
        uint32_t option = (instruction >> 13) & 0x7;
        uint32_t imm3 = (instruction >> 10) & 0x7;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        adds_extended(rd, rn, imm3, option, rm);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    }

    else if( opCode8 == 0xF1){          //                      SUB immediate 0b11110001
        uint32_t shift = (instruction >> 22) & 0x1;
        uint32_t imm12 = (instruction >> 10) & 0xFFF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        if( rd == 0x1F){
            cmp_immediate(rd, rn, imm12, shift);
        } else{
            subs_immediate(rd, rn, imm12, shift);
        }
        
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }

    else if( opCode11 == 0x758){        //                      SUBS EXTENDED  0b11101011000
        uint32_t rm = (instruction >> 16) & 0xF;
        uint32_t option = (instruction >> 13) & 0x7;
        uint32_t imm3 = (instruction >> 10) & 0x7;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        if( rd == 0x1F ){
            cmp_extended(rd, rn, imm3, option, rm);
        } else {
            subs_extended(rd, rn, imm3, option, rm);
        }
        
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    }

    else if( opCode11 == 0x6A2 ){       //                      HLT
        hlt();
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    }

    else if( opCode8 == 0xEA ){         //                      ANDS (shifted register) 0b11101010
        uint32_t shift = (instruction >> 22) & 0x3;
        uint32_t rm = (instruction >> 16) & 0x1F;
        uint32_t imm6 = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        ands_shifted_register(rd, rn, imm6, rm, 0, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }

    else if( opCode8 == 0xCA ){         //                      EOR (shifted register) 0b11001010
        uint32_t shift = (instruction >> 22) & 0x3;
        uint32_t rm = (instruction >> 16) & 0x1F;
        uint32_t imm6 = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        eor_shifted_register(rd, rn, imm6, rm, 0, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }

    else if( opCode8 == 0xAA ){         //                      ORR (shifted register) 0b10101010
        uint32_t shift = (instruction >> 22) & 0x3;
        uint32_t rm = (instruction >> 16) & 0x1F;
        uint32_t imm6 = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        orr_shifted_register(rd, rn, imm6, rm, 0, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }
    
    else if( opCode6 == 0x5 ){          //                      B 0b000101
        int32_t imm26 = (instruction & 0x03FFFFFF); 
        execute_b(imm26);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode6);
    }

    else if( opCode22 == 0x3587C0 ){    //                      Br 0b1101011000011111000000 
        uint8_t rn = (instruction >> 5) & 0x1F;
        execute_br(rn);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode22);
    }

    else if( opCode8 == 0x54){          //                      B.cond 0b01010100
        int32_t imm19 = (instruction >> 5) & 0x7FFFF; 
        uint8_t condition = instruction & 0xF;
        execute_b_cond(imm19, condition);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);

    }

    else if( opCode9 == 0x1A6 ){        //                      LSL immediate 0b110100110
        // uint32_t n = (instruction >> 22) & 0x1;
        uint32_t immr = (instruction >> 16) & 0x3F;
        uint32_t imms = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        lsl_immediate(rd, rn, imms, immr, 0);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode9);
    }

    else if( opCode9 == 0x1A6 ){        //                      LSR immediate 0b110100110
        uint32_t immr = (instruction >> 16) & 0x3F;
        uint32_t imms = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        lsr_immediate(rd, rn, imms, immr, 0);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode9);
    }

    else if( opCode9 == 0x1E0 ){        //                       STUR 0b111100000
        uint32_t size = (instruction >> 30) & 0x3;
        uint32_t imm9 = (instruction >> 12) & 0x1FF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rt = instruction & 0x1F;
        stur(rt, rn, imm9, size);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode9);
    }

    else if( opCode9 == 0x1C0){         //                       STURB 0b111000000
        uint32_t size = (instruction >> 30) & 0x3;
        uint32_t imm9 = (instruction >> 12) & 0x1FF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rt = instruction & 0x1F;
        if(size == 0b00){
            sturb(rt, rn, imm9, size);
        } else if(size == 0b01){
            sturh(rt, rn, imm9, size);
        }
        // sturb(rt, rn, imm9, size);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode9);
    }

/*     else if( opCode9 == 0x1C0){  //                       STURH 0b111000000
        uint32_t size = (instruction >> 30) & 0x3;
        uint32_t imm9 = (instruction >> 12) & 0x1FF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rt = instruction & 0x1F;
        sturh(rt, rn, imm9, size);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    } */
   
    else if( opCode9 == 0x1C2){         //                       LDUR 0b111000010
        uint32_t size = (instruction >> 30) & 0x3;
        uint32_t imm9 = (instruction >> 12) & 0x1FF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rt = instruction & 0x1F;
        if( size == 0b10 || size == 0b11){
            ldur(rt, rn, imm9, size);    
        }
        else if( size == 0b00){
            ldurb(rt, rn, imm9, size);
        }
        else if( size == 0b01){
            ldurh(rt, rn, imm9, size);
        }
        
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode9);
    }

    else if( opCode9 == 0x1A5 ){        //                       MOVZ 0b110100101
        uint32_t hw = (instruction >> 21) & 0x3;
        uint32_t imm16 = (instruction >> 5) & 0xFFFF;
        uint32_t rd = instruction & 0x1F;
        movz(rd, imm16, hw);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode9);
    }

    else if( opCode8 == 0x51){          //                       ADD immediate 0b10010001
        uint32_t shift = (instruction >> 22) & 0x1;
        uint32_t imm12 = (instruction >> 10) & 0xFFF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        add_immediate(rd, rn, imm12, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode9);
    }

    else if( opCode11 == 0x459 ){       //                       ADD EXTENDED 0b10001011001
        uint32_t rm = (instruction >> 16) & 0xF;
        uint32_t option = (instruction >> 13) & 0x7;
        uint32_t imm3 = (instruction >> 10) & 0x7;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        add_extended(rd, rn, imm3, option, rm);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    } 

    else if( opCode11 == 0x4D8 ){       //                       MUL 0b10011011000
        uint32_t rm = (instruction >> 16) & 0xF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        mul(rd, rn, rm);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    }

    else if( opCode8 == 0xB4 ){         //                       CBZ 0b10110100
        uint32_t imm19 = (instruction >> 5) & 0x7FFFF;
        uint32_t rt = instruction & 0x1F;
        cbz(rt, imm19);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }

    else if( opCode8 == 0xB5 ){         //                       CBNZ 0b10110101
        uint32_t imm19 = (instruction >> 5) & 0x7FFFF;
        uint32_t rt = instruction & 0x1F;
        cbnz(rt, imm19);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }

    else{
        printf("Error: opcode no reconocido\n");
    }


    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}



void adds_immediate(int rd, int rn, uint32_t imm12, int shift) {
    uint64_t operand1;
    uint64_t imm;
    
    if (rn == 31) {
        operand1 = CURRENT_STATE.REGS[31]; // uso el registro especial XZR/WZR.
    } else {
        operand1 = CURRENT_STATE.REGS[rn];
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
    NEXT_STATE.REGS[rd] = result;


    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("imm: %llu\n", imm);
    printf("result: %llu\n", result);
    printf("d: %d\n", rd);
    printf("n: %d\n", rn);
    printf("imm12: %d\n", imm12);
    printf("shift: %d\n", shift);
}

void adds_extended(int rd, int rn, int imm3, int option, int rm) {
    uint64_t operand1 = (rn == 31) ? CURRENT_STATE.REGS[31] : CURRENT_STATE.REGS[rn];  // Si n == 31, usa el stack pointer (SP)
    uint64_t operand2 = CURRENT_STATE.REGS[rm];  // Segundo operando sin extender

    // Aplicar extensión según el campo 'option'
    switch (option) {
        case 0b000: operand2 = (uint8_t)operand2; break;  // UXTB (Unsigned Extend Byte)
        case 0b001: operand2 = (uint16_t)operand2; break; // UXTH (Unsigned Extend Halfword)
        case 0b010: operand2 = (uint32_t)operand2; break; // UXTW (Unsigned Extend Word)
        case 0b011: operand2 = operand2; break;          // UXTX (Unsigned Extend Doubleword, sin cambios)
        case 0b100: operand2 = (int8_t)operand2; break;  // SXTB (Sign Extend Byte)
        case 0b101: operand2 = (int16_t)operand2; break; // SXTH (Sign Extend Halfword)
        case 0b110: operand2 = (int32_t)operand2; break; // SXTW (Sign Extend Word)
        case 0b111: operand2 = operand2; break;          // SXTX (Sign Extend Doubleword, sin cambios)
        default:
            printf("Error: opción de extensión inválida (%d)\n", option);
            return;
    }

    // Aplicar el desplazamiento (imm3 debe estar entre 0 y 4)
    if (imm3 > 4) {
        printf("Error: imm3 inválido (%d), debe estar entre 0 y 4\n", imm3);
        return;
    }
    operand2 <<= imm3;

    // Suma con actualización de flags
    uint64_t result = operand1 + operand2;
    
    update_flags(result);

    // Guardar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;

    // Depuración
    printf("operand1: %llu\n", operand1);
    printf("operand2 (extendido y desplazado): %llu\n", operand2);
    printf("result: %llu\n", result);
    printf("d: %d, n: %d, rm: %d, imm3: %d, option: %d\n", rd, rn, rm, imm3, option);
}


void subs_immediate(int rd, int rn, uint32_t imm12, int shift) {
    uint64_t operand1;
    uint64_t imm;

    operand1 = (rn == 31) ? CURRENT_STATE.REGS[31] : CURRENT_STATE.REGS[rn]; // Uso el registro especial XZR/WZR si n es 31.

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

    uint64_t result = operand1 - imm;
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;


    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("imm: %llu\n", imm);
    printf("result: %llu\n", result);
    printf("d: %d\n", rd);
    printf("n: %d\n", rn);
    printf("imm12: %d\n", imm12);
    printf("shift: %d\n", shift);
}

void subs_extended(int rd, int rn, int imm3, int option, int rm) {
    uint64_t operand1 = (rn == 31) ? CURRENT_STATE.REGS[31] : CURRENT_STATE.REGS[rn];  // Si n == 31, usa el stack pointer (SP)
    uint64_t operand2 = CURRENT_STATE.REGS[rm];  // Segundo operando sin extender

    // Aplicar extensión según el campo 'option'
    switch (option) {
        case 0b000: operand2 = (uint8_t)operand2; break;  // UXTB (Unsigned Extend Byte)
        case 0b001: operand2 = (uint16_t)operand2; break; // UXTH (Unsigned Extend Halfword)
        case 0b010: operand2 = (uint32_t)operand2; break; // UXTW (Unsigned Extend Word)
        case 0b011: operand2 = operand2; break;          // UXTX (Unsigned Extend Doubleword, sin cambios)
        case 0b100: operand2 = (int8_t)operand2; break;  // SXTB (Sign Extend Byte)
        case 0b101: operand2 = (int16_t)operand2; break; // SXTH (Sign Extend Halfword)
        case 0b110: operand2 = (int32_t)operand2; break; // SXTW (Sign Extend Word)
        case 0b111: operand2 = operand2; break;          // SXTX (Sign Extend Doubleword, sin cambios)
        default:
            printf("Error: opción de extensión inválida (%d)\n", option);
            return;
    }

    // Aplicar el desplazamiento (imm3 debe estar entre 0 y 4)
    if (imm3 > 4) {
        printf("Error: imm3 inválido (%d), debe estar entre 0 y 4\n", imm3);
        return;
    }
    operand2 <<= imm3;

    // Resta con actualización de flags
    uint64_t result = operand1 - operand2;
    
    update_flags(result);

    // Guardar el resultado en el registro destino
    NEXT_STATE.REGS[rd] = result;

    // Depuración
    printf("operand1: %llu\n", operand1);
    printf("operand2 (extendido y desplazado): %llu\n", operand2);
    printf("result: %llu\n", result);
    printf("d: %d, n: %d, rm: %d, imm3: %d, option: %d\n", rd, rn, rm, imm3, option);
}


void hlt() {
    RUN_BIT = 0;
}


void cmp_immediate(int rd, int rn, uint32_t imm12, int shift) {
    uint64_t operand1;
    uint64_t imm;

    operand1 = (rn == 31) ? CURRENT_STATE.REGS[31] : CURRENT_STATE.REGS[rn]; // Uso el registro especial XZR/WZR si n es 31.

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

    uint64_t result = operand1 - imm;
    update_flags(result);


    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("imm: %llu\n", imm);
    printf("result: %llu\n", result);
    printf("d: %d\n", rd);
    printf("n: %d\n", rn);
    printf("imm12: %d\n", imm12);
    printf("shift: %d\n", shift);
}

void cmp_extended(int rd, int rn, int imm3, int option, int rm) {
    uint64_t operand1 = (rn == 31) ? CURRENT_STATE.REGS[31] : CURRENT_STATE.REGS[rn];  // Si n == 31, usa el stack pointer (SP)
    uint64_t operand2 = CURRENT_STATE.REGS[rm];  // Segundo operando sin extender

    // Aplicar extensión según el campo 'option'
    switch (option) {
        case 0b000: operand2 = (uint8_t)operand2; break;  // UXTB (Unsigned Extend Byte)
        case 0b001: operand2 = (uint16_t)operand2; break; // UXTH (Unsigned Extend Halfword)
        case 0b010: operand2 = (uint32_t)operand2; break; // UXTW (Unsigned Extend Word)
        case 0b011: operand2 = operand2; break;          // UXTX (Unsigned Extend Doubleword, sin cambios)
        case 0b100: operand2 = (int8_t)operand2; break;  // SXTB (Sign Extend Byte)
        case 0b101: operand2 = (int16_t)operand2; break; // SXTH (Sign Extend Halfword)
        case 0b110: operand2 = (int32_t)operand2; break; // SXTW (Sign Extend Word)
        case 0b111: operand2 = operand2; break;          // SXTX (Sign Extend Doubleword, sin cambios)
        default:
            printf("Error: opción de extensión inválida (%d)\n", option);
            return;
    }

    // Aplicar el desplazamiento (imm3 debe estar entre 0 y 4)
    if (imm3 > 4) {
        printf("Error: imm3 inválido (%d), debe estar entre 0 y 4\n", imm3);
        return;
    }
    operand2 <<= imm3;

    // Resta con actualización de flags
    uint64_t result = operand1 - operand2;
    
    update_flags(result);

    // Depuración
    printf("operand1: %llu\n", operand1);
    printf("operand2 (extendido y desplazado): %llu\n", operand2);
    printf("result: %llu\n", result);
    printf("d: %d, n: %d, rm: %d, imm3: %d, option: %d\n", rd, rn, rm, imm3, option);

}


void ands_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift) {
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    uint64_t result = operand1 & operand2;
        
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;

    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("operand2: %llu\n", operand2);
    printf("result: %llu\n", result);
    printf("rd: %d\n", rd);
    printf("rn: %d\n", rn);
    printf("imm6: %d\n", imm6);
    printf("rm: %d\n", rm);
    printf("n: %d\n", n);
    printf("shift: %d\n", shift);
}


void eor_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift) {
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    uint64_t result = operand1 ^ operand2;
    
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;

    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("operand2: %llu\n", operand2);
    printf("result: %llu\n", result);
    printf("rd: %d\n", rd);
    printf("rn: %d\n", rn);
    printf("imm6: %d\n", imm6);
    printf("rm: %d\n", rm);
    printf("n: %d\n", n);
    printf("shift: %d\n", shift);
}

void orr_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift) {
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    uint64_t result = operand1 | operand2;
    
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;

    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("operand2: %llu\n", operand2);
    printf("result: %llu\n", result);
    printf("rd: %d\n", rd);
    printf("rn: %d\n", rn);
    printf("imm6: %d\n", imm6);
    printf("rm: %d\n", rm);
    printf("n: %d\n", n);
    printf("shift: %d\n", shift);
}


void execute_b(uint32_t imm26) {
    int64_t offset = ((int64_t)imm26 << 2); 
    
    if (imm26 & (1 << 25)) { 
        offset |= 0xFFFFFFFFFC000000; 
    }
    
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    
    // Depuración
    printf("Branch Execution:\n");
    printf("Current PC: 0x%llx\n", CURRENT_STATE.PC);
    printf("Offset: %lld (0x%llx)\n", offset, offset);
    printf("New PC: 0x%llx\n", NEXT_STATE.PC);
}

void execute_br(uint8_t rn) {
    uint64_t target = CURRENT_STATE.REGS[rn];
    
    NEXT_STATE.PC = target;
    
    // Depuración
    printf("Branch Register Execution:\n");
    printf("Register X%d contains address: 0x%llx\n", rn, target);
    printf("New PC: 0x%llx\n", NEXT_STATE.PC);
}

void execute_b_cond(uint32_t imm19, uint8_t condition) { 
    int64_t offset = ((int64_t)imm19 << 2); 
    
    if (imm19 & (1 << 18)) { // Si el bit 18 está encendido, es negativo
        offset |= 0xFFFFFFFFFFF80000; 
    }
    
    
    bool cond;
    switch (condition) {
        case 0x0: cond = CURRENT_STATE.FLAG_Z; break;  // BEQ (Z == 1)
        case 0x1: cond = !CURRENT_STATE.FLAG_Z; break; // BNE (Z == 0)
        case 0xA: cond = !CURRENT_STATE.FLAG_N && !CURRENT_STATE.FLAG_Z; break; // BGT (N == 0 && Z == 0)
        case 0xB: cond = CURRENT_STATE.FLAG_N; break; // BLT (N == 1)
        case 0xC: cond = !CURRENT_STATE.FLAG_N; break; // BGE (N == 0)
        case 0xD: cond = CURRENT_STATE.FLAG_Z || CURRENT_STATE.FLAG_N; break; // BLE (Z == 1 || N == 1)
        // default: return 0; 
        default:
            printf("Error: condición inválida (%d)\n", condition);
            return;
    }
    if (cond) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
    }
    
    // Depuración
    printf("Conditional Branch Execution:\n");
    printf("Current PC: 0x%llx\n", CURRENT_STATE.PC);
    printf("Offset: %lld (0x%llx)\n", offset, offset);
    printf("Condition: 0x%x\n", condition);

}


void lsl_immediate(int rd, int rn, uint32_t imms, uint32_t immr, int n) {
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t shift = (immr & 0x3F) - (imms & 0x3F);
    uint64_t result = operand1 << shift;
    
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;

    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("result: %llu\n", result);
    printf("rd: %d\n", rd);
    printf("rn: %d\n", rn);
    printf("imms: %d\n", imms);
    printf("immr: %d\n", immr);
    printf("n: %d\n", n);
}

void lsr_immediate(int rd, int rn, uint32_t imms, uint32_t immr, int n) {
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t shift = (immr & 0x3F) - (imms & 0x3F);
    uint64_t result = operand1 >> shift;
    
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;

    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("result: %llu\n", result);
    printf("rd: %d\n", rd);
    printf("rn: %d\n", rn);
    printf("imms: %d\n", imms);
    printf("immr: %d\n", immr);
    printf("n: %d\n", n);
}


/* void stur(int rt, int rn, int imm9) {
    // Sign-extend imm9 a 64 bits (maneja valores negativos correctamente)
    int64_t offset = (int64_t)((imm9 << 55) >> 55);  
    uint64_t address = CURRENT_STATE.REGS[rn] + offset;
    uint64_t data = CURRENT_STATE.REGS[rt];
    mem_write_64(address, data);

    // Depuración
    printf("STUR Execution:\n");
    printf("Register X%d contains data: 0x%llx\n", rt, data);
    printf("Register X%d contains address: 0x%llx\n", rn, CURRENT_STATE.REGS[rn]);
    printf("Offset (sign-extended): %ld\n", offset);
    printf("Memory address: 0x%llx\n", address);
    printf("Data stored: 0x%llx\n", data);
} */

void stur(int rt, int rn, int imm9, int size) {
    // Sign-extend imm9 a 64 bits (para manejar valores negativos correctamente)
    // int64_t offset = (int64_t)((imm9 << 55) >> 55);  
    int64_t offset = (((int64_t)imm9 << 55) >> 55);  
    uint64_t address = CURRENT_STATE.REGS[rn] + offset;
    // Determinar el tamaño de datos a escribir
    if (size == 2) { // 32-bit word
        uint32_t data = (uint32_t)(CURRENT_STATE.REGS[rt]); // Solo los 32 bits inferiores
        mem_write_32(address, data);
        printf("STUR (32 bits): X%d -> Mem[0x%llx] = 0x%x\n", rt, address, data);
    } else if (size == 3) { // 64-bit double word
        uint64_t data = CURRENT_STATE.REGS[rt]; // Almacenar 64 bits completos
        mem_write_64(address, data);
        printf("STUR (64 bits): X%d -> Mem[0x%llx] = 0x%llx\n", rt, address, data);
    } else {
        printf("Error: Tamaño inválido en STUR (size=%d)\n", size);
    }

    // Depuración
    printf("STUR Execution:\n");
    // printf("Register X%d contains data: 0x%llx\n", rt, data);
    printf("Register X%d contains address: 0x%llx\n", rn, CURRENT_STATE.REGS[rn]);
    printf("Offset (sign-extended): %lld\n", offset);
    printf("Memory address: 0x%llx\n", address);
    // printf("Data stored: 0x%llx\n", data);
}

void sturb(int rt, int rn, int imm9, int size) {
    // Sign-extend imm9 a 64 bits (maneja valores negativos correctamente)
    // int64_t offset = (int64_t)((imm9 << 55) >> 55);  
    int64_t offset = (((int64_t)imm9 << 55) >> 55);  
    uint64_t address = CURRENT_STATE.REGS[rn] + offset;
    uint64_t data = CURRENT_STATE.REGS[rt] & 0xFF;
    mem_write_8(address, data);

    // Depuración
    printf("STURB (Store Byte) Execution:\n");
    printf("Register X%d contains data: 0x%llx\n", rt, data);
    printf("Register X%d contains address: 0x%llx\n", rn, CURRENT_STATE.REGS[rn]);
    printf("Offset (sign-extended): %lld\n", offset);
    printf("Memory address: 0x%llx\n", address);
    printf("Data stored: 0x%llx\n", data);
}

void sturh(int rt, int rn, int imm9, int size) {
    // Sign-extend imm9 a 64 bits (maneja valores negativos correctamente)
    // int64_t offset = (int64_t)((imm9 << 55) >> 55);  
    int64_t offset = (((int64_t)imm9 << 55) >> 55);  
    uint64_t address = CURRENT_STATE.REGS[rn] + offset;
    uint64_t data = CURRENT_STATE.REGS[rt] & 0xFFFF;
    mem_write_16(address, data);

    // Depuración
    printf("STURH (Store Halfword) Execution:\n");
    printf("Register X%d contains data: 0x%llx\n", rt, data);
    printf("Register X%d contains address: 0x%llx\n", rn, CURRENT_STATE.REGS[rn]);
    printf("Offset (sign-extended): %lld\n", offset);
    printf("Memory address: 0x%llx\n", address);
    printf("Data stored: 0x%llx\n", data);
}

// if (size == 2 || size == 3) {  //  (LDUR)
void ldur(int rt, int rn, int imm9, int size) {
    // Sign-extend imm9 a 64 bits (para manejar valores negativos correctamente)
    // int64_t offset = (int64_t)((imm9 << 55) >> 55);  
    int64_t offset = (((int64_t)imm9 << 55) >> 55);  
    uint64_t address = CURRENT_STATE.REGS[rn] + offset;
    uint64_t data;
    // Determinar el tamaño de datos a leer
    if (size == 2) { // 32-bit word
        data = mem_read_32(address);
        printf("LDUR (32 bits): Mem[0x%llx] -> X%d = 0x%llx\n", address, rt, data);
    } else if (size == 3) { // 64-bit double word
        data = mem_read_64(address);
        printf("LDUR (64 bits): Mem[0x%llx] -> X%d = 0x%llx\n", address, rt, data);
    } else {
        printf("Error: Tamaño inválido en LDUR (size=%d)\n", size);
    }

    // Guardar el dato leído en el registro destino
    NEXT_STATE.REGS[rt] = (uint64_t)data;

    // Depuración
    printf("LDUR Execution:\n");
    printf("Register X%d contains address: 0x%llx\n", rn, CURRENT_STATE.REGS[rn]);
    printf("Offset (sign-extended): %lld\n", offset);
    printf("Memory address: 0x%llx\n", address);
    printf("Data loaded: 0x%llx\n", data);
}

// if (size == 1) {  // 16-bit (LDURH)
void ldurh(int rt, int rn, int imm9, int size) {
    // Extender signo de imm9 (manejar valores negativos)
    // int64_t offset = (int64_t)((imm9 << 55) >> 55);
    int64_t offset = (((int64_t)imm9 << 55) >> 55);
    uint64_t address = CURRENT_STATE.REGS[rn] + offset;
    uint16_t data = mem_read_16(address);
    NEXT_STATE.REGS[rt] = (uint64_t)data;

    // Depuración
    printf("LDURH: Mem[0x%llx] -> W%d = 0x%04x\n", address, rt, data);
    printf("LDURH Execution:\n");
    printf("Register X%d contains address: 0x%llx\n", rn, CURRENT_STATE.REGS[rn]);
    printf("Offset (sign-extended): %lld\n", offset);
    printf("Memory address: 0x%llx\n", address);
    printf("Data loaded: 0x%hx\n", data);
}

    // if (size == 0) {  // 8-bit (LDURB)
void ldurb(int rt, int rn, int imm9, int size) {
    // Extender signo de imm9 (manejar valores negativos)
    // int64_t offset = (int64_t)((imm9 << 55) >> 55);
    int64_t offset = (((int64_t)imm9 << 55) >> 55);
    uint64_t address = CURRENT_STATE.REGS[rn] + offset;
    uint8_t data = mem_read_8(address);
    NEXT_STATE.REGS[rt] = (uint64_t)data;

    // Depuración
    printf("LDURB: Mem[0x%llx] -> W%d = 0x%02x\n", address, rt, data);
    printf("LDURB Execution:\n");
    printf("Register X%d contains address: 0x%llx\n", rn, CURRENT_STATE.REGS[rn]);
    printf("Offset (sign-extended): %lld\n", offset);
    printf("Memory address: 0x%llx\n", address);
    printf("Data loaded: 0x%hhx\n", data);
}

// solo implementamos con hw=0, es decir shift=0
void movz(int rd, int imm16, int hw){
    uint64_t imm = (uint64_t)imm16;
    imm <<= (16 * hw);
    NEXT_STATE.REGS[rd] = imm;

    // Depuración
    printf("MOVZ Execution:\n");
    printf("Immediate value: 0x%llx\n", imm);
    printf("Register X%d = 0x%llx\n", rd, imm);
    printf("hw: %d\n", hw);
}


void add_immediate(int rd, int rn, uint32_t imm12, int shift) {
    uint64_t operand1;
    uint64_t imm;

    operand1 = (rn == 31) ? CURRENT_STATE.REGS[31] : CURRENT_STATE.REGS[rn]; // Uso el registro especial XZR/WZR si n es 31.

    printf("operand1: %llu\n", operand1);

    // Solo se permiten shifts 0 y 1 según el manual de ARM
    if (shift == 0b00) {
        imm = imm12;  // LSL #0 (sin desplazamiento)
    } else if (shift == 0b01) {
        imm = imm12 << 12;  // LSL #12 (desplazamiento de 12 bits)
    } else {
        printf("Error: shift inválido (%d). Solo 0 y 1 están permitidos.\n", shift);
        return;
    }

    uint64_t result = operand1 + imm;
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;
}

void add_extended(int rd, int rn, int imm3, int option, int rm){
    uint64_t operand1 = (rn == 31) ? CURRENT_STATE.REGS[31] : CURRENT_STATE.REGS[rn];  // Si n == 31, usa el stack pointer (SP)
    uint64_t operand2 = CURRENT_STATE.REGS[rm];  // Segundo operando sin extender

    switch (option) {
        case 0b000: operand2 = (uint8_t)operand2; break;  // UXTB (Unsigned Extend Byte)
        case 0b001: operand2 = (uint16_t)operand2; break; // UXTH (Unsigned Extend Halfword)
        case 0b010: operand2 = (uint32_t)operand2; break; // UXTW (Unsigned Extend Word)
        case 0b011: operand2 = operand2; break;          // UXTX (Unsigned Extend Doubleword, sin cambios)
        case 0b100: operand2 = (int8_t)operand2; break;  // SXTB (Sign Extend Byte)
        case 0b101: operand2 = (int16_t)operand2; break; // SXTH (Sign Extend Halfword)
        case 0b110: operand2 = (int32_t)operand2; break; // SXTW (Sign Extend Word)
        case 0b111: operand2 = operand2; break;          // SXTX (Sign Extend Doubleword, sin cambios)
        default:
            printf("Error: opción de extensión inválida (%d)\n", option);
            return;
    }

    // Aplicar el desplazamiento (imm3 debe estar entre 0 y 4)
    if (imm3 > 4) {
        printf("Error: imm3 inválido (%d), debe estar entre 0 y 4\n", imm3);
        return;
    }
    operand2 <<= imm3;

    uint64_t result = operand1 + operand2;
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;

    // Depuración
    printf("operand1: %llu\n", operand1);
    printf("operand2 (extendido y desplazado): %llu\n", operand2);
    printf("result: %llu\n", result);
    printf("d: %d, n: %d, rm: %d, imm3: %d, option: %d\n", rd, rn, rm, imm3, option);

}


void mul(int rd, int rn, int rm) {
    uint64_t operand1 = CURRENT_STATE.REGS[rn];
    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    uint64_t result = operand1 * operand2;
    
    update_flags(result);
    NEXT_STATE.REGS[rd] = result;

    // Depuracion 
    printf("operand1: %llu\n", operand1);
    printf("operand2: %llu\n", operand2);
    printf("result: %llu\n", result);
    printf("rd: %d\n", rd);
    printf("rn: %d\n", rn);
    printf("rm: %d\n", rm);
}


void cbz(int rt, int imm19) {
    uint64_t operand = CURRENT_STATE.REGS[rt];

    // Sign-extend imm19 a 64 bits y multiplicar por 4
    // int64_t offset = (int64_t)((imm19 << 45) >> 45) << 2;
    int64_t offset = (((int64_t)imm19 << 45) >> 45) << 2;

    // Si el registro es cero, actualizar el PC
    if (operand == 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        printf("CBZ: X%d == 0, salto a 0x%llx\n", rt, NEXT_STATE.PC);
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;  // Avanzar a la siguiente instrucción
        printf("CBZ: X%d != 0, no salta\n", rt);
    }

    // Depuración
    printf("CBZ Execution:\n");
    printf("Register X%d contains value: 0x%llx\n", rt, operand);
    printf("Current PC: 0x%llx\n", CURRENT_STATE.PC);
    printf("Offset: %lld (0x%llx)\n", offset, offset);
    printf("New PC: 0x%llx\n", NEXT_STATE.PC);
}

void cbnz(int rt, int imm19) {
    uint64_t operand = CURRENT_STATE.REGS[rt];

    // Sign-extend imm19 a 64 bits y multiplicar por 4
    int64_t offset = ((int64_t)imm19 << 45) >> 45 << 2;
    // int64_t offset = ((int64_t)(imm19 & 0x7FFFF) << 2);
    // if (imm19 & (1 << 18)) { // Si el bit 18 está encendido, es un número negativo
    //     offset |= 0xFFFFFFFFFFF80000; // Extender el signo manualmente
    // }

    // Si el registro no es cero, actualizar el PC
    if (operand != 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        printf("CBNZ: X%d != 0, salto a 0x%llx\n", rt, NEXT_STATE.PC);
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;  // Avanzar a la siguiente instrucción
        printf("CBNZ: X%d == 0, no salta\n", rt);
    }

    // Depuración
    printf("CBNZ Execution:\n");
    printf("Register X%d contains value: 0x%llx\n", rt, operand);
    printf("Current PC: 0x%llx\n", CURRENT_STATE.PC);
    printf("Offset: %lld (0x%llx)\n", offset, offset);
    printf("New PC: 0x%llx\n", NEXT_STATE.PC);
}

