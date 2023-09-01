#include "stdio.h"
#include "helper.h"

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