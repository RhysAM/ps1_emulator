#include "memory_mapper.h"
#include "bios.h"

#ifndef CPU_H
#define CPU_H

#define REGISTER_COUNT 32

typedef struct Instruction {
    uint32_t opcode;
    uint32_t secondary_opcode;
    uint32_t rs;
    uint32_t rt;
    uint32_t rd;
    uint32_t immediate_val;
    uint32_t imm_5;
    uint32_t imm_26;
    uint32_t raw_instruction;
} Instruction;

typedef struct CPU {
    uint32_t PC;
    uint32_t registers[REGISTER_COUNT];
    MemoryMapper memory_mapper;
    Instruction next_instruction;
} CPU;

void initiate_cpu(CPU* cpu);
Instruction decode_bits(uint32_t raw_instruction);
void read_and_execute(CPU* cpu);
void handle_instruction(CPU* cpu, Instruction instr);
void set_register(CPU* cpu, int register_number, uint32_t val);
void print_registers(CPU* cpu);

#endif