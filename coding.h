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


#define BLOCK_SIZE      (size_t)(1024u * 1024u)


#define MAX_FILE_SIZE   (1024u * 1024u * 1024u * 4u - 64u)
#define NO_OF_BYTES     256u // 2^8 do not change
#define NO_OF_SYMBOLS   NO_OF_BYTES

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
        const char * in_file,
        const char * out_file);


int decode(
        const char * in_file,
        const char * out_file);


#endif
