#include "memory_mapper.h"
#include "bios.h"

#ifndef CPU_H
#define CPU_H

#define REGISTER_COUNT 32

typedef struct CPU {
    uint32_t PC;
    uint32_t registers[REGISTER_COUNT];
    MemoryMapper memory_mapper;
} CPU;

typedef struct Instruction {
    uint32_t opcode;
    uint32_t register_num;
    uint32_t immediate_val;
} Instruction;

void initiate_cpu(CPU* cpu);
Instruction decode_bits(uint32_t raw_instruction);
void read_and_execute(CPU* cpu);

#endif