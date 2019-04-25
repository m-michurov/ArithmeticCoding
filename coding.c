#include "coding.h"


static int char_to_index[NO_OF_BYTES];
static int frequency[NO_OF_SYMBOLS + 1] = { 0 };
static int cumulative_frequency[NO_OF_SYMBOLS + 1] = { 0 };

static unsigned int index_to_char[NO_OF_BYTES + 2];


static void reset_frequencies(
        void)
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


static force_inline void update_frequencies(
        int symbol)
{
    static unsigned int ch_i;
    static unsigned int ch_symbol;
    static unsigned int accumulated;

    static int * c_pointer;

    accumulated = 0;

    static int i;

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

    c_pointer = cumulative_frequency + i;

    while (c_pointer > cumulative_frequency)
    {
        (*(--c_pointer))++;
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

    printf("\n");

    if (total > MAX_FILE_SIZE)
    {
        return FILE_SIZE_ERROR;
    }

    return total;
}


int encode(
        char * in_file,
        char * out_file)
{
    register int byte_pos = 0;
    register int bit_pos = 0;
    register int bits_to_follow = 0u;

    register int high = TOP_VALUE;
    register int low = 0u;
    register int range;

    register int current_symbol;

    unsigned char * buff;
    unsigned char * bit_buff;
    unsigned char file_size[4] = { 0 };

    unsigned int encoded_len = 0u;
    unsigned int input_file_len;

    size_t buff_len = 0;

    FILE * fin;
    FILE * fout;

    fin = fopen(in_file, "rb");
    fout = fopen(out_file, "wb");

    if (fin == NULL)
    {
        return IN_FILE_ERROR;
    }

    if (fout == NULL)
    {
        return OUT_FILE_ERROR;
    }

    buff = (unsigned char *) malloc(BLOCK_SIZE);
    bit_buff = (unsigned char *) calloc(BLOCK_SIZE, 1);

    if (buff == NULL || bit_buff == NULL)
    {
        free(buff);
        free(bit_buff);
        return ALLOCATION_ERROR;
    }

    input_file_len = (unsigned int) count_file_length(fin, buff);

    if (input_file_len == (unsigned int) FILE_SIZE_ERROR)
    {
        return FILE_SIZE_ERROR;
    }

    reset_frequencies();

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

            high = low + cumulative_frequency[current_symbol - 1] * range / cumulative_frequency[0] - 1;
            low = low + cumulative_frequency[current_symbol] * range / cumulative_frequency[0];

            while (true)
            {
                if (high < HALF)
                {
                    BitsPlusFollow0(bit_pos, byte_pos, bit_buff);
                }
                else if (low >= HALF)
                {
                    BitsPlusFollow1(bit_pos, byte_pos, bit_buff);

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

            update_frequencies(current_symbol);
        }

        encoded_len += buff_len;
#ifdef PERCENTAGE_OUTPUT
        printf("\r\tEncoding: %.2lf %%", (double) encoded_len / input_file_len * 100);
#endif
    }

    bits_to_follow++;
    if (low < FIRST_QUARTER)
    {
        BitsPlusFollow0(bit_pos, byte_pos, bit_buff);
    }
    else
    {
        BitsPlusFollow1(bit_pos, byte_pos, bit_buff);
    }

#ifdef PERCENTAGE_OUTPUT
    printf("\n");
#endif

    fwrite(bit_buff, 1, (size_t) byte_pos + (bit_pos ? 1 : 0), fout);

    fclose(fin);
    fclose(fout);

    free(buff);
    free(bit_buff);

    return 0;
}


int decode(
        char * in_file,
        char * out_file)
{
    register int byte_pos = 0;
    register int bit_pos = 0;

    register int high = TOP_VALUE;
    register int low = 0u;
    register int range;
    register int value = 0u;
    register int frequency;

    register int current_symbol;

    unsigned char * buff;
    unsigned char * bit_buff;
    unsigned char file_size[4] = { 0 };

    unsigned int decoded = 0u;
    unsigned int out_pos = 0u;
    unsigned int input_file_len;
    /*unsigned int read;*/

    int * c_pointer;

    FILE * fin;
    FILE * fout;

    fin = fopen(in_file, "rb");
    fout = fopen(out_file, "wb");

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

    bit_buff = (unsigned char *) calloc(BLOCK_SIZE, 1);

    if (buff == NULL || bit_buff == NULL)
    {
        free(buff);
        free(bit_buff);
        return ALLOCATION_ERROR;
    }

    reset_frequencies();

    /*read = */fread(bit_buff, 1, BLOCK_SIZE, fin);

    for (int k = 0; k < REG_BITS; k++)
    {
        value += value + ((bit_buff[byte_pos] >> (7u - bit_pos++)) & 0x01);

        if (bit_pos > 7)
        {
            bit_pos = 0;
            byte_pos++;
        }
    }

    for (int f = 0; f < input_file_len; f++)
    {
        range = (high - low + 1);

        frequency = ((value - low + 1) * cumulative_frequency[0] - 1) / range;

        for (current_symbol = 1, c_pointer = cumulative_frequency + 1; *c_pointer > frequency; current_symbol++, c_pointer++);

        high = low + *(c_pointer - 1) * range / cumulative_frequency[0] - 1;
        low = low + *c_pointer * range / cumulative_frequency[0];

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

            value += value + ((bit_buff[byte_pos] >> (7u - bit_pos++)) & 0x01);

            if (bit_pos > 7)
            {
                bit_pos = 0;
                byte_pos++;

                if (byte_pos == BLOCK_SIZE)
                {
                    byte_pos = 0;
/*
                    if (read == 0)
                    {
                        free(buff);
                        free(bit_buff);

                        fclose(fin);
                        fclose(fout);
                    }
*/
                    /*read = */fread(bit_buff, 1, BLOCK_SIZE, fin);
                }
            }
        }

        buff[out_pos++] = (unsigned char) index_to_char[current_symbol];

        if (out_pos == BLOCK_SIZE)
        {
            fwrite(buff, 1, BLOCK_SIZE, fout);
            out_pos = 0;
#ifdef PERCENTAGE_OUTPUT
            decoded += BLOCK_SIZE;

            printf("\r\tDecoding: %.2lf %%", (double) decoded / input_file_len * 100);
#endif
        }

        update_frequencies(current_symbol);
    }

    fwrite(buff, 1, out_pos, fout);

#ifdef PERCENTAGE_OUTPUT
    printf("\r\tDecoding: %.2lf %%", (double) 100);

    printf("\n");
#endif

    fclose(fin);
    fclose(fout);

    return 0;
}