#include "binio.h"

inline void BitWrite(
        IO_BUFF * out,
        int bit)
{
#ifdef DEBUG_BIT_OUTPUT
    printf("%d", bit);
#endif
    out->string[out->byte_pos] += (bit & 0x01) << (7 - out->bit_pos++);

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

inline int BitRead(
        IO_BUFF * in)
{
    if (in->bit_pos > 7) {
        in->byte_pos++;
        in->bit_pos = 0;
    }

    if (in->byte_pos == BLOCK_SIZE) {
        fread(in->string, 1, BLOCK_SIZE, in->file);

        in->byte_pos = 0;
        in->bit_pos = 0;
    }
#ifdef DEBUG_BIT_OUTPUT
    printf("%d", ((in->string[in->byte_pos]) >> (7 - in->bit_pos))  & 0x01);
#endif

    return ((in->string[in->byte_pos]) >> (7 - in->bit_pos++))  & 0x01;
}

inline void ByteWrite(
        IO_BUFF *out,
        unsigned char byte)
{
    for (int k = 7; k >= 0; k--)
        BitWrite(out, (byte >> k) & 0x01);
}

inline unsigned char ByteRead(
        IO_BUFF * in)
{
    unsigned char byte = 0;

    for (int k = 7; k >= 0; k--)
        byte += (BitRead(in) << k);

    return byte;
}

inline void EndWrite(
        IO_BUFF * out)
{
    fwrite(out->string, 1, out->byte_pos + (size_t)(out->bit_pos ? 1 : 0), out->file);
}

inline void NextByte(
        IO_BUFF *buff)
{
    buff->byte_pos += buff->bit_pos ? 1 : 0;
    buff->bit_pos = 0;
}

IO_BUFF * InitBinaryIO(
        FILE * file,
        bool mode)
{
    IO_BUFF * new_buff = (IO_BUFF *) malloc(sizeof(IO_BUFF));

    if (new_buff == NULL)
        return NULL;

    memset(new_buff->string, 0, BLOCK_SIZE);

    new_buff->file = file;
    new_buff->byte_pos = mode;
    new_buff->bit_pos = 0;

    return new_buff;
}
