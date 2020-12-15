#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>

#include "pointer_arithmetic.h"
#include "elf_reader.h"
#include "elf_header.h"
#include "elf_program_header.h"
#include "elf_section_header.h"

void * open_and_map(const char* path, off_t* size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Could not open file" );
        return NULL;
    }

    struct stat buf;

    int r = fstat(fd, &buf);
    if (r < 0) {
        perror("Could not stat file");
        return NULL;
    }

    if (buf.st_size < (off_t)sizeof(Elf32_Ehdr)) {
        fprintf(stderr, "File is too short to contain anything\n");
    }

    *size = buf.st_size;
    char *file = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!file) {
        perror("Could not map file");
        return NULL;
    }
    return file;
}

//checks if there are error in elf
//and if we are in good file format
int check_valid_file(const char* file) {
    // checks if file starts with 0x7f E L F
    char MAGIC[] = { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3 };
    if (strncmp(MAGIC, file, 4)) {
        fprintf(stderr, "ELF file must start with 0x7fELF\n");
        return 1;
    }


    int is32bitlittleendian = 1;
    char* bits = NULL;
    switch (file[EI_CLASS]) {
        case ELFCLASS32:
            bits = "32bit";
            break;
        case ELFCLASS64:
            is32bitlittleendian = 0;
            bits = "64bit";
            break;
        case ELFCLASSNONE:
        default:
            is32bitlittleendian = 0;
            fprintf(stderr, "wrong file architecture bit field\n");
    }

    char* endianness = NULL;
    switch (file[EI_DATA]) {
        case ELFDATA2LSB:
            endianness = "little endian";
            break;
        case ELFDATA2MSB:
            is32bitlittleendian = 0;
            endianness = "big endian";
            break;
        case ELFDATANONE:
        default:
            is32bitlittleendian = 0;
            fprintf(stderr, "wrong file architecture endian field\n");
    }

    printf("Opening a %s %s ELF file\n", bits, endianness);

    if (!is32bitlittleendian)
        return 1;
    return 0;
}



int elf_read(const char* path) {
    // first we map file in mem
    off_t file_size;
    char* file = open_and_map(path, &file_size);
    if (!file)
        return 1;

    int res = check_valid_file(file);
    if (!res) {
        printf("file size is %lu\n", file_size);
        Elf32_Ehdr* header = (Elf32_Ehdr*)file;
        print_machine_arch(header);
        print_elf_type(header);
        print_entry(header);
        printf("ph offset %u\n", header->e_phoff);

        Elf32_Phdr* ph = p_add_offset(header, header->e_phoff);
        printf("nb program headers %u\n", header->e_phnum);
        printf("program header size %u\n", header->e_phentsize);
        print_program_header(ph, header->e_phentsize, header->e_phnum);

        Elf32_Shdr* sh = p_add_offset(header, header->e_shoff);
        printf("nb program headers %u\n", header->e_shnum);
        printf("program header size %u\n", header->e_shentsize);
        print_section_header(sh, header->e_shentsize, header->e_shnum);
    }

    if (munmap(file, file_size)) {
        perror("Error unmapping");
    }
    if (!res)
        printf("everything is good\n");
    return res;
}
