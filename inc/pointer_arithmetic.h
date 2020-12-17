#ifndef POINTER_ARITHMETIC_H
#define POINTER_ARITHMETIC_H

#include <stdint.h>

uint32_t reverse_endianess(const uint32_t num);
void *p_add_offset(void* p, unsigned add);
void print_binary(uint32_t value);

#endif /* POINTER_ARITHMETIC_H */
