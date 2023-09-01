#include "stdlib.h"

#ifndef BIOS_H
#define BIOS_H

// BIOS is 512 kb
#define BIOS_SIZE 512*32

extern const uint32_t BIOS_START;
extern const uint32_t BIOS_END;

typedef struct Bios{
    uint32_t data[BIOS_SIZE];
} Bios;

void initiate_bios(Bios* bios);

#endif