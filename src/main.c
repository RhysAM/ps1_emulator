#include "stdlib.h"
#include "bios.h"
#include "cpu.h"
#include "helper.h"

int main(int argc, char** argv)
{
    Bios bios;
    initiate_bios(&bios);
    MemoryMapper memory_mapper = {.bios = bios};
    CPU cpu = {.registers = {0}, .memory_mapper = memory_mapper}; 
    initiate_cpu(&cpu);
    // dump_array(&bios, BIOS_SIZE);
    while (1)
    {
        read_and_execute(&cpu);
    }
}