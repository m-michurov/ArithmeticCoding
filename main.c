#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "binio.h"


int count_freqs(
        FILE * in,
        unsigned int * frequencies)
{
    unsigned char input_buff[BLOCK_SIZE];

    unsigned int read = 0;
    unsigned int pos = 0;

    unsigned long long total = 0;


    while ((read = fread(input_buff, 1, BLOCK_SIZE, in))) {
        while (pos < read)
            frequencies[(int) input_buff[pos++]]++;

        pos = 0;
        total += read;
    }

    if (total > MAX_FILE_SIZE)
        return -1;

    return 0;
}


void BitsPlusFollow(
        IO_BUFF * out,
        int bit,
        unsigned int * bits_to_follow)
{
    BitWrite(out, bit);

    for ( ; (*bits_to_follow) > 0; (*bits_to_follow) -= 1)
    {
        BitWrite(out, 1 - bit);
    }
}


int EncodeFile(
        char * in_file,
        char * out_file)
{
    unsigned char input_buff[BLOCK_SIZE];
    unsigned char file_size[4] = { 0 };

    unsigned int frequencies[NUM_OF_DIFFERENT_BYTES] = { 0 };
    unsigned int input_file_len = 0;

    unsigned int * b = NULL;
    unsigned int index[NUM_OF_DIFFERENT_BYTES] = { 0 };
    unsigned int unique_bytes_count = 0;

    size_t buff_len = 0;
    size_t byte_pos = 0;

    FILE * fin;
    FILE * fout;

    fin = fopen(in_file, "rb");
    fout = fopen(out_file, "wb");

    if (fin == NULL || fout == NULL)
        return -2;

    count_freqs(fin, frequencies);
    // check the result

    fseek(fin, 0, 0);

    // This ensures independence from unsigned int size
    // on different platforms
    file_size[0] = (unsigned char) ((input_file_len >> 24) & 0xFF);
    file_size[1] = (unsigned char) ((input_file_len >> 16) & 0xFF);
    file_size[2] = (unsigned char) ((input_file_len >> 8) & 0xFF);
    file_size[3] = (unsigned char) ((input_file_len) & 0xFF);


    fwrite(file_size, 1, 4, fout);


    for (int k = 0; k < NUM_OF_DIFFERENT_BYTES; k++)
    {
        if (frequencies[k] > 0)
        {
            unique_bytes_count++;
            input_file_len += frequencies[k];
        }
    }

    b = (unsigned int *) calloc((size_t) unique_bytes_count + 1, sizeof(unsigned int));

    if (b == NULL)
    {
        return -1;
    }

    unique_bytes_count = 0;

    for (int k = 0; k < NUM_OF_DIFFERENT_BYTES; k++)
    {
        if (frequencies[k] > 0)
        {
            unique_bytes_count++;

            b[unique_bytes_count] = b[unique_bytes_count - 1] + frequencies[k];
            printf("index %d, symbol = %c, frequency = %d, b[%d] = %d\n", unique_bytes_count, k, frequencies[k], unique_bytes_count, b[unique_bytes_count]);
            index[k] = unique_bytes_count;
        }
    }


    // i'm so sorry but i'll move declarations i promise

    unsigned int high = 0x10000;
    unsigned int low = 0x0;
    unsigned int prev_high = 0x10000;
    unsigned int prev_low = 0x0;
    unsigned int div = b[unique_bytes_count];
    unsigned int first_quarter = (high + 1) / 4;
    unsigned int half = first_quarter * 2;
    unsigned int third_quarter = first_quarter * 3;
    unsigned int bits_to_follow = 0;

    unsigned char current_symbol = '\0';

    IO_BUFF * out = InitBinaryIO(fout, WRITE);


    while ((buff_len = fread(input_buff, 1, BLOCK_SIZE, fin)) > 0)
    {
        for (int k = 0; k < buff_len; k++)
        {
            current_symbol = input_buff[k];

            low = prev_low + b[index[current_symbol] - 1] * (prev_high - prev_low + 1) / div;
            high = prev_low + b[index[current_symbol]] * (prev_high - prev_low + 1) / div - 1;

            while (true)
            {
                if (high < half)
                {
                    BitsPlusFollow(out, 0, &bits_to_follow);
                }
                else if (low >= half)
                {
                    BitsPlusFollow(out, 1, &bits_to_follow);

                    low -= half;
                    high -= half;
                }
                else if ((low >= first_quarter) && (high < third_quarter))
                {
                    bits_to_follow++;

                    low -= first_quarter;
                    high -= first_quarter;
                }
                else
                {
                    break;
                }

                low += low;
                high += high + 1;
            }

            prev_high = high;
            prev_low = low;
        }
    }

    bits_to_follow++;
    if (low < first_quarter)
    {
        BitsPlusFollow(out, 0, &bits_to_follow);
    }
    else
    {
        BitsPlusFollow(out, 1, &bits_to_follow);
    }


    EndWrite(out);

    fclose(fin);
    fclose(fout);

    free(out);

    high = 0x10000;
    low = 0x0;
    prev_high = 0x10000;
    prev_low = 0x0;
    div = b[unique_bytes_count];
    first_quarter = (high + 1) / 4;
    half = first_quarter * 2;
    third_quarter = first_quarter * 3;
    bits_to_follow = 0;

    unsigned int value = 0u;
    unsigned int frequency = 0u;

    fin = fopen("out.txt", "rb");
    fout = fopen("new.txt", "wb");

    fread(file_size, 1, 4, fin);

    IO_BUFF * in = InitBinaryIO(fin, READ);

    printf("\n");

    value += ByteRead(in) << 8;
    value += ByteRead(in);

    for (int i = 0; i < input_file_len; i++)
    {
        frequency = ((value - prev_low + 1) * div - 1) / (prev_high - prev_low + 1);

        for (current_symbol = 0; b[index[current_symbol]] <= frequency; current_symbol++);

        low = prev_low + b[index[current_symbol] - 1] * (prev_high - prev_low + 1) / div;
        high = prev_low + b[index[current_symbol]] * (prev_high - prev_low + 1) / div - 1;

        while (true)
        {
            if (high < half)
            {
                ;
            }
            else if (low >= half)
            {
                low -= half;
                high -= half;

                value -= half;
            }
            else if ((low >= first_quarter) && (high < third_quarter))
            {
                low -= first_quarter;
                high -= first_quarter;
                value -= first_quarter;
            }
            else
            {
                break;
            }

            low += low;
            high += high + 1;

            value += value + BitRead(in);
        }

        prev_high = high;
        prev_low = low;

        printf("%c", current_symbol);

    }

    fclose(fin);
    fclose(fout);

    free(in);


    // free the shit out of that memory

    return 0;
}

int DecodeFile(
        char * in_file,
        char * out_file)
{
    FILE * fin = fopen(in_file, "rb");
    FILE * fout = fopen(out_file, "wb");

    if (fin == NULL || fout == NULL)
        return -2;

    unsigned char output_buff[BLOCK_SIZE];
    unsigned char file_size[4] = { 0 };

    unsigned int len = 0;
    unsigned int output_pos = 0;
    unsigned int stored_len = 0;
    unsigned int completed_len = 0;

    size_t read;

    int bit;



    fclose(fin);
    fclose(fout);

    return 0;
}


int main(
        int argc,
        char * argv[])
{
    if (argc == 4) {
        if (!strcmp(argv[1], "-e")) {
            EncodeFile(argv[argc - 2], argv[argc - 1]);
        }
        else if (!strcmp(argv[1], "-d")) {
            DecodeFile(argv[argc - 2], argv[argc - 1]);
        }
        else {
            printf("Bad command line arguments");
        }
    }
    else {
        printf("Bad command line arguments");
    }

    return 0;
}