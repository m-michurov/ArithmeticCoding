#pragma once

#ifndef HUFFMANARCHIVER_BINIO_H
#define HUFFMANARCHIVER_BINIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"


//#define DEBUG_BIT_OUTPUT


#define NUM_OF_DIFFERENT_BYTES  256 // 2^8 do not change
#define BLOCK_SIZE              65536
#define MAX_FILE_SIZE           4294966500


#define WRITE   0
#define READ    BLOCK_SIZE

typedef struct st_BitBuff
{
    size_t byte_pos;
    size_t bit_pos;

    FILE * file;

    unsigned char string[BLOCK_SIZE];
} IO_BUFF;


void BitWrite(
        IO_BUFF * out,
        int bit);


int BitRead(
        IO_BUFF *in);


void ByteWrite(
        IO_BUFF * out,
        unsigned char byte);


unsigned char ByteRead(
        IO_BUFF * in);


void EndWrite(
        IO_BUFF * out);


void NextByte(
        IO_BUFF * buff);

// Boolean variable "mode" should be
// either WRITE or READ
IO_BUFF * InitBinaryIO(
        FILE * file,
        bool mode);

#endif
