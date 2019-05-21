#include "test.h"


int filecmp(
        char * file_name1,
        char * file_name2)
{
    FILE * file1 = fopen(file_name1, "rb");
    FILE * file2 = fopen(file_name2, "rb");

    unsigned char * buff1 = (unsigned char *) calloc(BLOCK_SIZE, 1);
    unsigned char * buff2 = (unsigned char *) calloc(BLOCK_SIZE, 1);

    unsigned int read1 = 0;
    unsigned int read2 = 0;

    if (file1 == NULL)
    {
        return -100;
    }

    if (file2 == NULL)
    {
        return -200;
    }

    if (buff1 == NULL || buff2 == NULL)
    {
        return ALLOCATION_ERROR;
    }

    while ((read2 = (unsigned int) fread(buff2, 1, BLOCK_SIZE, file2)) == (read1 = (unsigned int) fread(buff1, 1, BLOCK_SIZE, file1)) && read1 > 0)
    {
        if (memcmp(buff1, buff2, read1) != 0)
        {
            fclose(file1);
            fclose(file2);
            return CMP_ERR;
        }
    }

    if (read1 != read2)
    {
        fclose(file1);
        fclose(file2);
        return CMP_ERR - 1;
    }

    fclose(file1);
    fclose(file2);

    return 0;
}


size_t filesize(
        char * filename)
{
    FILE * file = fopen(filename, "rb");

    if (file == NULL)
    {
        return (size_t) IN_FILE_ERROR;
    }

    size_t size = 0;
    size_t read;

    unsigned char * buff = (unsigned char *) calloc(BLOCK_SIZE, 1);

    if (buff == NULL)
    {
        return (size_t) ALLOCATION_ERROR;
    }

    while ((read = fread(buff, 1, BLOCK_SIZE, file)))
    {
        size += read;
    }

    fclose(file);

    return size;
}


void test(
        void)
{
    static char * infilenames[] = {
            "in.txt",
            "nier.mp4",
            "ArithmeticCoding.exe"
    };
    static char * newfilenames[] = {
            "in.txt.bin",
            "nier.mp4.bin",
            "ArithmeticCoding.exe.bin"
    };

    int errcode;

    size_t size;
    size_t size_comp;

    clock_t encode_start;
    clock_t encode_end;
    clock_t decode_start;
    clock_t decode_end;

    for (int y = 0; y < 3; y++)
    {
        printf("TEST %d/3:\n", y + 1);

        encode_start = clock();
        errcode = encode(infilenames[y], "out.txt");
        encode_end = clock();

        if (!errcode)
        {
            printf("\tEncode time: %.2lf s\n", (double) (encode_end - encode_start) / 1000);

            decode_start = clock();
            errcode = decode("out.txt", newfilenames[y]);
            decode_end = clock();

            if (!errcode)
            {
                printf("\tDecode time: %.2lf s\n", (double) (decode_end - decode_start) / 1000);

                errcode = filecmp(infilenames[y], newfilenames[y]);

                if (!errcode)
                {
                    printf("PASSED:\n");

                    size = filesize(infilenames[y]);
                    size_comp = filesize("out.txt");

                    printf("\t\"%s\": compressed %u %s to %u %s\n\tCompression rate %.2lf\n",
                            infilenames[y], FORMAT_SIZE_SIZE(size), FORMAT_SIZE_PREFIX(size),
                                            FORMAT_SIZE_SIZE(size_comp), FORMAT_SIZE_PREFIX(size_comp),
                           (double) size / size_comp);
                }
                else
                {
                    printf("FAILED: file decoded incorrectly\n");
                }
            }
            else
            {
                printf("FAILED: decode returned error code %s\n", CODE_TO_MESSAGE(errcode));
            }
        }
        else
        {
            printf("FAILED: encode returned error code %s\n", CODE_TO_MESSAGE(errcode));
        }
    }
}