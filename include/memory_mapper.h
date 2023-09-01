#include "bios.h"

typedef struct MemoryMapper{
    bios main_bios;
} MemoryMapper;

uint32_t load_word(MemoryMapper* mapper, uint32_t*destination, uint8_t offset);
void store_word();