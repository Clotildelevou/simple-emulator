#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>


enum cond {
    EQZ = 0b0000,
    NEZ = 0b0001,
    NE  = 0b0010,
    MIN = 0b0100,
    PLN = 0b0101,
    VSV = 0b0110,
    VCV = 0b0111,
    HIC = 0b1000,
    LSC = 0b1001,
    GEN = 0b1010,
    LTN = 0b1011,
    GTZ = 0b1100,
    LEZ = 0b1101,
    AL  = 0b1110,
    ERR = 0b1111,
};
enum data_operation {
    AND = 0b0000,
    EOR = 0b0001,
    SUB = 0b0010,
    RSB = 0b0011,
    ADD = 0b0100,
    ADC = 0b0101,
    SBC = 0b0110,
    RSC = 0b0111,
    TST = 0b1000,
    TEQ = 0b1001,
    CMP = 0b1010,
    CMN = 0b1011,
    ORR = 0b1100,
    MOV = 0b1101,
    BIC = 0b1110,
    MVN = 0b1111
};


struct instruction {
    int succesfully_parsed;
    enum data_operation op;
    enum cond cond;
    char s;
    char rn;
    char rd;
    char i;
};

struct instruction parse_single(const uint32_t inst);
void print_parsed_inst(struct instruction s);
struct instruction* parse(const uint32_t* inst, unsigned nb_instructions);

#endif /* PARSER_H */
