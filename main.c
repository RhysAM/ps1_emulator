#include "stdlib.h"
#include "stdio.h"
#include "bios.h"

void dump_array(uint32_t* arr, int file_size)
{
    int counter = 0;
    for (int i = 0; i < (file_size / sizeof(arr[0])); i++) {
        printf("%08x ", arr[i]);
        counter++;
        if (counter % 4 == 0)
        {
            printf("\n");
        }
    }
}

void read_file(const char* filename, int file_size, uint8_t* destination){
    FILE* f = fopen(filename, "rb");
    if (f == NULL){
        exit(1);
    }
    fread(destination, 1, file_size, f); 
    if (ferror(f))
    {
        exit(1);
    }
    fclose(f);
}


int main(int argc, char** argv)
{
    bios ps1_bios;
    read_file("scph1001.bin", BIOS_SIZE, ps1_bios.bios_memory);
    dump_array((uint32_t*) ps1_bios.bios_memory, BIOS_SIZE);
}