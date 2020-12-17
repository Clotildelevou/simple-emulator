#include "parser.h"
#include "pointer_arithmetic.h"
#include <stdio.h>
#include <stdlib.h>

const char *const data_op_str[] = {
    [DP_AND] = "AND", [DP_EOR] = "EOR", [DP_SUB] = "SUB", [DP_RSB] = "RSB",
    [DP_ADD] = "ADD", [DP_ADC] = "ADC", [DP_SBC] = "SBC", [DP_RSC] = "RSC",
    [DP_TST] = "TST", [DP_TEQ] = "TEQ", [DP_CMP] = "CMP", [DP_CMN] = "CMN",
    [DP_ORR] = "ORR", [DP_MOV] = "MOV", [DP_BIC] = "BIC", [DP_MVN] = "MVN"};
const char *const branch_str[]  = {[0] = "b", [1] = "bl"};
const char *const data_tr_str[] = {[0] = "str", [1] = "ldr"};

const char *const cond_str[] = {
    [COND_EQZ] = "EQZ", [COND_NEZ] = "NEZ",
    [COND_NE] = "NE",   [COND_CC] = "CC",
    [COND_MIN] = "MIN", [COND_PLN] = "PLN",
    [COND_VSV] = "VSV", [COND_VCV] = "VCV",
    [COND_HIC] = "HIC", [COND_LSC] = "LSC",
    [COND_GEN] = "GEN", [COND_LTN] = "LTN",
    [COND_GTZ] = "GTZ", [COND_LEZ] = "LEZ",
    [COND_AL] = "AL",   [COND_ERR] = "ERROR ILL FORMED",
};

const uint8_t data_processing_filter  = 0b00001100;
const uint8_t data_processing_bitmask = 0b00000000;
const uint8_t branch_filter           = 0b00001110;
const uint8_t branch_bitmask          = 0b00001010;
const uint8_t single_data_filter      = 0b00001100;
const uint8_t single_data_bitmask     = 0b00000100;

int test_inst_type(uint8_t byte, uint8_t filter, uint8_t bitmask) {
    return !((byte & filter) ^ bitmask);
}

struct instruction parse_single(const uint32_t inst) {

    // parse
    uint32_t rinst = reverse_endianess(inst);
    char *   data  = (char *) &rinst;

    struct instruction res;

    res.cond = (enum cond)(data[0] & 0b11110000) >> 4;
    if (test_inst_type(data[0], data_processing_filter,
                       data_processing_bitmask)) {

        res.type = INST_T_DATA_PROCESSING;
        char op  = 0;

        op += (data[0] & 1) << 3;
        op += (data[1] & 0b11100000) >> 5;

        res.inst.data_proc.opcode = (enum data_processing) op;
        res.inst.data_proc.i      = data[0] & 0b00000010;
        res.inst.data_proc.s      = data[1] & 0b00010000;
        res.inst.data_proc.rn     = data[1] & 0b00001111;
        res.inst.data_proc.rd     = (data[2] & 0b11110000) >> 4;

        if (res.inst.data_proc.i) {

            // op2 not a register
            res.inst.data_proc.op2.lmm.rotate = data[3] & 0b00001111;
            res.inst.data_proc.op2.lmm.lmm    = data[4];

        } else {

            // op2 is a register
            res.inst.data_proc.op2.reg.shift = (data[3] & 0b00001111) << 4;
            res.inst.data_proc.op2.reg.shift = (data[4] & 0b11110000) >> 4;
            res.inst.data_proc.op2.reg.rm    = data[4] & 0b00001111;
        }

    } else if (test_inst_type(data[0], branch_filter, branch_bitmask)) {

        res.type                = INST_T_BRANCH;
        res.inst.branch.link    = data[0] & 1;
        res.inst.branch.address = (rinst & 0x00FFFFFF) << 2;

    } else if (test_inst_type(data[0], single_data_filter,
                              single_data_bitmask)) {
        res.type = INST_T_SINGLE_DATA_TRANSFER;

        res.inst.sd_transfer.ipubwl = (data[0] & 0b00000011) << 4;
        res.inst.sd_transfer.ipubwl += (data[1] & 0b11110000) >> 4;
        res.inst.sd_transfer.rn     = data[1] & 0b00001111;
        res.inst.sd_transfer.rd     = (data[2] & 0b11110000) >> 4;
        res.inst.sd_transfer.offset = (data[3] & 0b00001111) << 8;
        res.inst.sd_transfer.offset += data[4];
    } else {
        res.type = INST_T_NOT_IMPLEMENTED_YET;
    }

    return res;
}

void print_parsed_inst(struct instruction s) {
    if (s.type == INST_T_NOT_IMPLEMENTED_YET) {
        printf("not implemented yet, cond %s \n", cond_str[s.cond]);
    } else if (s.type == INST_T_BRANCH) {
        printf("BR: %s %u \n", branch_str[s.inst.branch.link],
               s.inst.branch.address);
    } else if (s.type == INST_T_DATA_PROCESSING) {
        printf("DP: %s rd %02x rn %02x i %hhx s %hhx ",
               data_op_str[s.inst.data_proc.opcode], s.inst.data_proc.rd,
               s.inst.data_proc.rn, s.inst.data_proc.i, s.inst.data_proc.s);
        if (s.inst.data_proc.i) {
            printf("op2 rot %02x lmm %02x\n", s.inst.data_proc.op2.lmm.rotate,
                   s.inst.data_proc.op2.lmm.lmm);
        } else {
            printf("op2 shift %02x lmm %02x\n", s.inst.data_proc.op2.reg.shift,
                   s.inst.data_proc.op2.reg.rm);
        }
    } else if (s.type == INST_T_SINGLE_DATA_TRANSFER) {
        printf("DT: %s write_base %s byte/bit %s updown %s pre/post %s immediate %s\n",
               data_tr_str[s.inst.sd_transfer.ipubwl & 1],
               s.inst.sd_transfer.ipubwl & 0b10 ? "no write b": "write b",
               s.inst.sd_transfer.ipubwl & 0b100 ? "byte" : "word",
               s.inst.sd_transfer.ipubwl & 0b1000 ? "Up" : "Down",
               s.inst.sd_transfer.ipubwl & 0b10000 ? "pre" : "post",
               s.inst.sd_transfer.ipubwl & 0b100000 ? "reg" : "immediate");
    } else {
        printf("WHAT WHAT WHAT ?\n");
    }
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
