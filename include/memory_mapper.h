#include "bios.h"
#include "stdlib.h"
#include "stdio.h"
#include <stdbool.h>

#ifndef MEMORYMAPPER_H
#define MEMORYMAPPER_H

#define MAIN_RAM_SIZE 0x200000

typedef struct RAM{
    uint8_t data[MAIN_RAM_SIZE];
} RAM;

typedef struct MemoryMapper{
    Bios bios;
    RAM* ram;
    bool bad_state;
    uint32_t expansion_1_base_adddress;
    uint32_t expansion_2_base_adddress;
} MemoryMapper;

bool contains(uint32_t addr, uint32_t start, uint32_t end);
uint32_t load_word(MemoryMapper* mapper, uint32_t addr);
void store_word(MemoryMapper* mapper, uint32_t addr, uint32_t word, uint32_t status_register);
void store_half_word(MemoryMapper* mapper, uint32_t addr, uint16_t half_word, uint32_t status_register);
void store_byte(MemoryMapper* mapper, uint32_t addr, uint8_t byte, uint32_t status_register);
uint8_t load_byte(MemoryMapper* mapper, uint32_t addr);

#endif