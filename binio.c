#include "binio.h"


#ifdef FORCE_INLINE
force_inline
#else
inline
#endif
void BitWrite(
        IO_BUFF * out,
        int bit)
{
#ifdef DEBUG_BIT_OUTPUT
    printf("%d", bit);
#endif
    out->string[out->byte_pos] += ((unsigned int) bit & 0x01u) << (7u - out->bit_pos++);

    if (out->bit_pos > 7) {
        out->bit_pos = 0;

        if (out->byte_pos == BLOCK_SIZE - 1) {
            fwrite(out->string, 1, BLOCK_SIZE, out->file);
            out->byte_pos = 0;

            memset(out->string, 0, BLOCK_SIZE);
            return;
        }
        out->byte_pos++;
    }
}

#ifdef FORCE_INLINE
force_inline
#else
inline
#endif
int BitRead(
        IO_BUFF * in)
{
    if (in->bit_pos > 7) {
        in->byte_pos++;
        in->bit_pos = 0;
    }

    if (in->byte_pos == BLOCK_SIZE) {
        in->read = fread(in->string, 1, BLOCK_SIZE, in->file);

        in->byte_pos = 0;
        in->bit_pos = 0;
    }
#ifdef DEBUG_BIT_OUTPUT
    printf("%d", ((in->string[in->byte_pos]) >> (7 - in->bit_pos))  & 0x01);
#endif

    if (in->byte_pos >= in->read)
    {
        in->garbage_bits++;

        if (in->garbage_bits > REG_BITS - 2)
        {
            in->data_corrupted = true;
        }
    }

    return (((unsigned int) in->string[in->byte_pos]) >> (7u - in->bit_pos++))  & 0x01u;
}


inline void EndWrite(
        IO_BUFF * out)
{
    fwrite(out->string, 1, out->byte_pos + (size_t)(out->bit_pos ? 1 : 0), out->file);
}


IO_BUFF * InitBinaryIO(
        FILE * file,
        bool mode)
{
    IO_BUFF * new_buff = (IO_BUFF *) malloc(sizeof(IO_BUFF));

    if (new_buff == NULL)
    {
        return NULL;
    }

    new_buff->string = (unsigned char *) calloc(1, BLOCK_SIZE);

    if (new_buff->string == NULL)
    {
        free(new_buff);
        return NULL;
    }

    new_buff->file = file;
    new_buff->data_corrupted = false;
    new_buff->byte_pos = mode;
    new_buff->bit_pos = 0;

    return new_buff;
}
