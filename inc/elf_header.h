#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include <elf.h>

void print_machine_arch(const Elf32_Ehdr* header);
void print_elf_type(const Elf32_Ehdr* header);
void print_entry(const Elf32_Ehdr* header);

#endif /* ELF_HEADER_H */
