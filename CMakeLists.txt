cmake_minimum_required(VERSION 3.12)

project(simple_empulator VERSION 0.1)
#set(CMAKE_C_FLAGS "Wall Wextra Werror pedantic g")


add_executable(arm_emul "")

#include dir
target_include_directories(arm_emul PRIVATE "${CMAKE_SOURCE_DIR}/inc/")

#source file
set(SOURCE_FILES pointer_arithmetic.c elf_program_header.c elf_header.c elf_reader.c main.c)
list(TRANSFORM SOURCE_FILES PREPEND "src/")
target_sources(arm_emul PRIVATE ${SOURCE_FILES})

target_compile_options(arm_emul PRIVATE -Wall -Wextra -Werror -pedantic -g)

