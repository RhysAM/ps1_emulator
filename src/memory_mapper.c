#include "memory_mapper.h"

const int HARDWARE_REGISTERS_START = 0x1f801000;
const int HARDWARE_REGISTERS_END = HARDWARE_REGISTERS_START + 8*0x1000;

uint8_t contains(uint32_t addr, uint32_t start, uint32_t end)
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
        exit(1);
    }
}

void store_word(MemoryMapper* mapper, uint32_t addr, uint32_t word)
{
    if (contains(addr, BIOS_START, BIOS_END))
    {
        printf("Ignoring write to BIOS ROM: %08x\n", addr);
        return;
    }
    else if ((addr == BIOS_ROM_DELAY ) || (addr == RAM_SIZE) || (addr == COM_DELAY))
    {
        printf("Write to ignored hardware register: %08x\n", addr);
        return;
    }
    else{
        printf("Tried to write %08x to unmapped address: %08x\n", word, addr);
        exit(1);
    }
}