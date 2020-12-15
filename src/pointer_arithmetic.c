#include "pointer_arithmetic.h"

void *p_add_offset(void* p, unsigned add) {
    char* pointer = p;
    return pointer + add;
}
