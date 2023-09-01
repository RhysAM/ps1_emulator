#include "memory_mapper.h"

const uint32_t PC_START = 0xbfc00000;
const int REGISTER_COUNT = 32;

typedef struct CPU{
    uint32_t PC = PC_START;
    uint32_t registers[REGISTER_COUNT];
    MemoryMapper memory_mapper;
}