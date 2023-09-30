#include "cpu.h"
#include "memory_mapper.h"
#include "helper.h"
#include <string.h>
#include <limits.h>

const int BCONDZ = 0x01;
const int J = 0x02;
const int JAL = 0x03;
const int BEQ = 0x04;
const int BNE = 0x05;
const int BLEZ = 0x06;
const int BGTZ = 0x07;
const int SLTI = 0x0a;
const int ANDI = 0x0c;
const int ORI = 0x0d;
const int LUI = 0x0f;
const int LB = 0x20;
const int LW = 0x23;
const int LBU = 0x24;
const int SB = 0x28;
const int SH = 0x29;
const int SW = 0x2b;
const int ADDI = 0x08;
const int ADDIU = 0x09;
const int COP0 = 0x10;

const int SPECIAL = 0x00;
const int SLL = 0x00;
const int SRA = 0x03;
const int JR = 0x08;
const int JALR = 0x09;
const int DIV = 0x1a;
const int ADD = 0x20;
const int ADDU = 0x21;
const int SUBU = 0x23;
const int AND = 0x24;
const int OR = 0x25;
const int SLTU = 0x2b;

// Coprocessor opcodes
const int MFC = 0x00;
const int MTC = 0x04;

const int STATUS_REGISTER = 12;
const int RA_REGISTER = 0x1F;

uint32_t COP0_STATUS_REGISTERS[6] = {3,5,6,7,9,11};

void set_register(CPU* cpu, int register_number, uint32_t val)
{
    if (register_number) // $0 register is never written to
    {
        cpu->registers_copy[register_number] = val;
    }
}

uint32_t get_register(CPU* cpu, int register_number)
{
    return cpu->registers[register_number];
}

void set_cop_register(CPU* cpu, Coprocessor* cop, int register_number, uint32_t val)
{
    if (register_number) // $0 register is never written to
    {
        if ((val) && is_in(register_number, COP0_STATUS_REGISTERS, sizeof(COP0_STATUS_REGISTERS) / sizeof(COP0_STATUS_REGISTERS[0])))
        {  
            printf("Writing non-zero value to COP0 status registers.\n");
            cop->halted = true;
        }
        if (register_number == STATUS_REGISTER)
        {
            cpu->status_register = val;
        }
        cop->registers[register_number] = val;
    }
}

uint32_t get_cop_register(Coprocessor* cop, int register_number)
{
    return cop->registers[register_number];
}

void print_registers(CPU* cpu)
{
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
    instr.secondary_opcode = raw_instruction & 0x3f;
    instr.rd = (raw_instruction >> 11) & 0x1f;
    instr.rt = (raw_instruction >> 16) & 0x1f;
    instr.rs = (raw_instruction >> 21) & 0x1f;
    instr.imm_5 = (raw_instruction >> 6) & 0x1f;
    instr.imm_26 = raw_instruction & 0x3ffffff;
    instr.imm_16 = (raw_instruction & 0xffff);
    instr.imm_16_se = (uint32_t) (int16_t) (raw_instruction & 0xFFFF);
    instr.cop_no = raw_instruction & (0x03);
    instr.raw_instruction = raw_instruction;
    return instr;
}

void read_and_execute(CPU* cpu)
{
    if (cpu->halted)
    {
        return;
    }
    Instruction current_instr = cpu->next_instruction;
    uint32_t raw_instruction = load_word((cpu->memory_mapper), cpu->PC); 
    Instruction next_instr = decode_bits(raw_instruction); 
    printf("PC: %08x\n", cpu->PC);
    printf("Instruction hex: %08x\n", current_instr.raw_instruction);
    set_register(cpu, cpu->pending_load_reg, cpu->pending_load_val);
    printf("Pending load %i %08x\n", cpu->pending_load_reg, cpu->pending_load_val);
    cpu->pending_load_reg = 0;
    cpu->pending_load_val = 0;
    cpu->next_instruction = next_instr;
    cpu->PC += 4;
    // Copy register copy -> registers to account for load delay
    memcpy(cpu->registers, cpu->registers_copy, sizeof(cpu->registers[0]) * REGISTER_COUNT);
    handle_instruction(cpu, current_instr);
    print_registers(cpu);
}

