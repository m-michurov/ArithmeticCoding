#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#include "coding.h"
#include "errors.h"
#include "iodefinitions.h"
#include "test.h"


#define RELEASE


int main(
        int argc,
        char * argv[])
{
#ifdef RELEASE
    int errcode;

    clock_t start;
    clock_t end;

    if (argc == 4 || argc == 5) {
        if (!strcmp(argv[1], "-e"))
        {
            start = clock();
            errcode = encode(argv[argc - 2], argv[argc - 1]);
            end = clock();

            switch (errcode) {
                case SUCCESS:
                    printf("Successfully encoded \"%s\" to \"%s\"\n", argv[argc - 2], argv[argc - 1]);

                    if (!strcmp(argv[2], "-t"))
                    {
                        printf("Encode time: %.2lf s\n", (double) (end - start) / 1000);
                    }

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
            start = clock();
            errcode = decode(argv[argc - 2], argv[argc - 1]);
            end = clock();

            switch (errcode) {
                case SUCCESS:
                    printf("Successfully decoded \"%s\" to \"%s\"\n", argv[argc - 2], argv[argc - 1]);

                    if (!strcmp(argv[2], "-t"))
                    {
                        printf("Decode time: %.2lf s\n", (double) (end - start) / 1000);
                    }

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
            printf("Bad argument: \"%s\".\nUsage: -[e|d] -[t] <input file name> <output file name>\n", argv[1]);
        }
    }
    else {
        printf("Bad number of arguments.\nUsage: -[e|d] -[t] <input file name> <output file name>\n");
    }
#else

    test();

#endif
    return 0;
}