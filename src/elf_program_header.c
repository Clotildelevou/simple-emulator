#include "pointer_arithmetic.h"
#include "elf_program_header.h"
#include <elf.h>
#include <stdio.h>

void print_ph_type(Elf32_Phdr *elt) {
    printf("addr %p\n", (void *) elt);
    switch (elt->p_type) {
        case PT_NULL:
            printf("unused header\n");
            break;

        case PT_LOAD:
            printf("loadable segment\n");
            break;

        case PT_DYNAMIC:
            printf("dynamic info for relocation\n");
            break;

        case PT_INTERP:
            printf("interpreter path\n");
            break;

        case PT_NOTE:
            printf("notes\n");
            break;

        case PT_SHLIB:
            printf("undefined ABI\n");
            break;

        case PT_PHDR:
            printf("program header table\n");
            break;

        case PT_GNU_STACK:
            printf("gnu stack\n");
            break;
        default:
            if (elt->p_type >= PT_LOPROC && elt->p_type <= PT_HIPROC) {
                printf("processor specific instructions\n");
            } else {
                printf("failed\n");
            }
    }
}

void print_ph_flag(Elf32_Phdr *elt) {
    printf("segment is");
    if (elt->p_flags & PF_X)
            printf(" executable");
    if (elt->p_flags & PF_W)
            printf(" writable");
    if (elt->p_flags & PF_R)
            printf(" readable");
    printf("\n");
}

void print_program_header(Elf32_Phdr *arr, unsigned size_elt, unsigned nb) {
    printf("-----------------------program headers-----------------------\n");
    for (unsigned k = 0; k < nb; k++) {

        printf("\n");
        Elf32_Phdr *elt = p_add_offset(arr, k * size_elt);
        print_ph_type(elt);
        print_ph_flag(elt);
    }
    printf("\n");
}
