#pragma once

#ifndef ARITHMETICCODING_CODING_H
#define ARITHMETICCODING_CODING_H


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#include "binio.h"
#include "errors.h"
#include "iodefinitions.h"


#ifndef REG_BITS
#define REG_BITS        16u
#endif

#ifndef BLOCK_SIZE
#define BLOCK_SIZE      (size_t)(1024 * 1024)
#endif

#define MAX_FILE_SIZE   (UINT_MAX - 64)
#define NO_OF_BYTES     256 // 2^8 do not change
#define NO_OF_SYMBOLS   (NO_OF_BYTES + 1)

#define TOP_VALUE       ((1u << REG_BITS) - 1u)
#define FIRST_QUARTER   (TOP_VALUE / 4 + 1)
#define HALF            (FIRST_QUARTER * 2)
#define THIRD_QUARTER   (FIRST_QUARTER * 3)

#define MAX_FREQUENCY   (TOP_VALUE / 4)

#ifdef EOF_OPTIMISATION
#define EOF_SYMBOL      (NO_OF_BYTES + 1)
#endif


int encode(
        char * in_file,
        char * out_file);


int decode(
        char * in_file,
        char * out_file);


#endif
