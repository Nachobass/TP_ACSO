#include <stdint.h>
#include "shell.h"  // Incluye la definición de CPU_State y funciones auxiliares

void execute_ands(uint32_t instruction) {
    uint8_t rd = (instruction >> 0) & 0x1F;  // Extraer Rd (registro destino)
    uint8_t rn = (instruction >> 5) & 0x1F;  // Extraer Rn (primer operando)
    uint8_t rm = (instruction >> 16) & 0x1F; // Extraer Rm (segundo operando)
    
    uint64_t operand1 = STATE_CURRENT.REGS[rn];
    uint64_t operand2 = STATE_CURRENT.REGS[rm];
    uint64_t result = operand1 & operand2;
    
    // Guardar el resultado en el registro destino
    STATE_NEXT.REGS[rd] = result;
    
    // Actualizar las banderas N y Z
    STATE_NEXT.FLAG_N = (result >> 63) & 1; // Si el bit más significativo es 1, N = 1
    STATE_NEXT.FLAG_Z = (result == 0) ? 1 : 0; // Si el resultado es 0, Z = 1
    
    // Avanzar el PC
    STATE_NEXT.PC = STATE_CURRENT.PC + 4;
}
