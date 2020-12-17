#include "pointer_arithmetic.h"

#include <stdio.h>

uint32_t reverse_endianess(const uint32_t num) {
    char* arr = (char*)&num;
    char reversed[4];
    reversed[0] = arr[3];
    reversed[1] = arr[2];
    reversed[2] = arr[1];
    reversed[3] = arr[0];
    uint32_t* res= (uint32_t*)reversed;
    return *res;
}

void print_binary(uint32_t value) {
    uint32_t reversed = reverse_endianess(value);
    char* arr = (char*)&reversed;
    char str[36];

    unsigned padding = 0;
    for (unsigned i = 0; i < 4; i++, padding += 9) {
        str[padding + 0] = arr[i] & 0x80 ? '1' : '0';
        str[padding + 1] = arr[i] & 0x40 ? '1' : '0';
        str[padding + 2] = arr[i] & 0x20 ? '1' : '0';
        str[padding + 3] = arr[i] & 0x10 ? '1' : '0';
        str[padding + 4] = arr[i] & 0x08 ? '1' : '0';
        str[padding + 5] = arr[i] & 0x04 ? '1' : '0';
        str[padding + 6] = arr[i] & 0x02 ? '1' : '0';
        str[padding + 7] = arr[i] & 0x01 ? '1' : '0';
        str[padding + 8] = ' ';
    }

    str[35] = '\0';
    puts(str);
}

void *p_add_offset(void* p, unsigned add) {
    char* pointer = p;
    return pointer + add;
}
