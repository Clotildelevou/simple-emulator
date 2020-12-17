#include "elf_section_header.h"
#include "pointer_arithmetic.h"
#include "parser.h"

#include <string.h>
#include <elf.h>
#include <stdio.h>

void print_sh_type(Elf32_Shdr *elt) {
    switch (elt->sh_type) {

        case SHT_NULL:
            printf("Null section header\n");
            break;
        case SHT_PROGBITS:
            printf("Progbits section header\n");
            break;
        case SHT_SYMTAB:
            printf("Symtab section header (Symbol table)\n");
            break;
        case SHT_STRTAB:
            printf("Strtab section header\n");
            break;
        case SHT_RELA:
            printf("rela section header (relocation entries)\n");
            break;
        case SHT_HASH:
            printf("hashtable section header\n");
            break;
        case SHT_DYNAMIC:
            printf("dynamic linking section header\n");
            break;
        case SHT_NOTE:
            printf("notes section header\n");
            break;
        case SHT_NOBITS:
            printf("Nobits section header (empty)\n");
            break;
        case SHT_REL:
            printf("rel section header (relocation without addends)\n");
            break;
        case SHT_SHLIB:
            printf("shlib section header (reserved but useless)\n");
            break;

        case SHT_DYNSYM:
            printf("dynsym section header (minimal set of dyn linking "
                   "ssymbols)\n");
            break;
        case SHT_LOUSER:
            printf("louser section header(lower bound indices usable by "
                   "application)\n");
            break;
        case SHT_HIUSER:
            printf("hiuser section header(upper bound indices usable by "
                   "application)\n");
            break;
        default:
            printf("no category found\n");
            /* SHT_LOPROC, SHT_HIPROC */
            /*        Values in the inclusive range [SHT_LOPROC, SHT_HIPROC] are
             * reserved for  proces‐ */
            /*        sor-specific semantics. */
    }
}

void print_sh_flag(Elf32_Shdr *elt) {
    printf("section header is ");
    if (elt->sh_flags & SHF_WRITE)
        printf(" writable");

    if (elt->sh_flags & SHF_ALLOC)
        printf(" allocated at execution");

    if (elt->sh_flags & SHF_EXECINSTR)
        printf(" executable");

    if (elt->sh_flags & SHF_MASKPROC)
        printf(" processor specific");
    printf("\n");
}


void print_section_header(Elf32_Ehdr *header, Elf32_Shdr *arr) {
    printf("-----------------------section headers-----------------------\n");

    
    // getting the strtab for printing section names
    Elf32_Shdr* strtab =  p_add_offset(arr, header->e_shentsize * header->e_shstrndx);
    char* strings =  p_add_offset(header, strtab->sh_offset);


    unsigned shdr_size = header->e_shentsize;
    unsigned shdr_nb   = header->e_shnum;
    for (unsigned k = 0; k < shdr_nb; k++) {
        printf("\n");
        Elf32_Shdr *elt = p_add_offset(arr, k * shdr_size);
        printf("name %s\n", strings + elt->sh_name);
        print_sh_type(elt);
        print_sh_flag(elt);
        if (!strcmp(".text", strings + elt->sh_name)) {
            uint32_t *inst = p_add_offset(header, elt->sh_offset);
            uint32_t *end = p_add_offset(inst, elt->sh_size);
            for (; inst < end; inst++) {
                printf("%08x ", *inst);
                print_binary(*inst);
                print_parsed_inst(parse_single(*inst));
                printf("\n");
            }
        }
    }
    printf("\n");
}
