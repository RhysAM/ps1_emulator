#include "bios.h"
#include "helper.h"

void initiate_bios(Bios* bios)
{
    read_file("scph1001.bin", BIOS_SIZE, bios->data);
}
