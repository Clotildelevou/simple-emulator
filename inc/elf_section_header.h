#ifndef ELF_SECTION_HEADER_H
#define ELF_SECTION_HEADER_H

#include <elf.h>

void print_section_header(Elf32_Shdr* arr, unsigned size_elt, unsigned nb);

#endif /* ELF_SECTION_HEADER_H */
