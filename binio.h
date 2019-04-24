#pragma once

#ifndef HUFFMANARCHIVER_BINIO_H
#define HUFFMANARCHIVER_BINIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"


//#define DEBUG_BIT_OUTPUT


#ifndef BLOCK_SIZE
#define BLOCK_SIZE      (size_t)(1024 * 1024)
#endif

#ifndef REG_BITS
#define REG_BITS        16u
#endif

#define WRITE   0
#define READ    BLOCK_SIZE



#define force_inline __attribute__((always_inline)) inline

#define FORCE_INLINE

typedef struct st_BitBuff
{
    unsigned int byte_pos;
    unsigned int bit_pos;

    size_t read;

    unsigned int garbage_bits;
    bool data_corrupted;

    FILE * file;

    unsigned char * string;
} IO_BUFF;


void BitWrite(
        IO_BUFF * out,
        int bit);


int BitRead(
        IO_BUFF *in);


void EndWrite(
        IO_BUFF * out);

// Boolean variable "mode" should be
// either WRITE or READ
IO_BUFF * InitBinaryIO(
        FILE * file,
        bool mode);

#endif
