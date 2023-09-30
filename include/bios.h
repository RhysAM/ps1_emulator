#include "stdlib.h"

#ifndef BIOS_H
#define BIOS_H

// BIOS is 512 kb
#define BIOS_SIZE 512 * 0x1000
#define BIOS_ROM_DELAY 0x1F801010
#define BIOS_RAM_SIZE_REGISTER 0x1F801060
#define COM_DELAY 0x1f801020

const uint32_t extern BIOS_START;

typedef struct Bios{
    uint8_t data[BIOS_SIZE];
} Bios;

void initiate_bios(Bios* bios);

#endif