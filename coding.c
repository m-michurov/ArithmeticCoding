#include "coding.h"


#define PERCENTAGE_OUTPUT


static void reset_frequencies(
        unsigned int * frequency,
        unsigned int * cumulative_frequency,
        int * char_to_index,
        unsigned int * index_to_char)
{
    unsigned int k;

    for (k = 0; k < NO_OF_BYTES; k++)
    {
        char_to_index[k] = k + 1;
        index_to_char[k + 1] = k;
    }

    for (k = 0; k <= NO_OF_SYMBOLS; k++)
    {
        frequency[k] = 1;
        cumulative_frequency[k] = NO_OF_SYMBOLS - k;
    }

    frequency[0] = 0u;
}

static
#ifdef FORCE_INLINE
force_inline
#else
inline
#endif
void update_frequencies(
        int symbol,
        unsigned int * frequency,
        unsigned int * cumulative_frequency,
        int * char_to_index,
        unsigned int * index_to_char)
{
    unsigned int ch_i;
    unsigned int ch_symbol;
    unsigned int accumulated = 0;

    int i;

    if (cumulative_frequency[0] == MAX_FREQUENCY)
    {
        for (int k = NO_OF_SYMBOLS; k >= 0; k--)
        {
            frequency[k] = (frequency[k] + 1) / 2;

            cumulative_frequency[k] = accumulated;
            accumulated += frequency[k];
        }
    }

    for (i = symbol; frequency[i] == frequency[i - 1]; i--);

    if (i < symbol)
    {
        ch_i = index_to_char[i];
        ch_symbol = index_to_char[symbol];

        index_to_char[i] = ch_symbol;
        index_to_char[symbol] = ch_i;

        char_to_index[ch_i] = symbol;
        char_to_index[ch_symbol] = i;
    }

    frequency[i]++;

    while (i > 0)
    {
        i--;
        cumulative_frequency[i]++;
    }
}


static long long int count_file_length(
        FILE *in,
        unsigned char *input_buff)
{
    size_t read = 0;

    unsigned long long total = 0;

    while ((read = fread(input_buff, 1, BLOCK_SIZE, in)))
    {
        total += read;

        printf("\r\tReading file: %llu %s   ",
               total > 1023 ? (total > 1048575 ? total / 1048576 : total / 1024) : total,
               total > 1023 ? (total > 1048575 ? "MiB" : "KiB") : "B");
    }

    printf("\r\n\tDone reading!\n");

    if (total > MAX_FILE_SIZE)
    {
        return FILE_SIZE_ERROR;
    }

    return total;
}


static
#ifdef FORCE_INLINE
force_inline
#else
inline
#endif
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


