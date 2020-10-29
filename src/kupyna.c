#include "kupyna.h"

#include "util.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

extern kupyna_config * kupyna;
extern const size_t rows_count;
extern size_t byte_size;
extern const byte s_box[4][16][16];
extern size_t s_box_dimensions;
extern byte mds_matrix[8][8];

/**
 * For inputed message returns digest
 *
 * @param msg - input message
 * @param blocks - message block size
 * @param out - message digest
 */
void hash(dw * msg, size_t blocks, dw * out) {
    dw * state = (dw *) malloc(rows_count * kupyna->state_rows);
    dw * t1 = (dw *) malloc(rows_count * kupyna->state_rows);
    dw * t2 = (dw *) malloc(rows_count * kupyna->state_rows);
    dw * cur_msg;

    memset(state, 0, rows_count * kupyna->state_rows);

    ((byte *)&state[0])[0] = 0x80; 

    if (kupyna->block_size == 512) {

        ((byte *)&state[0])[0] >>= 1; 
    }

    for (int i = 0; i < blocks; i++) {
        cur_msg = msg + i * kupyna->block_dwsize;
        xor_arr(t1, state, cur_msg);
        memcpy(t2, cur_msg, kupyna->block_bsize);

        t_map_xor(t1);
        t_map_add(t2);

        xor_3arr(state, state, t1, t2);
    }

    memcpy(t1, state, kupyna->block_bsize);
    t_map_xor(t1);
    xor_arr(state, state, t1);

    memcpy(out, state, kupyna->message_diggest_bsize);

    free(state);
    free(t1);
    free(t2);
}

/**
 * T xor transformation 
 *
 * @param state - state matrix
 */
void t_map_xor(dw * state) {
    for (size_t i = 0; i < kupyna->rounds; i++) {
        xor_round_key(state, i);
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
    }
}

/**
 * T add transformation 
 *
 * @param state - state matrix
 */
void t_map_add(dw * state) {
    for (size_t i = 0; i < kupyna->rounds; i++) {
        add_round_key(state, i);
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
    }
}

/**
 * Realization of sub bytes
 *
 * @param state - state matrix
 */ 
void sub_bytes(dw * state) {
    for (size_t clmn = 0; clmn < kupyna->state_rows; clmn++) {
        byte * state_clmn = (byte *)&state[clmn];
        for (size_t row = 0; row < rows_count; row++) {
            state_clmn[row] = s_box[row % s_box_dimensions][high_bits(state_clmn[row])]
                                                      [low_bits(state_clmn[row])];
        }
    }
}

/**
 * Realization of shift rows
 *
 * @param state - state matrix
 */ 
void shift_rows(dw * state) {
    size_t i;
    for (i = 0; i < rows_count - 1; i++) {
        size_t shift_s = i;

        for (int j = 0; j < shift_s; j++) {
            byte swp = ((byte *)&state[kupyna->state_rows - 1])[i];

            for (int k = kupyna->state_rows - 1; k > 0; k--) {
                ((byte *)&state[k])[i] = ((byte *)&state[k - 1])[i];
            }

            ((byte *)&state[0])[i] = swp;
        }
    }

    size_t shift_s = kupyna->block_size == 512 ? i : 11;

    for (int j = 0; j < shift_s; j++) {
        byte swp = ((byte *)&state[kupyna->state_rows - 1])[i];

        for (int k = kupyna->state_rows - 1; k > 0; k--) {
            ((byte *)&state[k])[i] = ((byte *)&state[k - 1])[i];
        }

        ((byte *)&state[0])[i] = swp;
    }
}

/**
 * Realization of mix columns
 *
 * @param state - state matrix
 */ 
void mix_columns(dw * state) {
    matrix_multiply(state, mds_matrix);
}

/**
 * Realization of xor round key
 *
 * @param dest - xor destination
 * @param state - first array
 * @param msg - second array
 */
void xor_round_key(dw * state, size_t round) {
    for (size_t i = 0; i < kupyna->state_rows; i++) {
        ((byte *)&state[i])[0] ^= (i << 4) ^ round;
    }
}

/**
 * Realization of add round key
 *
 * @param dest - xor destination
 * @param state - first array
 * @param msg - second array
 */
