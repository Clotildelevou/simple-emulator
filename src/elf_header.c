#include <stdio.h>
#include <elf.h>

#include "elf_header.h"

void print_machine_arch(const Elf32_Ehdr *header) {
    if (!header)
        return;
    switch (header->e_machine) {
        case EM_NONE:
            printf("An unknown machine\n");
            break;
        case EM_M32:
            printf("AT&T WE 32100\n");
            break;
        case EM_SPARC:
            printf("Sun Microsystems SPARC\n");
            break;
        case EM_386:
            printf("Intel 80386\n");
            break;
        case EM_68K:
            printf("Motorola 68000\n");
            break;
        case EM_88K:
            printf("Motorola 88000\n");
            break;
        case EM_860:
            printf("Intel 80860\n");
            break;
        case EM_MIPS:
            printf("MIPS RS3000 (big-endian only)\n");
            break;
        case EM_PARISC:
            printf("HP/PA\n");
            break;
        case EM_SPARC32PLUS:
            printf("SPARC with enhanced instruction set\n");
            break;
        case EM_PPC:
            printf("PowerPC\n");
            break;
        case EM_PPC64:
            printf("PowerPC 64-bit\n");
            break;
        case EM_S390:
            printf("IBM S/390\n");
            break;
        case EM_ARM:
            printf("Advanced RISC Machines\n");
            break;
        case EM_SH:
            printf("Renesas SuperH\n");
            break;
        case EM_SPARCV9:
            printf("SPARC v9 64-bit\n");
            break;
        case EM_IA_64:
            printf("Intel Itanium\n");
            break;
        case EM_X86_64:
            printf("AMD x86-64\n");
            break;
        case EM_VAX:
            printf("DEC Vax\n");
            break;
        default:
            fprintf(stderr, "Switch fall through\n");
    }
}
void print_elf_type(const Elf32_Ehdr *header) {
    if (!header)
        return;
    switch(header->e_type) {
        case ET_NONE:
            printf("An unknown type.\n");
            break;
        case ET_REL:
            printf("A relocatable file.\n");
            break;
        case ET_EXEC:
            printf("An executable file.\n");
            break;
        case ET_DYN:
            printf("A shared object.\n");
            break;
        case ET_CORE:
            printf("A core file.\n");
            break;
        default:
            fprintf(stderr, "Switch fall through\n");
    }
}
void print_entry(const Elf32_Ehdr *header) {
    if (!header)
        return;

    if (header->e_entry == 0)
        printf("No entry point\n");
    else {
        printf("Entry point is at %x, %u\n", header->e_entry, header->e_entry);
    }
}
