#pragma once

#ifndef ARITHMETICCODING_CODING_H
#define ARITHMETICCODING_CODING_H


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>


#include "errors.h"
#include "iodefinitions.h"
#include "bool.h"


#define force_inline __attribute__((always_inline)) inline


#define PERCENTAGE_OUTPUT


#define REG_BITS        16u


#define BLOCK_SIZE      (size_t)(1024 * 1024)


#define MAX_FILE_SIZE   (UINT_MAX - 64)
#define NO_OF_BYTES     256 // 2^8 do not change
#define NO_OF_SYMBOLS   (NO_OF_BYTES + 1)

#define TOP_VALUE       ((1u << REG_BITS) - 1u)
#define FIRST_QUARTER   (TOP_VALUE / 4 + 1)
#define HALF            (FIRST_QUARTER * 2)
#define THIRD_QUARTER   (FIRST_QUARTER * 3)

#define MAX_FREQUENCY   (TOP_VALUE / 4)


#define BitsPlusFollow1(bit_pos, byte_pos, bit_buff) \
{\
    bit_buff[byte_pos] |= (1u << (7u - bit_pos++));\
\
    if (bit_pos > 7) \
    {\
        bit_pos = 0;\
        byte_pos++;\
\
        if (byte_pos == BLOCK_SIZE) \
        {\
            fwrite(bit_buff, 1, BLOCK_SIZE, fout);\
            byte_pos = 0;\
\
            memset(bit_buff, 0, BLOCK_SIZE);\
        }\
    }\
\
    for ( ; bits_to_follow > 0; bits_to_follow--)\
    {\
        bit_pos++;\
\
        if (bit_pos > 7) \
        {\
            bit_pos = 0;\
            byte_pos++;\
\
            if (byte_pos == BLOCK_SIZE) \
            {\
                fwrite(bit_buff, 1, BLOCK_SIZE, fout);\
                byte_pos = 0;\
\
                memset(bit_buff, 0, BLOCK_SIZE);\
            }\
        }\
    }\
}


#define BitsPlusFollow0(bit_pos, byte_pos, bit_buff) \
{\
   bit_pos++;\
\
    if (bit_pos > 7) \
    {\
        bit_pos = 0;\
        byte_pos++;\
\
        if (byte_pos == BLOCK_SIZE) \
        {\
            fwrite(bit_buff, 1, BLOCK_SIZE, fout);\
            byte_pos = 0;\
\
            memset(bit_buff, 0, BLOCK_SIZE);\
        }\
    }\
\
    for ( ; bits_to_follow > 0; bits_to_follow--)\
    {\
        bit_buff[byte_pos] |= (1u << (7u - bit_pos++));\
\
        if (bit_pos > 7) \
        {\
            bit_pos = 0;\
            byte_pos++;\
\
            if (byte_pos == BLOCK_SIZE) \
            {\
                fwrite(bit_buff, 1, BLOCK_SIZE, fout);\
                byte_pos = 0;\
\
                memset(bit_buff, 0, BLOCK_SIZE);\
            }\
        }\
    }\
}


int encode(
        char * in_file,
        char * out_file);


int decode(
        char * in_file,
        char * out_file);


#endif
