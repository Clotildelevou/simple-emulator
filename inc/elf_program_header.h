#ifndef ELF_PROGRAM_HEADER_H
#define ELF_PROGRAM_HEADER_H

#include <elf.h>

void print_program_header(Elf32_Phdr* arr, unsigned size_elt, unsigned nb);

#endif /* ELF_PROGRAM_HEADER_H */
