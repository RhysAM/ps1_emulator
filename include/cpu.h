#include "memory_mapper.h"
#include "bios.h"
#include <stdbool.h>

#ifndef CPU_H
#define CPU_H

#define REGISTER_COUNT 32
#define COP_COUNT 4

typedef struct Instruction {
    uint32_t opcode;
    uint32_t secondary_opcode;
    uint32_t rs;
    uint32_t rt;
    uint32_t rd;
    uint32_t imm_5;
    uint32_t imm_26;
    uint32_t imm_16;
    uint32_t imm_16_se;
    uint32_t raw_instruction;
    uint32_t cop_no;
} Instruction;

typedef struct Coprocessor
{
    uint32_t registers[REGISTER_COUNT];
    bool halted;
} Coprocessor;

typedef struct CPU{
    bool halted;
    uint32_t PC;
    uint32_t registers[REGISTER_COUNT];
    uint32_t registers_copy[REGISTER_COUNT];
    MemoryMapper* memory_mapper;
    Instruction next_instruction;
    Coprocessor* COP0;
    uint32_t pending_load_val;
    uint32_t pending_load_reg;
    uint32_t status_register;
    uint32_t hi;
    uint32_t lo;
} CPU;

void initiate_cpu(CPU* cpu);
Instruction decode_bits(uint32_t raw_instruction);
void read_and_execute(CPU* cpu);
void handle_instruction(CPU* cpu, Instruction instr);
void print_registers(CPU* cpu);

#endif