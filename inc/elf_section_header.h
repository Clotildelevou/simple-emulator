#ifndef ELF_SECTION_HEADER_H
#define ELF_SECTION_HEADER_H

#include <elf.h>

void print_section_header(Elf32_Ehdr *header, Elf32_Shdr* arr);

#endif /* ELF_SECTION_HEADER_H */
