#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

enum cond {
    COND_EQZ = 0b0000,
    COND_NEZ = 0b0001,
    COND_NE  = 0b0010,
    COND_CC  = 0b0011,
    COND_MIN = 0b0100,
    COND_PLN = 0b0101,
    COND_VSV = 0b0110,
    COND_VCV = 0b0111,
    COND_HIC = 0b1000,
    COND_LSC = 0b1001,
    COND_GEN = 0b1010,
    COND_LTN = 0b1011,
    COND_GTZ = 0b1100,
    COND_LEZ = 0b1101,
    COND_AL  = 0b1110,
    COND_ERR = 0b1111,
};

enum data_processing {
    DP_AND = 0b0000,
    DP_EOR = 0b0001,
    DP_SUB = 0b0010,
    DP_RSB = 0b0011,
    DP_ADD = 0b0100,
    DP_ADC = 0b0101,
    DP_SBC = 0b0110,
    DP_RSC = 0b0111,
    DP_TST = 0b1000,
    DP_TEQ = 0b1001,
    DP_CMP = 0b1010,
    DP_CMN = 0b1011,
    DP_ORR = 0b1100,
    DP_MOV = 0b1101,
    DP_BIC = 0b1110,
    DP_MVN = 0b1111
};

extern const char *const data_pro_str[];
extern const char *const cond_str[];

struct reg {
    uint8_t rm;
    uint8_t shift;
};

struct lmm {
    uint8_t rotate;
    uint8_t lmm;
};

union dt_operand2 {
    struct lmm lmm;
    struct reg reg;
};

struct data_processing_inst {
    enum data_processing opcode;
    uint8_t             s;
    uint8_t             rn;
    uint8_t             rd;
    uint8_t             i;
    union dt_operand2   op2;
};

struct branch_inst {
    uint8_t  link;
    uint32_t address;
};

enum instruction_type {
    INST_T_BRANCH,
    INST_T_DATA_PROCESSING,
    INST_T_SINGLE_DATA_TRANSFER,
    INST_T_NOT_IMPLEMENTED_YET
};

struct single_data_transfer {
    uint8_t ipubwl; //bitmask
    uint8_t rn;
    uint8_t rd;
    uint32_t offset;
};

union instructions {
    struct data_processing_inst data_proc;
    struct branch_inst          branch;
    struct single_data_transfer sd_transfer;
};

struct instruction {
    enum cond             cond;
    enum instruction_type type;
    union instructions    inst;
};

struct instruction  parse_single(const uint32_t inst);
void                print_parsed_inst(struct instruction s);
struct instruction *parse(const uint32_t *inst, unsigned nb_instructions);

#endif /* PARSER_H */
