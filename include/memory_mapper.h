#include "bios.h"
#include "stdlib.h"
#include "stdio.h"
#include <stdbool.h>

#ifndef MEMORYMAPPER_H
#define MEMORYMAPPER_H

typedef struct MemoryMapper{
    Bios bios;
    bool bad_state;
    uint32_t expansion_1_base_adddress;
    uint32_t expansion_2_base_adddress;
} MemoryMapper;

bool contains(uint32_t addr, uint32_t start, uint32_t end);
uint32_t load_word(MemoryMapper* mapper, uint32_t addr);
void store_word(MemoryMapper* mapper, uint32_t addr, uint32_t word);

#endif