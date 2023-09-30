#include "stdlib.h"
#include "stdbool.h"

#ifndef HELPER_H
#define HELPER_H

void dump_array(uint32_t* arr, int file_size);
void read_file(const char* filename, int file_size, uint8_t* destination);
bool is_in(uint32_t val, uint32_t* arr, uint32_t arr_size);

#endif