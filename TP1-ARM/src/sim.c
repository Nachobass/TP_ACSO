#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "shell.h"  // nose si se puede


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

    if( opCode8 == 0xB1 ){          // ADDS IMMEDIATE
        uint32_t shift = (instruction >> 22) & 0x1;
        // uint32_t imm12 = instruction & 0xFFF;
        uint32_t imm12 = (instruction >> 10) & 0xFFF;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        adds_immediate(rd, rn, imm12, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);

    }

    else if( opCode11 == 0x558){   //0b10101011000            ADDS EXTENDED
        uint32_t rm = (instruction >> 16) & 0xF;
        uint32_t option = (instruction >> 13) & 0x7;
        uint32_t imm3 = (instruction >> 10) & 0x7;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        adds_extended(rd, rn, imm3, option, rm);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    }

    else if( opCode8 == 0xF1){        //0b11110001    SUB immediate
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

    else if( opCode11 == 0x758){   //0b11101011000              SUBS EXTENDED
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

    else if( opCode11 == 0x6A2 ){       //            HLT
        hlt();
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    }

    else if( opCode8 == 0xEA ){       //            ANDS (shifted register) 0b11101010
        uint32_t shift = (instruction >> 22) & 0x3;
        uint32_t rm = (instruction >> 16) & 0x1F;
        uint32_t imm6 = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        ands_shifted_register(rd, rn, imm6, rm, 0, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }

    else if( opCode8 == 0xCA ){       //            EOR (shifted register) 0b11001010
        uint32_t shift = (instruction >> 22) & 0x3;
        uint32_t rm = (instruction >> 16) & 0x1F;
        uint32_t imm6 = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        eor_shifted_register(rd, rn, imm6, rm, 0, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }

    else if( opCode8 == 0xAA ){       //            ORR (shifted register) 0b10101010
        uint32_t shift = (instruction >> 22) & 0x3;
        uint32_t rm = (instruction >> 16) & 0x1F;
        uint32_t imm6 = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        orr_shifted_register(rd, rn, imm6, rm, 0, shift);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }
    
    else if( opCode6 == 0x5 ){       //            B 0b000101
        int32_t imm26 = (instruction & 0x03FFFFFF); 
        execute_b(imm26);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);
    }

    else if( opCode22 == 0x3587C0 ){  //  Br 0b1101011000011111000000 
        uint8_t rn = (instruction >> 5) & 0x1F;
        execute_br(rn);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode22);
    }

    else if( opCode8 == 0x54){ // B.cond 0b01010100
        int32_t imm19 = (instruction >> 5) & 0x7FFFF; 
        uint8_t condition = instruction & 0xF;
        execute_b_cond(imm19, condition);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode8);

    }

    else if( opCode9 == 0x1A6 ){       //            LSL immediate 0b110100110
        // uint32_t n = (instruction >> 22) & 0x1;
        uint32_t immr = (instruction >> 16) & 0x3F;
        uint32_t imms = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        lsl_immediate(rd, rn, imms, immr, 0);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
    }

    else if( opCode9 == 0b110100110 ){
        uint32_t immr = (instruction >> 16) & 0x3F;
        uint32_t imms = (instruction >> 10) & 0x3F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t rd = instruction & 0x1F;
        lsr_immediate(rd, rn, imms, immr, 0);
        printf("instruction: %x\n", instruction);
        printf("opcode: %x\n", opCode11);
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


void subs_immediate(int rd, int rn, uint32_t imm12, int shift){
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

void subs_extended(int rd, int rn, int imm3, int option, int rm){
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


void hlt(){
    RUN_BIT = 0;
}


void cmp_immediate(int rd, int rn, uint32_t imm12, int shift){
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

void cmp_extended(int rd, int rn, int imm3, int option, int rm){
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


void ands_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift){
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


void eor_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift){
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

void orr_shifted_register(int rd, int rn, int imm6, int rm, int n, int shift){
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
        case 0x0: cond = CURRENT_STATE.FLAG_Z;  // BEQ (Z == 1)
        case 0x1: cond = !CURRENT_STATE.FLAG_Z; // BNE (Z == 0)
        case 0xA: cond = !CURRENT_STATE.FLAG_N && !CURRENT_STATE.FLAG_Z; // BGT (N == 0 && Z == 0)
        case 0xB: cond = CURRENT_STATE.FLAG_N; // BLT (N == 1)
        case 0xC: cond = !CURRENT_STATE.FLAG_N; // BGE (N == 0)
        case 0xD: cond = CURRENT_STATE.FLAG_Z || CURRENT_STATE.FLAG_N; // BLE (Z == 1 || N == 1)
        default: return 0; 
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


void lsl_immediate(int rd, int rn, uint32_t imms, uint32_t immr, int n){
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

void lsr_immediate(int rd, int rn, uint32_t imms, uint32_t immr, int n){
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