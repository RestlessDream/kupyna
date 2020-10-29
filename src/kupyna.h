#ifndef _KUPYNA_H_
#define _KUPYNA_H_

#include "static.h"

void hash(dw * msg, size_t blocks, dw * out);
void t_map_xor(dw * state);
void t_map_add(dw * state);

void sub_bytes(dw * state);
void shift_rows(dw * state);
void mix_columns(dw * state);
void xor_round_key(dw * state, size_t round);
void add_round_key(dw * state, size_t round);
void xor_arr(dw * dest, dw * state, dw * msg);
void xor_3arr(dw * dest, dw * state, dw * t1, dw * t2);
void matrix_multiply(dw * state, uint8_t mat[8][8]);
byte multiplyGF(byte x, byte y);
 
void pad_block(dw * msg_block, uint64_t size);
void setup(kupyna_config * kupyna_std);

int proof_of_work(dw * in, size_t blocks, size_t zeros_count, dw * out);
int cmpbits(dw v1, dw v2, size_t count);

#endif 
