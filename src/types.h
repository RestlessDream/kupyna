#ifndef _TYPES_H_
#define _TYPES_H_

#include "stdlib.h"
#include "stdint.h"

typedef unsigned char byte;
typedef uint32_t word;
typedef uint64_t dw;
typedef void (*operation)(dw *, size_t, dw *);

typedef struct {
    const size_t message_bsize;
    const size_t block_size;
    const size_t block_bsize;
    const size_t block_dwsize;
    const size_t message_diggest_bsize;
    const size_t rounds;
    const size_t state_rows;
} kupyna_config;

#endif // TYPES_H_
