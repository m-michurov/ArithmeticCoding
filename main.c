#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_OF_DIFFERENT_BYTES  256 // 2^8 do not change
#define BLOCK_SIZE              9
#define MAX_FILE_SIZE           4294966500


typedef struct {
    unsigned char data;
    double high;
    double low;
    double frequency;
} CharData;


int count_freqs(
        FILE * in,
        CharData ** data_array,
        unsigned int * len)
{
    unsigned char input_buff[BLOCK_SIZE];

    unsigned int freqs[NUM_OF_DIFFERENT_BYTES] = { 0 };
    unsigned int read = 0;
    unsigned int pos = 0;

    unsigned long long total = 0;

    //double prev_low = 0.0;
    double prev_high = 0.0;

    while ((read = fread(input_buff, 1, BLOCK_SIZE, in))) {
        while (pos < read)
            freqs[(int) input_buff[pos++]]++;

        pos = 0;
        total += read;
    }

    if (total > MAX_FILE_SIZE)
        return -1;

    *len = (unsigned int) total;

    for (int k = 0; k < NUM_OF_DIFFERENT_BYTES; k++) {
        if (freqs[k] != 0) {
            data_array[k] = calloc(1, sizeof(CharData));
            // test for null
            data_array[k]->frequency = (double) freqs[k] / total;
            data_array[k]->low = prev_high;
            data_array[k]->high = data_array[k]->low + data_array[k]->frequency;

            prev_high = data_array[k]->high;
        }
    }

    return 0;
}


void print_binary(
        double result)
{
    unsigned char null_count = 0;
    unsigned char bit_count = 0;

    unsigned long long * p = (unsigned long long *)&result;

    for ( ; (((*p) >> (null_count)) & 0x01) != 1 && null_count < 62; null_count++);

    bit_count = (unsigned char)62 - null_count;

    printf("%.50lf - ", result);

    for (int k = 7; k >= 0; k--) {
        printf("%u", (bit_count >> k) & 0x01);
    }

    printf(" - ");

    for (int k = 61; k >= null_count; k--) {
        printf("%llu", ((*p) >> k) & 0x01);
    }

    printf("\n");
}


int find_byte(
        double num,
        CharData ** data)
{
    for (int k = 0; k < NUM_OF_DIFFERENT_BYTES; k++)
        if (data[k] && num >= data[k]->low && num < data[k]->high)
            return k;

    return -1;
}


int decode_double(
        double num,
        CharData ** data)
{
    unsigned char current_symbol = 0;
    double current_range;

    for (int k = 0; k < BLOCK_SIZE; k++) {
        current_symbol = (unsigned char) find_byte(num, data);
        printf("%c", current_symbol);
        current_range = data[current_symbol]->high - data[current_symbol]->low;
        num -= data[current_symbol]->low;
        num /= current_range;
    }

    return 0;
}


int EncodeFile(
        char * in_file,
        char * out_file)
{
    FILE * fin = fopen(in_file, "rb");
    FILE * fout = fopen(out_file, "wb");

    if (fin == NULL || fout == NULL)
        return -2;

    unsigned char input_buff[BLOCK_SIZE];
    unsigned char file_size[4] = { 0 };

    size_t buff_len = 0;
    size_t buff_pos = 0;

    unsigned int input_file_len = 0;

    CharData * data[256] = { NULL };

    double low;
    double high;

    double code_range;

    count_freqs(fin, data, &input_file_len);
    // check the result

    fseek(fin, 0, 0);

    // This ensures independence from unsigned int size
    // on different platforms
    file_size[0] = (unsigned char) ((input_file_len >> 24) & 0xFF);
    file_size[1] = (unsigned char) ((input_file_len >> 16) & 0xFF);
    file_size[2] = (unsigned char) ((input_file_len >> 8) & 0xFF);
    file_size[3] = (unsigned char) ((input_file_len) & 0xFF);

    for (int k = 0; k < (input_file_len + BLOCK_SIZE - 1) / BLOCK_SIZE; k++)
    {
        buff_len = (size_t) fread(input_buff, 1, BLOCK_SIZE, fin);

        low = 0.0;
        high = 1.0;

        for (buff_pos = 0; buff_pos < buff_len; buff_pos++) {
            code_range = high - low;
            high = low + code_range * data[input_buff[buff_pos]]->high;
            low = low + code_range * data[input_buff[buff_pos]]->low;
        }

        //print_binary(low);
        decode_double(low, data);
    }

    fclose(fin);
    fclose(fout);

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