void initiate_cpu(CPU* cpu)
{
    cpu->PC = BIOS_START;
    memset(cpu->registers, 0xbabebabe, REGISTER_COUNT*sizeof((cpu->registers[0])));
    memset(cpu->registers_copy, 0xbabebabe, REGISTER_COUNT*sizeof((cpu->registers[0])));
    memset(cpu->COP0->registers, 0x0, REGISTER_COUNT*sizeof((cpu->COP0->registers[0])));
    cpu->registers[0] = 0;
    cpu->registers_copy[0] = 0;
    cpu->COP0->registers[0] = 0;
    cpu->status_register = 0;
    cpu->hi = 0;
    cpu->lo = 0;
    cpu->next_instruction = decode_bits(0x00000000);
    cpu->halted = false;
}

void do_branch(CPU* cpu, uint32_t branch_amt)
{
    branch_amt <<= 2;
    cpu->PC -= 4;
    cpu->PC += branch_amt;
}

void handle_instruction(CPU* cpu, Instruction instr)
{
    int32_t signed_va, numerator, denominator;
    uint32_t val, addr;
    uint16_t half_word;
    uint8_t byte;
    bool gez, link;
    switch (instr.opcode)
    {
        case COP0:
            switch (instr.rs) // bits 26-21 in COP mode are 
            {
                case MTC:
                    printf("MTC%d: $%i,$%i\n", instr.cop_no, instr.rt, instr.rd);
                    if (instr.cop_no)
                    {
                        printf("Move to nonzero coprocessor register.\n");
                        cpu->halted = true;
                        break;
                    }
                    set_cop_register(cpu, cpu->COP0, instr.rd, get_register(cpu, instr.rt));
                    break;
                case MFC:
                    printf("MFC%d: $%i,$%i\n", instr.cop_no, instr.rt, instr.rd);
                    if (instr.cop_no)
                    {
                        printf("Move from nonzero coprocessor register.\n");
                        cpu->halted = true;
                        break;
                    }
                    val = get_cop_register(cpu->COP0, instr.rd);
                    cpu->pending_load_reg = instr.rt;
                    cpu->pending_load_val = val;
                    set_register(cpu, instr.rt, val);
                    break;                   
                default:
                    printf("Unhandled coprocessor instruction: %08x. Opcode: %02x\n", instr.raw_instruction, instr.rs);
                    cpu->PC -= 4; 
                    cpu->halted = true;
                    break;
            }
            break;
        case BEQ:
            printf("BEQ $%i, $%i, (%i)\n", instr.rs, instr.rt, (instr.imm_16_se) << 2);
            if (get_register(cpu, instr.rt) == get_register(cpu, instr.rs))
            {
                do_branch(cpu, instr.imm_16_se);
            }
            break;
        case BNE:
            printf("BNE $%i, $%i, (%i)\n", instr.rs, instr.rt, (instr.imm_16_se) << 2);
            if (get_register(cpu, instr.rt) != get_register(cpu, instr.rs))
            {
                do_branch(cpu, instr.imm_16_se);
            }
            break;
        case BGTZ:
            printf("BGTZ $%i (%i)\n", instr.rs, (instr.imm_16_se) << 2);
            signed_val = (int32_t) get_register(cpu, instr.rs);
            if (signed_val > 0)
            {
                do_branch(cpu, instr.imm_16_se);
            }
            break;
        case BLEZ:
            printf("BLEZ $%i (%i)\n", instr.rs, (instr.imm_16_se) << 2);
            signed_val = (int32_t) get_register(cpu, instr.rs);
            if (signed_val <= 0)
            {
                do_branch(cpu, instr.imm_16_se);
            }
            break;
        case BCONDZ:
            gez = (instr.raw_instruction >> 16) & 1;
            link = (instr.raw_instruction >> 20) & 1;
            signed_val = (int32_t) get_register(cpu, instr.rs);
            if (gez)
            {
                if (link)
                {
                    printf("bgezal $%i %i\n", instr.rs, (instr.imm_16_se) << 2);
                }
                else
                {
                    printf("bgez $%i %i\n", instr.rs, (instr.imm_16_se) << 2);
                }
            }
            else
            {
                if (link)
                {
                    printf("bltzal $%i %i\n", instr.rs, (instr.imm_16_se) << 2);
                }
                else
                {
                    printf("bltz $%i %i\n", instr.rs, (instr.imm_16_se) << 2);
                }
            }
            if (gez)
            {
                if (signed_val >= 0)
                {
                    if (link)
                    {
                        set_register(cpu, RA_REGISTER, cpu->PC);
                    }
                    do_branch(cpu, instr.imm_16_se);
                }
            }
            else
            {
                if (signed_val < 0)
                {
                    if (link)
                    {
                        set_register(cpu, RA_REGISTER, cpu->PC);
                    }
                    do_branch(cpu, instr.imm_16_se);
                }
            }
            break;
        case SLTI:
            printf("slti $%i, $%i, %i\n", instr.rt, instr.rs, instr.imm_16_se);
            val = (int32_t) get_register(cpu, instr.rs) < (int32_t) instr.imm_16_se;
            set_register(cpu, instr.rt, val);
            break;
        case SPECIAL:
            switch(instr.secondary_opcode)
            {
                case SLL:
                    printf("sll $%i $%i %i\n", instr.rt, instr.rd, instr.imm_5);
                    val = get_register(cpu, instr.rt);
                    val <<= instr.imm_5;
                    set_register(cpu, instr.rd, val);
                    break;
                case SRA:
                    printf("sra $%i $%i %i\n", instr.rt, instr.rd, instr.imm_5);
                    signed_val = (int32_t) get_register(cpu, instr.rt);
                    signed_val >>= instr.imm_5;
                    set_register(cpu, instr.rd, signed_val);
                    break;
                case OR:
                    printf("or $%i $%i $%i\n", instr.rd, instr.rs, instr.rt);
                    val = get_register(cpu, instr.rs) | get_register(cpu, instr.rt);
                    set_register(cpu, instr.rd, val);
                    break;
                case SLTU:
                    printf("sltu $%i $%i $%i\n", instr.rd, instr.rs, instr.rt);
                    val = get_register(cpu, instr.rs) < get_register(cpu, instr.rt);
                    set_register(cpu, instr.rd, val);
                    break;
                case DIV:
                    printf("div $%i $%i\n", instr.rs, instr.rt);
                    numerator = (int32_t) get_register(cpu, instr.rs);
                    denominator = (int32_t) get_register(cpu, instr.rs);

                    cpu->halted = true;
                    break;
                case ADD:
                    printf("add $%i $%i $%i\n", instr.rd, instr.rs, instr.rt);
                    int32_t rs = get_register(cpu, instr.rs);
                    int32_t rt = get_register(cpu, instr.rt);
                    // Check for overflow
                    if ((rt > 0 && rs > INT_MAX - rt) ||
                        (rt < 0 && rs < INT_MIN - rt))
                    {
                        printf("OVERFLOW IN ADD.\n");
                        cpu->halted = true;
                    }
                    uint32_t result = rt + rs;
                    set_register(cpu, instr.rd, result);
                    break;
                case ADDU:
                    printf("addu $%i $%i $%i\n", instr.rd, instr.rs, instr.rt);
                    val = get_register(cpu, instr.rs) + get_register(cpu, instr.rt);
                    set_register(cpu, instr.rd, val);
                    break;
                case SUBU:
                    printf("subu $%i $%i $%i\n", instr.rd, instr.rs, instr.rt);
                    val = get_register(cpu, instr.rs) - get_register(cpu, instr.rt);
                    set_register(cpu, instr.rd, val);
                    break;
                case JR:
                    printf("jr $%i\n", instr.rs);
                    cpu->PC = get_register(cpu, instr.rs);
                    break;
                case JALR:
                    printf("jalr $%i $%i\n", instr.rd, instr.rs);
                    set_register(cpu, instr.rd, cpu->PC);
                    cpu->PC = get_register(cpu, instr.rs);
                    break;
                case AND:
                    printf("and $%i $%i $%i\n", instr.rd, instr.rs, instr.rt);
                    val = get_register(cpu, instr.rs) & get_register(cpu, instr.rt);
                    set_register(cpu, instr.rd, val);
                    break;
                default:
                    printf("Unhandled special opcode %02x, raw %08x.\n", instr.secondary_opcode, instr.raw_instruction);
                    cpu->halted = true;
                    break;
            }
            break;
        case LUI:
            printf("lui $%i, 0x%02x\n", instr.rt, instr.imm_16);
            set_register(cpu, instr.rt, (instr.imm_16 << 16));;
            break;
        case ORI:
            printf("ori $%i, $%i, 0x%02x\n", instr.rt, instr.rs, instr.imm_16);
            val = get_register(cpu, instr.rs);
            val |= instr.imm_16;
            set_register(cpu, instr.rt, val);;
            break;
        case ANDI:
            printf("andi $%i, $%i, 0x%02x\n", instr.rt, instr.rs, instr.imm_16);
            val = get_register(cpu, instr.rs);
            val &= instr.imm_16;
            set_register(cpu, instr.rt, val);;
            break;
        case SW:
            printf("sw $%i, %d($%i)\n", instr.rt, (int16_t) instr.imm_16_se, instr.rs);
            addr = get_register(cpu, instr.rs) + instr.imm_16_se;
            store_word(cpu->memory_mapper, addr, get_register(cpu, instr.rt), cpu->status_register);
            break;
        case LW:
            printf("lw $%i, %d($%i)\n", instr.rt, (int16_t) instr.imm_16_se, instr.rs);
            addr = get_register(cpu, instr.rs) + instr.imm_16_se;
            val = load_word(cpu->memory_mapper, addr);
            cpu->pending_load_reg = instr.rt;
            cpu->pending_load_val = val;
            set_register(cpu, instr.rt, val);
            break;
        case ADDI:
            printf("addi $%i, $%i, %d\n", instr.rt, instr.rs, (int16_t) instr.imm_16_se);
            int32_t rs = get_register(cpu, instr.rs);
            int32_t immediate_val = instr.imm_16_se;
            // Check for overflow
            if ((immediate_val > 0 && rs > INT_MAX - immediate_val) ||
                (immediate_val < 0 && rs < INT_MIN - immediate_val))
            {
                printf("OVERFLOW IN ADDI.\n");
                cpu->halted = true;
            }
            uint32_t result = immediate_val + rs;
            set_register(cpu, instr.rt, result);
            break;
        case ADDIU:
            printf("addiu $%i, $%i, 0x%02x\n", instr.rt, instr.rs, instr.imm_16_se);
            set_register(cpu, instr.rt, get_register(cpu, instr.rs) + instr.imm_16_se);
            break;
        case J:
            printf("J 0x%04x\n", instr.imm_26 << 2);
            cpu->PC = (cpu->PC & 0xf0000000) + (instr.imm_26 << 2);
            break;
        case JAL:
            printf("JAL 0x%04x\n", instr.imm_26 << 2);
            set_register(cpu, RA_REGISTER, cpu->PC);
            cpu->PC = (cpu->PC & 0xf0000000) + (instr.imm_26 << 2);
            break;
        case SH:
            printf("sh $%i, %d($%i)\n", instr.rt, (int32_t) instr.imm_16_se, instr.rs);
            addr = get_register(cpu, instr.rs) + instr.imm_16_se;
            half_word = (uint16_t) get_register(cpu, instr.rt);
            store_half_word(cpu->memory_mapper, addr, half_word, cpu->status_register);
            break;
        case SB:
            printf("sb $%i, %d($%i)\n", instr.rt, (int32_t) instr.imm_16_se, instr.rs);
            addr = get_register(cpu, instr.rs) + instr.imm_16_se;
            byte = (uint8_t) get_register(cpu, instr.rt);
            store_byte(cpu->memory_mapper, addr, byte, cpu->status_register);
            break;
        case LB:
            printf("lb $%i, %d($%i)\n", instr.rt, (int32_t) instr.imm_16_se, instr.rs);
            addr = get_register(cpu, instr.rs) + instr.imm_16_se;
            byte = load_byte(cpu->memory_mapper, addr);
            cpu->pending_load_reg = instr.rt;
            cpu->pending_load_val = byte;
            set_register(cpu, instr.rt, (int8_t) byte);
            break;
        case LBU:
            printf("lbu $%i, %d($%i)\n", instr.rt, (int32_t) instr.imm_16_se, instr.rs);
            addr = get_register(cpu, instr.rs) + instr.imm_16_se;
            byte = load_byte(cpu->memory_mapper, addr);
            cpu->pending_load_reg = instr.rt;
            cpu->pending_load_val = byte;
            set_register(cpu, instr.rt, byte);
            break;
        default:
            printf("Unhandled instruction: %08x. Opcode: %02x\n", instr.raw_instruction, instr.opcode);
            cpu->PC -= 4; 
            cpu->halted = true;
    }
}