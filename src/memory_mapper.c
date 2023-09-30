#include "memory_mapper.h"

const uint32_t KUSEG_START = 0x000000000;
const uint32_t KUSEG_END = 0x7fffffff;
const uint32_t KSEG0_START = 0x80000000;
const uint32_t KSEG0_END = 0x9fffffff;
const uint32_t KSEG1_START = 0xa0000000;
const uint32_t KSEG1_END = 0xbfffffff;
const uint32_t KSEG2_START = 0xc0000000;
const uint32_t KSEG2_END = 0xffffffff;


const uint32_t MAIN_RAM_START = 0x00000000;
const uint32_t IRQ_CONTROL_REGISTER_START = 0x1f801070;
const uint32_t IRQ_CONTROL_REGISTER_END = 0x1f801078;

const uint32_t BIOS_START = 0x1fc00000;
const uint32_t BIOS_END = 0x1fc00000 + BIOS_SIZE;
const uint32_t HARDWARE_REGISTERS_START = 0x1f801000;
const uint32_t HARDWARE_REGISTERS_END = HARDWARE_REGISTERS_START + 8*0x1000;

const uint32_t EXPANSION_1_BASE_ADDRESS_REGISTER = 0x1F801000;
const uint32_t EXPANSION_2_BASE_ADDRESS_REGISTER = 0x1F801004;
const uint32_t MEMORY_CONTROL_REGISTERS_START = 0x1F801008;
const uint32_t MEMORY_CONTROL_REGISTERS_SIZE = 0x18;
const uint32_t CACHE_CONTROL_REGISTER = 0xfffe0130;
const uint32_t SOUND_REGISTERS_START = 0x1f801d80;
const uint32_t SOUND_REGISTERS_END = 0x1f801d80 + 0x60;

const uint32_t EXPANSION_1_START = 0x1F000000;
const uint32_t EXPANSION_1_END = 0x1F000100;
const uint32_t EXPANSION_2_START = 0x1F802000;
const uint32_t EXPANSION_2_END = 0x1F802042;

const uint32_t WEIRD_ADDR = 0x1fc0683c;

bool contains(uint32_t addr, uint32_t start, uint32_t end)
{
    return (addr >= start) && (addr <= end);
}

uint32_t get_physical_addr(uint32_t memory_mapped_addr)
{
    uint32_t physical_addr = 0; 
    if (contains(memory_mapped_addr, KUSEG_START, KUSEG_END))
    {
        physical_addr = memory_mapped_addr;
    }
    else if (contains(memory_mapped_addr, KSEG0_START, KSEG0_END))
    {
        physical_addr = memory_mapped_addr &  0x7fffffff;
    }
    else if (contains(memory_mapped_addr, KSEG1_START, KSEG1_END))
    {
        physical_addr = memory_mapped_addr & 0x1fffffff;   
    }
    else if (contains(memory_mapped_addr, KSEG2_START, KSEG2_END))
    {
        physical_addr = memory_mapped_addr;
    }
    if (physical_addr == WEIRD_ADDR)
    {
        printf("SOMETHING IS TRYING TO READ/WRITE TO BIOS ADDR\n.");
    }
    return physical_addr;
}

uint32_t load_word(MemoryMapper* mapper, uint32_t addr)
{
    addr = get_physical_addr(addr);
    if (addr % 4)
    {
        printf("Unaligned read %08x\n", addr);
        mapper->bad_state = true;
        return 0;
    }
    if (contains(addr, BIOS_START, BIOS_END))
    {
        uint32_t mem_offset = addr - BIOS_START; 
        uint32_t* start = (uint32_t*) (mapper->bios.data + mem_offset);
        return *start;
    }
    else if (contains(addr, MAIN_RAM_START, MAIN_RAM_START + MAIN_RAM_SIZE))
    {
        uint32_t offset = addr - MAIN_RAM_START;
        uint32_t* start = (uint32_t*) (mapper->ram->data + offset);
        printf("Reading from RAM addr %08x: %08x\n", addr, *start);
        return *start;
    }
    else{
        printf("Tried to fetch unmapped address: %08x\n", addr);
        mapper->bad_state = true;
        return 0;
    }
}

