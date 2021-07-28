#include <stdio.h>
#include <stdlib.h>
#include "mho.h"

#pragma warning(disable: 4996) // fopen unsafe

char *
mho_read_file_buffer(const char *filename)
{
    FILE        *fptr;
    long        file_len;
    usize       ret;
    char        *source = NULL;


    fptr = fopen(filename, "rb");
    if (fptr)
    {
        file_len = mho_filelen(fptr);
        if (file_len != -1)
        {
            source = (char *)malloc(file_len + 1);
            if (source)
            {
                ret = fread(source, 1, file_len, fptr);
                if (ret == file_len)
                {
                    source[file_len] = 0;
                }
                else
                {
                    printf("Could not read full file: %s\n", filename);
                }
            }
        }
        else
        {
            printf("Could not read file length: %s\n", filename);
        }
    }
    else
    {
        printf("Could not load file: %s\n", filename);
    }
    fclose(fptr);

    return source;
}

long
mho_filelen(FILE *fp)
{
    long        len,
                pos;


    pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, pos, SEEK_SET);

    return len;
}

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

