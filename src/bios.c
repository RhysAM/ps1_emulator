#include "bios.h"
#include "helper.h"

const uint32_t BIOS_START = 0xbfc00000;
const uint32_t BIOS_END = 0xbfc00000 + BIOS_SIZE;

void initiate_bios(Bios* bios)
{
    read_file("scph1001.bin", BIOS_SIZE, bios->data);
}
