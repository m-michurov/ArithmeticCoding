#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#include "binio.h"
#include "coding.h"
#include "errors.h"
#include "iodefinitions.h"
#include "test.h"


//#define RELEASE


int main(
        int argc,
        char * argv[])
{
#ifdef RELEASE
    int errcode;

    if (argc == 4) {
        if (!strcmp(argv[1], "-e"))
        {
            errcode = encode(argv[argc - 2], argv[argc - 1]);

            switch (errcode) {
                case SUCCESS:
                    printf("\nSuccessfully encoded \"%s\" to \"%s\"\n", argv[argc - 2], argv[argc - 1]);
                    break;
                case FILE_SIZE_ERROR:
                    printf("Input file \"%s\" exceeds maximum supported file size of %u %s\n", argv[argc - 2],
                           FORMAT_SIZE_SIZE(MAX_FILE_SIZE), FORMAT_SIZE_PREFIX(MAX_FILE_SIZE));
                    return 0;
                case IN_FILE_ERROR:
                    printf("Unable to open file \"%s\"\n", argv[argc - 2]);
                    return 0;
                case OUT_FILE_ERROR:
                    printf("Unable to open file \"%s\"\n", argv[argc - 1]);
                    return 0;
                case ALLOCATION_ERROR:
                    printf("Process terminated due to memory allocation error");
                    return 0;
                default:
                    return 0;
            }
        }
        else if (!strcmp(argv[1], "-d"))
        {
            errcode = decode(argv[argc - 1], "new.bin");

            switch (errcode) {
                case SUCCESS:
                    printf("\nSuccessfully decoded \"%s\" to \"%s\"\n", argv[argc - 2], argv[argc - 1]);
                    break;
                case DATA_CORRUPTION_ERROR:
                    printf("\nData is corrupted and cannot be decoded\n\n");
                    return 0;
                case IN_FILE_ERROR:
                    printf("\nUnable to open file \"%s\"\n", argv[argc - 2]);
                    return 0;
                case OUT_FILE_ERROR:
                    printf("\nUnable to open file \"%s\"\n", argv[argc - 1]);
                    return 0;
                case ALLOCATION_ERROR:
                    printf("\nProcess terminated due to memory allocation error\n");
                    return 0;
                default:
                    return 0;
            }
        }
        else {
            printf("Bad command line arguments");
        }
    }
    else {
        printf("Bad command line arguments");
    }
#else
/*
    encode("ArithmeticCoding.exe", "out.txt");
    decode("out.txt", "ArithmeticCoding.exe.bin");

    printf("%d", filecmp("ArithmeticCoding.exe", "ArithmeticCoding.exe.bin"));
*/

    test();

#endif
    return 0;
}