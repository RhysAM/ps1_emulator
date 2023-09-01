const int BIOS_SIZE = 512; // BIOS is 512 kb

typedef struct Bios{
    uint32_t data[BIOS_SIZE];
} bios;

