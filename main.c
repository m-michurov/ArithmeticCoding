#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#include "binio.h"

// http://compression.ru/download/ar.html


//#define DEBUG_DATA_OUTPUT

#define REG_BITS        16

#define MAX_FILE_SIZE   4294966500
#define NO_OF_BYTES     256 // 2^8 do not change

#define TOP_VALUE       ((1u << REG_BITS) - 1)
#define FIRST_QUARTER   (TOP_VALUE / 4 + 1)
#define HALF            (FIRST_QUARTER * 2)
#define THIRD_QUARTER   (FIRST_QUARTER * 3)

#define MAX_FREQUENCY   (TOP_VALUE / 4)


static inline void scale_frequencies(
        unsigned int * frequency,
        unsigned int * cumulative_frequency)
{
    for (int k = 0; k < NO_OF_BYTES; k++)
    {
        frequency[k] = (frequency[k] + 1) / 2;

        cumulative_frequency[k] = cumulative_frequency[k - 1] + frequency[k];
    }
}


long long int count_freqs(
        FILE * in,
        unsigned int * frequency,
        unsigned int * cumulative_frequency,
        unsigned char * input_buff)
{
    unsigned int read = 0;
    unsigned int pos = 0;

    unsigned long long total = 0;


    while ((read = fread(input_buff, 1, BLOCK_SIZE, in)))
    {
        while (pos < read)
        {
            if (cumulative_frequency[NO_OF_BYTES - 1] >= MAX_FREQUENCY)
            {
                scale_frequencies(frequency, cumulative_frequency);
            }

            frequency[(int) input_buff[pos]]++;

            for (int k = input_buff[pos]; k < NO_OF_BYTES; k++)
            {
                cumulative_frequency[k]++;
            }

            pos++;
        }

        pos = 0;
        total += read;

        printf("\rReading file: %llu %s   ",
                total > 1023 ? (total > 1048575 ? total / 1048576 : total / 1024) : total,
                 total > 1023 ? (total > 1048575 ? "MiB" : "KiB") : "B");
    }

    printf("\r\nDone reading!\n");

    if (total > MAX_FILE_SIZE)
        return -1;

    return total;
}


static inline void reset_frequencies(
        unsigned int *frequency,
        unsigned int *cumulative_frequency)
{
    for (unsigned int k = 0; k < NO_OF_BYTES; k++)
    {
        frequency[k] = 1;
        cumulative_frequency[k] = k + 1;
    }

    frequency[0] = 0u;
}