int encode(
        char * in_file,
        char * out_file)
{
    //register unsigned int byte_pos;
    //register unsigned int bit_pos;

    register unsigned int high = TOP_VALUE;
    register unsigned int low = 0u;
    register unsigned int range;

    unsigned char * buff;
    unsigned char file_size[4] = { 0 };

    unsigned int frequencies[NO_OF_SYMBOLS + 1] = { 0 };
    unsigned int cumulative_frequencies[NO_OF_SYMBOLS + 1] = { 0 };
    unsigned int index_to_char[NO_OF_BYTES + 2];
    unsigned int encoded_len = 0u;
    unsigned int bits_to_follow = 0u;
    unsigned int input_file_len;


    int char_to_index[NO_OF_BYTES];
    int current_symbol;

    size_t buff_len = 0;

    FILE * fin;
    FILE * fout;

    fin = fopen(in_file, "rb");
    fout = fopen(out_file, "wb");

    IO_BUFF * out = InitBinaryIO(fout, WRITE);

    if (fin == NULL)
    {
        return IN_FILE_ERROR;
    }

    if (fout == NULL)
    {
        return OUT_FILE_ERROR;
    }

    buff = (unsigned char *) malloc(BLOCK_SIZE);

    if (buff == NULL || out == NULL)
    {
        return ALLOCATION_ERROR;
    }

    input_file_len = (unsigned int) count_file_length(fin, buff);

    if (input_file_len == (unsigned int) FILE_SIZE_ERROR)
    {
        return FILE_SIZE_ERROR;
    }

    reset_frequencies(frequencies, cumulative_frequencies, char_to_index, index_to_char);

    fseek(fin, 0, 0);

    file_size[0] = (unsigned char) ((input_file_len >> 24u) & 0xFFu);
    file_size[1] = (unsigned char) ((input_file_len >> 16u) & 0xFFu);
    file_size[2] = (unsigned char) ((input_file_len >> 8u) & 0xFFu);
    file_size[3] = (unsigned char) ((input_file_len) & 0xFFu);

    fwrite(file_size, 1, 4, fout);

    while ((buff_len = fread(buff, 1, BLOCK_SIZE, fin)) > 0)
    {
        for (int k = 0; k < buff_len; k++)
        {
            current_symbol = char_to_index[buff[k]];

            range = high - low + 1;

            high = low + cumulative_frequencies[current_symbol - 1] * range / cumulative_frequencies[0] - 1;
            low = low + cumulative_frequencies[current_symbol] * range / cumulative_frequencies[0];

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

            update_frequencies(current_symbol, frequencies, cumulative_frequencies, char_to_index, index_to_char);
        }

        encoded_len += buff_len;
#ifdef PERCENTAGE_OUTPUT
        printf("\r\tEncoding: %.2lf %%", (double) encoded_len / input_file_len * 100);
#endif
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

#ifdef PERCENTAGE_OUTPUT
    printf("\n");
#endif

    EndWrite(out);

    fclose(fin);
    fclose(fout);

    free(buff);
    free(out->string);
    free(out);

    return 0;
}


int decode(
        char * in_file,
        char * out_file)
{
    register unsigned int high = TOP_VALUE;
    register unsigned int low = 0u;
    register unsigned int range;
    register unsigned int value = 0u;
    register unsigned int frequency;

    unsigned char * buff;
    unsigned char file_size[4] = { 0 };

    unsigned int frequencies[NO_OF_SYMBOLS + 1] = { 0 };
    unsigned int cumulative_frequencies[NO_OF_SYMBOLS + 1] = { 0 };
    unsigned int index_to_char[NO_OF_BYTES + 2];
    unsigned int decoded = 0u;
    unsigned int out_pos = 0u;
    unsigned int input_file_len;

    int char_to_index[NO_OF_BYTES];
    int current_symbol;

    FILE * fin;
    FILE * fout;

    fin = fopen(in_file, "rb");
    fout = fopen(out_file, "wb");

    IO_BUFF * in = InitBinaryIO(fin, READ);

    if (fin == NULL)
    {
        return IN_FILE_ERROR;
    }

    if (fout == NULL)
    {
        return OUT_FILE_ERROR;
    }

    fread(file_size, 1, 4, fin);

    input_file_len = ((unsigned int) file_size[0] << 24u)
                   + ((unsigned int) file_size[1] << 16u)
                   + ((unsigned int) file_size[2] << 8u)
                   +  (unsigned int) file_size[3];

    if (input_file_len > MAX_FILE_SIZE)
    {
        return DATA_CORRUPTION_ERROR;
    }

    buff = (unsigned char *) malloc(BLOCK_SIZE);

    if (buff == NULL || in == NULL)
    {
        return ALLOCATION_ERROR;
    }

    reset_frequencies(frequencies, cumulative_frequencies, char_to_index, index_to_char);

    for (int k = 0; k < REG_BITS; k++)
    {
        value += value + BitRead(in);
    }

    for (int f = 0; f < input_file_len; f++)
    {
        range = (high - low + 1);

        frequency = ((value - low + 1) * cumulative_frequencies[0] - 1) / range;

        for (current_symbol = 1; cumulative_frequencies[current_symbol] > frequency; current_symbol++);

        high = low + cumulative_frequencies[current_symbol - 1] * range / cumulative_frequencies[0] - 1;
        low = low + cumulative_frequencies[current_symbol] * range / cumulative_frequencies[0];

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
/*
            if (in->data_corrupted == true)
            {
                fclose(fin);
                fclose(fout);

                free(in->string);
                free(in);

                return DATA_CORRUPTION_ERROR;
            }
            */
        }

        buff[out_pos++] = (unsigned char) index_to_char[current_symbol];

        if (out_pos == BLOCK_SIZE)
        {
            fwrite(buff, 1, BLOCK_SIZE, fout);
            out_pos = 0;
            decoded += BLOCK_SIZE;

#ifdef PERCENTAGE_OUTPUT
            printf("\r\tDecoding: %.2lf %%", (double) decoded / input_file_len * 100);
#endif
        }

        update_frequencies(current_symbol, frequencies, cumulative_frequencies, char_to_index, index_to_char);
    }

    fwrite(buff, 1, out_pos, fout);

#ifdef PERCENTAGE_OUTPUT
    printf("\r\tDecoding: %.2lf %%", (double) 100);

    printf("\n");
#endif

    fclose(fin);
    fclose(fout);

    free(in->string);
    free(in);

    return 0;
}