#pragma once

#ifndef ARITHMETICCODING_TEST_H
#define ARITHMETICCODING_TEST_H


#include <stdio.h>
#include <time.h>


#include "coding.h"
#include "errors.h"


#define CMP_ERR     (-10)


void test(
        void);


int filecmp(
        char * file_name1,
        char * file_name2);

#endif
