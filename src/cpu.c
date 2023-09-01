#include "cpu.h"
#include "memory_mapper.h"
#include <string.h>

Instruction decode_bits(uint32_t raw_instruction)
{
    Instruction instr;
    instr.opcode = raw_instruction >> 26;
    instr.register_num = (raw_instruction >> 16) & 0x1f;
    instr.immediate_val = (raw_instruction & 0xffff);
    return instr;
}

void read_and_execute(CPU* cpu)
{
    uint32_t raw_instruction = load_word(&(cpu->memory_mapper), cpu->PC); 
    Instruction next_instr = decode_bits(raw_instruction); 
}

void initiate_cpu(CPU* cpu)
{
    cpu->PC = BIOS_START;
    memset(cpu->registers, 0, REGISTER_COUNT*sizeof((cpu->registers[0])));
}