static inline void update_frequencies(
        int symbol,
        unsigned int * frequency,
        unsigned int * cumulative_frequency)
{
    if (cumulative_frequency[NO_OF_BYTES - 1] >= MAX_FREQUENCY)
    {
        scale_frequencies(frequency, cumulative_frequency);
    }

    frequency[symbol]++;

    for (int k = symbol; k < NO_OF_BYTES; k++)
    {
        cumulative_frequency[k]++;
    }
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
    unsigned char * buff = (unsigned char *) malloc(BLOCK_SIZE);

    if (buff == NULL)
    {
        printf("Unable to allocate %llu bytes", (unsigned long long int) BLOCK_SIZE);
        return -1;
    }
    //unsigned char output_buff[BLOCK_SIZE];
    unsigned char file_size[4] = { 0 };

    unsigned int out_pos = 0u;
    unsigned int frequencies[NO_OF_BYTES] = { 0 };
    unsigned int input_file_len = 0u;
    unsigned int encoded_len = 0u;

    unsigned int * c_f = calloc(NO_OF_BYTES + 1, sizeof(unsigned int));
    unsigned int * cumulative_frequencies = c_f + 1;

    size_t buff_len = 0;

    FILE * fin;
    FILE * fout;

    fin = fopen(in_file, "rb");
    fout = fopen(out_file, "wb");

    if (fin == NULL || fout == NULL)
        return -2;

    clock_t count = clock();

    input_file_len = (unsigned int) count_freqs(fin, frequencies, cumulative_frequencies, buff);
    // check the result

    //reset_frequencies(frequencies, cumulative_frequencies);

    fseek(fin, 0, 0);

    // This ensures independence from unsigned int size
    // on different platforms
    file_size[0] = (unsigned char) ((input_file_len >> 24) & 0xFF);
    file_size[1] = (unsigned char) ((input_file_len >> 16) & 0xFF);
    file_size[2] = (unsigned char) ((input_file_len >> 8) & 0xFF);
    file_size[3] = (unsigned char) ((input_file_len) & 0xFF);


    fwrite(file_size, 1, 4, fout);

    //printf("%ld", (((long) 1 << 16) - 1));

#ifdef OLD_FREQ_COUNT
    for (int k = 0; k < NO_OF_BYTES; k++)
    {
        if (frequencies[k] > 0)
        {
            unique_bytes_count++;
            input_file_len += frequencies[k];
            cumulative_frequencies[k] = cumulative_frequencies[k - 1] + frequencies[k];
#ifdef DEBUG_DATA_OUTPUT
            printf("index %d, symbol = %c, frequency = %d, cumulative_frequencies[%d] = %d\n", unique_bytes_count, k, frequencies[k], unique_bytes_count, cumulative_frequencies[unique_bytes_count]);
#endif
        }
    }
#endif

    // i'm so sorry but i'll move declarations i promise

    unsigned int high = TOP_VALUE;
    unsigned int low = 0u;
    unsigned int prev_high = TOP_VALUE;
    unsigned int prev_low = 0x0;
    unsigned int div = cumulative_frequencies[NO_OF_BYTES - 1];
    unsigned int bits_to_follow = 0u;

    unsigned char current_symbol;

    IO_BUFF * out = InitBinaryIO(fout, WRITE);


    // Start encoding

    clock_t start = clock();

    while ((buff_len = fread(buff, 1, BLOCK_SIZE, fin)) > 0)
    {
        for (int k = 0; k < buff_len; k++)
        {
            current_symbol = buff[k];

            low = prev_low + cumulative_frequencies[current_symbol - 1] * (prev_high - prev_low + 1) / div;
            high = prev_low + cumulative_frequencies[current_symbol] * (prev_high - prev_low + 1) / div - 1;

            while (true)
            {
                if (high < HALF)
                {
                    BitsPlusFollow(out, 0, &bits_to_follow);
                }
                else if (low >= HALF)
                {
                    BitsPlusFollow(out, 1, &bits_to_follow);

                    low -= HALF;
                    high -= HALF;
                }
                else if ((low >= FIRST_QUARTER) && (high < THIRD_QUARTER))
                {
                    bits_to_follow++;

                    low -= FIRST_QUARTER;
                    high -= FIRST_QUARTER;
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
#ifdef AD
            for (int i = index[current_symbol]; i < unique_bytes_count + 1; i++)
            {
                cumulative_frequencies[i]++;
            }
#endif
            //update_frequencies(current_symbol, frequencies, cumulative_frequencies);
        }

        encoded_len += buff_len;
        printf("\rEncoding: %.2lf %%", (double) encoded_len / input_file_len * 100);
    }

    bits_to_follow++;
    if (low < FIRST_QUARTER)
    {
        BitsPlusFollow(out, 0, &bits_to_follow);
    }
    else
    {
        BitsPlusFollow(out, 1, &bits_to_follow);
    }

    // End encoding

    printf("\r\nDone encoding!\n");

    clock_t mid = clock();


    EndWrite(out);

    fclose(fin);
    fclose(fout);

    free(out->string);
    free(out);

    prev_high = TOP_VALUE;
    prev_low = 0u;
    div = cumulative_frequencies[NO_OF_BYTES - 1];
    bits_to_follow = 0u;

    unsigned int value = 0u;
    unsigned int frequency;
    unsigned int decoded = 0u;

    fin = fopen(out_file, "rb");
    fout = fopen("new.bin", "wb");

    //reset_frequencies(frequencies, cumulative_frequencies);

    fread(file_size, 1, 4, fin);
#ifdef AD
    for (int l = 1; l < unique_bytes_count +1; l++)
    {
        cumulative_frequencies[l] = cumulative_frequencies[l - 1] + 1;
    }
#endif

    IO_BUFF * in = InitBinaryIO(fin, READ);

    #ifdef DEBUD_DATA_OUTPUT
    printf("\n");
#endif
    // Start decoding with same frequency table (dunno how to write it into file yet)

    for (int k = 0; k < REG_BITS; k++)
    {
        value += value + BitRead(in);
    }

    for (int f = 0; f < input_file_len; f++)
    {
        frequency = ((value - prev_low + 1) * div - 1) / (prev_high - prev_low + 1);

        for (current_symbol = 0; cumulative_frequencies[current_symbol] <= frequency; current_symbol++);

        low = prev_low + cumulative_frequencies[current_symbol - 1] * (prev_high - prev_low + 1) / div;
        high = prev_low + cumulative_frequencies[current_symbol] * (prev_high - prev_low + 1) / div - 1;

        while (true)
        {
            if (high < HALF)
            {
                ;
            }
            else if (low >= HALF)
            {
                low -= HALF;
                high -= HALF;

                value -= HALF;
            }
            else if ((low >= FIRST_QUARTER) && (high < THIRD_QUARTER))
            {
                low -= FIRST_QUARTER;
                high -= FIRST_QUARTER;
                value -= FIRST_QUARTER;
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

#ifdef DEBUG_DATA_OUTPUT
        printf("%c", current_symbol);
#endif

        buff[out_pos++] = current_symbol;

        if (out_pos == BLOCK_SIZE)
        {
            fwrite(buff, 1, BLOCK_SIZE, fout);
            out_pos = 0;
            decoded += BLOCK_SIZE;
            printf("\rDecoding: %.2lf %%", (double) decoded / input_file_len * 100);
        }
#ifdef AD
        for (int i = index[current_symbol]; i < unique_bytes_count + 1; i++)
        {
            cumulative_frequencies[i]++;
        }
#endif
       //update_frequencies(current_symbol, frequencies, cumulative_frequencies);


    }

    fwrite(buff, 1, out_pos, fout);
    printf("\rDecoding: %.2lf %%", (double) 100);


    // End decoding

    printf("\r\nDone decoding!\n");

    clock_t end = clock();

    printf("Counting frequencies took %.2lf s, encoding took %.2lf s, decoding took %.2lf s\n",
           (double) (start - count) / 1000,  (double) (mid - start) / 1000, (double) (end - mid) / 1000);

    fclose(fin);
    fclose(fout);

    free(in->string);
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