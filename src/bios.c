#include "bios.h"
#include "helper.h"

const uint32_t BIOS_START = 0xbfc00000;
const uint32_t BIOS_END = 0xbfc00000 + BIOS_SIZE * 0x1000;

void initiate_bios(Bios* bios)
{
    read_file("scph1001.bin", BIOS_SIZE, (uint8_t*) bios->data);
}