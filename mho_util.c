#include <stdio.h>
#include <stdlib.h>
#include "mho.h"

// TODO: handle errors (ie, fptr is invalid)

#pragma warning(disable: 4996) // fopen unsafe

//-------------------- FILES ----------------//

char *
mho_file_read(const char *filename,
              usize byte_cnt)
{
    char        *buffer;
    FILE        *fptr;
    usize       bytes_read;


    fptr = fopen(filename, "rb");
    if (fptr)
    {
        if (byte_cnt == 0)
            byte_cnt = (usize)mho_file_lenf(fptr);

        if (byte_cnt != -1)
        {
            buffer = (char *)malloc(byte_cnt + 1);
            if (buffer)
            {
                bytes_read = fread(buffer, 1, byte_cnt, fptr);
                if (bytes_read == byte_cnt)
                {
                    buffer[byte_cnt] = 0;
                }
                else
                {
                    fprintf(stderr, "Could not read full file: %s\n", filename);
                    buffer = NULL;
                }
            }
            else
            {
                fprintf(stderr, "Could not allocate %u bytes for buffer for file: %s\n", byte_cnt, filename);
                buffer = NULL;
            }
        }
        else
        {
            fprintf(stderr, "Could not read file length: %s\n", filename);
            buffer = NULL;
        }
    }
    else
    {
        fprintf(stderr, "Could not load file: %s\n", filename);
        buffer = NULL;
    }
    fclose(fptr);

    return buffer;
}

b32
mho_file_write(const char *filename,
               char *buffer,
               usize byte_cnt)
{
    FILE        *fptr;
    b32         ret = 1;
    usize       bytes_written;


    fptr = fopen(filename, "wb");
    if (fptr)
    {
        if (byte_cnt == 0)
            byte_cnt = mho_strlen(buffer);

        bytes_written = fwrite(buffer, 1, byte_cnt, fptr);
        if (bytes_written != byte_cnt)
        {
            fprintf(stderr, "Could not write %u bytes to file: %s (wrote %u / %u)", byte_cnt, filename, bytes_written, byte_cnt);
            ret = -1;
        }
    }
    else
    {
        fprintf(stderr, "Could not load file: %s\n", filename);
        ret = -1;
    }
    fclose(fptr);

    return ret;
}

b32
mho_file_append(const char *filename,
                char *buffer,
                usize byte_cnt)
{
    FILE        *fptr;
    b32         ret = 1;
    usize       bytes_written;


    fptr = fopen(filename, "ab");
    if (fptr)
    {
        if (byte_cnt == 0)
            byte_cnt = mho_strlen(buffer);

        bytes_written = fwrite(buffer, 1, byte_cnt, fptr);
        if (bytes_written != byte_cnt)
        {
            fprintf(stderr, "Could not write %u bytes to file: %s (wrote %u / %u)", byte_cnt, filename, bytes_written, byte_cnt);
            ret = -1;
        }
    }
    else
    {
        fprintf(stderr, "Could not load file: %s\n", filename);
        ret = -1;
    }
    fclose(fptr);

    return ret;
}

long
mho_file_len(const char *filename)
{
    long        len = -1;
    FILE        *fptr;


    fptr = fopen(filename, "rb");
    if (fptr)
    {
        fseek(fptr, 0, SEEK_END);
        len = ftell(fptr);
    }
    fclose(fptr);

    return len;
}


u32
mho_file_lines(const char *filename)
{
    u32         ln_cnt = 0;
    FILE        *fptr;
    s32         c;


    fptr = fopen(filename, "rb");
    if (fptr)
    {
        do
        {
            c = fgetc(fptr);
            if (c == '\n')
                ln_cnt++;
        } while (c != EOF);
    }

    return ln_cnt;
}

char *
mho_file_readf(FILE *file,
               usize byte_cnt)
{
    long        pos;
    char        *buffer = NULL;
    usize       bytes_read;


    pos = ftell(file);
    if (byte_cnt == 0)
        byte_cnt = (usize)mho_file_lenf(file);

    if (byte_cnt != -1)
    {
        buffer = (char *)malloc(byte_cnt + 1);
        if (buffer)
        {
            bytes_read = fread(buffer, 1, byte_cnt, file);
            if (bytes_read == byte_cnt)
            {
                buffer[byte_cnt] = 0;
            }
        }
    }
    fseek(file, pos, SEEK_SET);

    return buffer;
}

b32
mho_file_writef(FILE *file,
                char *buffer,
                usize byte_cnt)
{
    long        pos;
    b32         ret = 1;
    usize       bytes_written;

    pos = ftell(file);
    if (byte_cnt == 0)
        byte_cnt = mho_strlen(buffer);

    bytes_written = fwrite(buffer, 1, byte_cnt, file);
    if (bytes_written != byte_cnt)
    {
        fprintf(stderr, "failed to write full bytes!");
        ret = -1;
    }
    fseek(file, pos, SEEK_SET);

    return ret;
}

b32
mho_file_appendf(FILE *file,
                 char *buffer,
                 usize byte_cnt)
{
    long        pos;
    b32         ret = 1;
    usize       bytes_written;

    pos = ftell(file);
    if (byte_cnt == 0)
        byte_cnt = mho_strlen(buffer);

    bytes_written = fwrite(buffer, 1, byte_cnt, file);
    if (bytes_written != byte_cnt)
    {
        fprintf(stderr, "failed to append bytes!\n");
        ret = -1;
    }
    fseek(file, pos, SEEK_SET);

    return ret;
}

long
mho_file_lenf(FILE *fp)
{
    long        len,
                pos;


    pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, pos, SEEK_SET);

    return len;
}

u32
mho_file_linesf(FILE *file)
{
    long        pos;
    u32         ln_cnt = 0;
    s32         c;


    pos = ftell(file);
    do
    {
        c = fgetc(file);
        if (c == '\n')
            ln_cnt++;
    } while (c != EOF);
    fseek(file, pos, SEEK_SET);

    return ln_cnt;
}

//-------------------- STDLIB ----------------//

void
mho_memcpy(void *dest,
           void *src,
           usize n)
{
    usize       i;


    for (i = 0; i < n; i++)
    {
        ((s8 *)dest)[i] = ((s8 *)src)[i];
    }
}

void *
mho_memset(void *dest,
           s32 c,
           usize n)
{
    u8          uc = (u8)c;
    usize       i;


    for (i = 0; i < n; i++)
    {
        ((u8 *)dest)[i] = uc;
    }

    return dest;
}

void
mho_strcpy(s8 *dest,
           s8 *src)
{
    while (*dest != 0)
    {
        *dest++ = *src++;
    }
}

void
mho_strncpy(s8 *dest,
            s8 *src,
            usize n)
{
    usize       i;


    for (i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }
}

usize
mho_strlen(s8 *str)
{
    usize       len = 0;


    while (*str++ != '\0')
    {
        len++;
    }

    return len;
}

s8 *
mho_strcat(s8 *str1,
           s8 *str2)
{
    u32         size1,
                size2,
                index,
                i;
    s8          *new_str;


    for (size1 = 0; str1[size1] != '\0'; size1++);
    for (size2 = 0; str2[size2] != '\0'; size2++);

    new_str = (s8 *)malloc(size1 + size2);

    for (index = 0; index < size1; index++)
        new_str[index] = str1[index];
    for (i = 0; index < (size1 + size2); index++, i++)
        new_str[index] = str2[i];

    return new_str;
}



#pragma warning(default: 4996) // fopen unsafe