void add_round_key(dw * state, size_t round) {
    for (size_t i = 0; i < kupyna->state_rows; i++) {
        state[i] += 0x00F0F0F0F0F0F0F3 ^ ((kupyna->state_rows - i - 1) << 4);
    }
}

/**
 * Xors arrays
 *
 * @param dest - xor destination
 * @param state - first array
 * @param msg - second array
 */
void xor_arr(dw * dest, dw * state, dw * msg) {
    for (size_t i = 0; i < kupyna->state_rows; i++) {
        dest[i] = state[i] ^ msg[i];
    }
}

/**
 * Xors 3 arrays
 *
 * @param dest - xor destination
 * @param state - first array
 * @param t1 - second array
 * @param t2 - third array
 */
void xor_3arr(dw * dest, dw * state, dw * t1, dw * t2) {
    for (size_t i = 0; i < kupyna->state_rows; i++) {
        dest[i] = state[i] ^ t1[i] ^ t2[i];
    }
}

/**
 * Multiply matrix in GF
 *
 * @param state - first matrix
 * @param mat - second matrix
 *
 * @return - result of multiply
 */
void matrix_multiply(dw * state, uint8_t mat[8][8]) {
    byte product;
    dw result;

    for (int col = 0; col < kupyna->state_rows; col++) {
        result = 0;

        for (int row = rows_count - 1; row >= 0; row--) {
            product = 0;
            for (int i = rows_count - 1; i >= 0; i--) {
                product ^= multiplyGF(((byte *)&state[col])[i], mat[row][i]);
            }

            result |= (dw)product << (row * rows_count);
        }

        state[col] = result;
    }
}

/**
 * Multiply bytes in GF
 *
 * @param x - first byte
 * @param y - second
 *
 * @return - result of multiply
 */
byte multiplyGF(byte x, byte y) {
    byte r = 0;
    byte hbit = 0;

    for (size_t i = 0; i < byte_size; i++) {
        if ((y & 0x1) == 1) {
            r ^= x;
        }

        hbit = x & 0x80;
        x <<= 1;

        if (hbit == 0x80) {
            x ^= 0x011d;
        }

        y >>= 1;
    }

    return r;
}

/**
 * Padding ensures that the padded message is a miltiple of block bits.
 *
 * Padd message with 10..0 and number that represent size of message in block.
 *
 * @param msg_block - the last message block, that going to be padded;
 * @param size - the whole message size.
 */
void pad_block(dw * msg_block, uint64_t size) {
    size_t block_size = size % kupyna->block_size;
    size_t pad_zeros = kupyna->block_size - block_size - 96;
    byte * msg = (byte *) msg_block;

    msg[block_size/byte_size] = 0x80;
    memset(msg + block_size/byte_size + 1, 0, pad_zeros/byte_size);

    for (size_t i = 0; i < 12; i++) { 
        if (i < sizeof(uint64_t)) {
            *(msg + kupyna->block_bsize - i - 1) = ((byte *)&size)[i];
        } else {
            *(msg + kupyna->block_bsize - i - 1) = 0;
        }
    }
}

void setup(kupyna_config * kupyna_std) {
    kupyna = kupyna_std;
}

/**
 * Realization of proof of work
 *
 * @param in - input message
 * @param blocks - size of message in blocks
 * @param zeros_count - seeked zeros count at the begining
 * @param out - seeked digest
 *
 * @return 0 if all OK
 */
int proof_of_work(dw * in, size_t blocks, size_t zeros_count, dw * out) {
    if (zeros_count >= 64) {
        return -1;
    }

    dw x = 0;
    size_t i = 0;

    do {
        in[blocks * kupyna->block_dwsize + i] = x;
        hash(in, blocks + 1, out);
        x++;

        if (x == 0xFFFFFFFFFFFFFFFF) {
            i++;
        }
    } while(cmpbits(out[kupyna->message_diggest_bsize/sizeof(dw) - 1], 0, zeros_count) == 0);

    return 0;
}

/**
 * Finds out is the value v1 starts count zeros
 *
 * @param v1 - tested value
 * @param v2 - ored value
 * @return 1 - if value starts with count zeros
 */
int cmpbits(dw v1, dw v2, size_t count) {
    if (((v1 | v2) & (0xFFFFFFFFFFFFFFFF >> (64 - count))) != 0) {
        return 0;
    }

    return 1;
}
