#include "bios.h"
#include "stdlib.h"
#include "stdio.h"

#ifndef MEMORYMAPPER_H
#define MEMORYMAPPER_H

typedef struct MemoryMapper{
    Bios bios;
} MemoryMapper;

uint8_t contains(uint32_t addr, uint32_t start, uint32_t end);
uint32_t load_word(MemoryMapper* mapper, uint32_t addr);
void store_word(MemoryMapper* mapper, uint32_t addr, uint32_t word);

#endif