void store_word(MemoryMapper* mapper, uint32_t addr, uint32_t word, uint32_t status_register)
{
    if (status_register & 0x10000)
    {
        printf("Ignoring write while cache is isolated.\n");
        return;
    }
    addr = get_physical_addr(addr);
    if (addr % 4)
    {
        printf("Unaligned write %08x\n", addr);
        mapper->bad_state = true;
        return;
    }

    if (contains(addr, BIOS_START, BIOS_END))
    {
        printf("Ignoring write to BIOS ROM: %08x\n", addr);
    }
    else if ((addr == BIOS_ROM_DELAY ) || (addr == BIOS_RAM_SIZE_REGISTER) || (addr == COM_DELAY))
    {
        printf("Write to ignored hardware register: %08x\n", addr);
    }
    else if (addr == EXPANSION_1_BASE_ADDRESS_REGISTER)
    {
        printf("Setting expansion 1 base address to: %08x\n", word);
        mapper->expansion_1_base_adddress = word;
    }
    else if (addr == EXPANSION_2_BASE_ADDRESS_REGISTER)
    {
        printf("Setting expansion 2 base address to: %08x\n", word);
        mapper->expansion_2_base_adddress = word;
    }
    else if (addr == CACHE_CONTROL_REGISTER)
    {
        printf("Ignoring write to cache control registers.\n");
    }
    else if (contains(addr, MEMORY_CONTROL_REGISTERS_START, MEMORY_CONTROL_REGISTERS_START+MEMORY_CONTROL_REGISTERS_SIZE))
    {
        printf("Ignoring write to memory control register %08x\n", addr);
    }
    else if (contains(addr, IRQ_CONTROL_REGISTER_START, IRQ_CONTROL_REGISTER_END))
    {
        printf("Ignoring write to IRQ control registers.\n");
    }
    else if (contains(addr, MAIN_RAM_START, MAIN_RAM_START + MAIN_RAM_SIZE))
    {
        printf("Storing to RAM addr %08x: %08x\n", addr, word);
        uint32_t offset = addr - MAIN_RAM_START;
        *(uint32_t*) (mapper->ram->data + offset)  = word;
        return;
    }
    else if (contains(addr, HARDWARE_REGISTERS_START, HARDWARE_REGISTERS_END))
    {
        printf("Ignoring write hardware registers.\n");
    }
    else{
        printf("Tried to write %08x to unmapped address: %08x\n", word, addr);
        mapper->bad_state = true;
    }
}

void store_half_word(MemoryMapper* mapper, uint32_t addr, uint16_t half_word, uint32_t status_register)
{
    if (status_register & 0x10000)
    {
        printf("Ignoring write while cache is isolated.\n");
        return;
    }

    addr = get_physical_addr(addr);
    if (addr % 2)
    {
        printf("Unaligned halfword write %08x\n", addr);
        return;
    }
    if (contains(addr, BIOS_START, BIOS_END))
    {
        printf("Ignoring write to BIOS ROM: %08x\n", addr);
    }
    if (contains(addr, SOUND_REGISTERS_START, SOUND_REGISTERS_END))
    {
        printf("Ignoring write to sound registers.\n");
        return;
    }
    else
    {
        printf("Unhandled half word write to %08x.\n", addr);
        mapper->bad_state = true;
    }
}

void store_byte(MemoryMapper* mapper, uint32_t addr, uint8_t byte, uint32_t status_register)
{
    if (status_register & 0x10000)
    {
        printf("Ignoring write while cache is isolated.\n");
        return;
    }

    addr = get_physical_addr(addr);
    
    if (contains(addr, EXPANSION_2_START, EXPANSION_2_END))
    {
        printf("Ignoring write to expansion 2 space.\n");
        return;
    }
    if (contains(addr, BIOS_START, BIOS_END))
    {
        printf("Ignoring write to BIOS ROM: %08x\n", addr);
    }
    else if (contains(addr, MAIN_RAM_START, MAIN_RAM_START + MAIN_RAM_SIZE))
    {
        uint32_t offset = addr - MAIN_RAM_START;
        mapper->ram->data[offset] = byte;
        printf("Storing to RAM addr %08x: %08x\n", addr, byte);
    }
    else
    {
        printf("Unhandled byte write to %08x.\n", addr);
        mapper->bad_state = true;
    }
}

uint8_t load_byte(MemoryMapper* mapper, uint32_t addr)
{
    addr = get_physical_addr(addr);
    uint8_t byte;
    if (contains(addr, BIOS_START, BIOS_END))
    {
        uint32_t mem_offset = addr - BIOS_START; 
        byte = mapper->bios.data[mem_offset];
        printf("Loading from BIOS addr %08x: %08x\n", addr, byte);
        return byte;
    }
    else if (contains(addr, MAIN_RAM_START, MAIN_RAM_START + MAIN_RAM_SIZE))
    {
        uint32_t offset = addr - MAIN_RAM_START;
        byte =  mapper->ram->data[offset];
        printf("Loading from RAM addr %08x: %08x\n", addr, byte);
        return byte;
    }
    else if (contains(addr, EXPANSION_1_START, EXPANSION_1_END))
    {
        printf("Default read from expansion 1.\n");
        return 0xff;
    }
    else
    {
        printf("Unhandled byte load from %08x.\n", addr);
        mapper->bad_state = true;
        return 0;
    }
}