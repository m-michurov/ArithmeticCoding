#pragma once

#ifndef HUFFMANARCHIVER_BINIO_H
#define HUFFMANARCHIVER_BINIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"


//#define DEBUG_BIT_OUTPUT


#define BLOCK_SIZE      (size_t)(1024 * 1024)


#define WRITE   0
#define READ    BLOCK_SIZE

typedef struct st_BitBuff
{
    size_t byte_pos;
    size_t bit_pos;

    FILE * file;

    unsigned char * string;
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
