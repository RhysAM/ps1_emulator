#include "memory_mapper.h"

uint8_t contains(uint32_t addr, uint32_t start, uint32_t end)
{
    return (addr >= start) && (addr <= end);
}

uint32_t load_word(MemoryMapper* mapper, uint32_t destination)
{
    if (contains(destination, BIOS_START, BIOS_END))
    {
        uint32_t mem_offset = destination - BIOS_START; 
        return mapper->bios.data[mem_offset];
    }
    else{
        printf("Tried to fetch unmapped address: %08x", destination);
        exit(1);
    }
}
