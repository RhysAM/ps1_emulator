#include "memory_mapper.h"

const int HARDWARE_REGISTERS_START = 0x1f801000;
const int HARDWARE_REGISTERS_END = HARDWARE_REGISTERS_START + 8*0x1000;
const int EXPANSION_1_BASE_ADDRESS_REGISTER = 0x1F801000;
const int EXPANSION_2_BASE_ADDRESS_REGISTER = 0x1F801004;
const int MEMORY_CONTROL_REGISTERS_START = 0x1F801008;
const int MEMORY_CONTROL_REGISTERS_SIZE = 0x18;
const int CACHE_CONTROL_REGISTER = 0xfffe0130;

bool contains(uint32_t addr, uint32_t start, uint32_t end)
{
    return (addr >= start) && (addr <= end);
}

uint32_t load_word(MemoryMapper* mapper, uint32_t addr)
{
    if (contains(addr, BIOS_START, BIOS_END))
    {
        uint32_t mem_offset = addr - BIOS_START; 
        uint32_t* start = (uint32_t*) (mapper->bios.data + mem_offset);
        return *start;
    }
    else{
        printf("Tried to fetch unmapped address: %08x\n", addr);
        mapper->bad_state = true;
        return 0;
    }
}

void store_word(MemoryMapper* mapper, uint32_t addr, uint32_t word)
{
    if (contains(addr, BIOS_START, BIOS_END))
    {
        printf("Ignoring write to BIOS ROM: %08x\n", addr);
    }
    else if ((addr == BIOS_ROM_DELAY ) || (addr == RAM_SIZE) || (addr == COM_DELAY))
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
    else{
        printf("Tried to write %08x to unmapped address: %08x\n", word, addr);
        mapper->bad_state = true;
    }
}