#include "parser.h"
#include "pointer_arithmetic.h"
#include <stdio.h>
#include <stdlib.h>

const char *const data_op_string[] = {
    [AND] = "AND", [EOR] = "EOR", [SUB] = "SUB", [RSB] = "RSB",
    [ADD] = "ADD", [ADC] = "ADC", [SBC] = "SBC", [RSC] = "RSC",
    [TST] = "TST", [TEQ] = "TEQ", [CMP] = "CMP", [CMN] = "CMN",
    [ORR] = "ORR", [MOV] = "MOV", [BIC] = "BIC", [MVN] = "MVN"};
const char *const cond_str[] = {
    [EQZ] = "EQZ", [NEZ] = "NEZ", [NE] = "NE",   [MIN] = "MIN", [PLN] = "PLN",
    [VSV] = "VSV", [VCV] = "VCV", [HIC] = "HIC", [LSC] = "LSC", [GEN] = "GEN",
    [LTN] = "LTN", [GTZ] = "GTZ", [LEZ] = "LEZ", [AL] = "AL",   [ERR] = "ERR",
};
const unsigned char data_operation_filter   = 0b00001100;
const unsigned char data_operation_rbitmask = 0b00000000;
struct instruction  parse_single(const uint32_t inst) {
    // parse
    uint32_t rinst = reverse_endianess(inst);
    char *             data = (char *) &rinst;
    struct instruction res;
    res.succesfully_parsed = 0;
    res.cond = (enum cond)(data[0] & 0b11110000) >> 4;
    if (!((data[0] & data_operation_filter) ^ data_operation_rbitmask)) {
        char op = 0;
        op += (data[0] & 0x01) << 3;
        op += (data[1] & 0b11100000) >> 5;
        res.op                 = (enum data_operation) op;
        res.i                  = data[0] & 0b00000010;
        res.s                  = data[1] & 0b00010000;
        res.rn                 = data[1] & 0b00001111;
        res.rd                 = (data[2] & 0b11110000) >> 4;
        res.succesfully_parsed = 1;
    }

    return res;
}

void print_parsed_inst(struct instruction s) {
    if (!s.succesfully_parsed) {
        printf("not implemented yet, cond %s \n", cond_str[s.cond]);
        return;
    }

    printf("%s rd %02x rn %02x i %hhx s %hhx", data_op_string[s.op], s.rd, s.rn,
           s.i, s.s);
}

struct instruction *parse(const uint32_t *inst, unsigned nb_instructions) {
    struct instruction *res =
        calloc(nb_instructions, sizeof(struct instruction));
    if (!res)
        return NULL;

    for (unsigned index = 0; index < nb_instructions; index++)
        res[index] = parse_single(inst[index]);

    return res;
}
