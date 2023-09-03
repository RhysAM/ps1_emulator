#include "cpu.h"
#include "memory_mapper.h"
#include <string.h>

const int J = 0x02;
const int ORI = 0x0d;
const int LUI = 0x0f;
const int SW = 0x2b;
const int ADDIU = 0x09;

const int SPECIAL = 0x00;
const int SLL = 0x00;

void print_registers(CPU* cpu)
{
    printf("PC: %08x\n", cpu->PC);
    int counter = 0;
    for (int i = 0; i < REGISTER_COUNT; i++)
    {
        printf("$%02d: %08x ", i, cpu->registers[i]);
        counter++;
        if (counter % 4 == 0)
        {
            printf("\n");
        }
    }
}

Instruction decode_bits(uint32_t raw_instruction)
{
    Instruction instr;
    instr.opcode = raw_instruction >> 26;
    instr.secondary_opcode = raw_instruction & 0x1f;
    instr.rd = (raw_instruction >> 11) & 0x1f;
    instr.rt = (raw_instruction >> 16) & 0x1f;
    instr.rs = (raw_instruction >> 21) & 0x1f;
    instr.immediate_val = (raw_instruction & 0xffff);
    instr.imm_5 = (raw_instruction >> 6) & 0x1f;
    instr.imm_26 = raw_instruction & 0x3ffffff;
    instr.raw_instruction = raw_instruction;
    return instr;
}

void read_and_execute(CPU* cpu)
{
    Instruction current_instr = cpu->next_instruction;
    uint32_t raw_instruction = load_word(&(cpu->memory_mapper), cpu->PC); 
    Instruction next_instr = decode_bits(raw_instruction); 
    cpu->next_instruction = next_instr;
    cpu->PC += 4;
    handle_instruction(cpu, current_instr);
    print_registers(cpu);
}

void initiate_cpu(CPU* cpu)
{
    cpu->PC = BIOS_START;
    memset(cpu->registers, 0xbabebabe, REGISTER_COUNT*sizeof((cpu->registers[0])));
    cpu->registers[0] = 0;
    cpu->next_instruction = decode_bits(0);
}

void set_register(CPU* cpu, int register_number, uint32_t val)
{
    if (register_number) // $0 register is never written to
    {
        cpu->registers[register_number] = val;
    }
}

uint32_t get_register(CPU* cpu, int register_number)
{
    return cpu->registers[register_number];
}

void handle_instruction(CPU* cpu, Instruction instr)
{
    switch (instr.opcode)
    {
        case SPECIAL:
            switch(instr.secondary_opcode)
            {
                case SLL:
                    printf("sll $%i $%i %02x\n", instr.rt, instr.rd, instr.imm_5);
                    uint32_t val = get_register(cpu, instr.rt);
                    val <<= instr.imm_5;
                    set_register(cpu, instr.rd, val);
                    break;
            }
        case LUI:
            printf("lui $%i, 0x%02x\n", instr.rt, instr.immediate_val);
            set_register(cpu, instr.rt, (instr.immediate_val << 16));
            break;
        case ORI:
            printf("ori $%i, $%i, 0x%02x\n", instr.rt, instr.rs, instr.immediate_val);
            uint32_t val = get_register(cpu, instr.rs);
            val |= instr.immediate_val;
            set_register(cpu, instr.rt, val);
            break;
        case SW:
            printf("sw $%i, %02x($%i)\n", instr.rt, instr.immediate_val, instr.rs);
            uint32_t addr = instr.immediate_val + (int) get_register(cpu, instr.rs);
            store_word(&cpu->memory_mapper, addr, get_register(cpu, instr.rt));
            break;
        case ADDIU:
            printf("addiu $%i, $%i, 0x%02x\n", instr.rt, instr.rs, instr.immediate_val);
            set_register(cpu, instr.rt, get_register(cpu, instr.rs) + instr.immediate_val);
            break;
        case J:
            printf("J 0x%04x\n", instr.imm_26 << 2);
            cpu->PC = (cpu->PC & 0xf0000000) + (instr.imm_26 << 2);
            break;
        default:
            printf("Unhandled instruction: %08x. Opcode: %02x\n", instr.raw_instruction, instr.opcode);
            exit(1);
    }
}