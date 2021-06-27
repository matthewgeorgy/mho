#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strchr
#include "mho.h"

// Important #define constants we use
#define MHO_MEM_TRUE              1
#define MHO_MEM_FALSE             0
#define MHO_MEM_FREE_BIT          0x01
#define MHO_MEM_OVER_BIT          0x02
#define MHO_MEM_UNDER_BIT         0x04
#define MHO_MEM_DOUBLE_FREE_BIT   0x08
#define MHO_MEM_OVER_NUM          0x192BA3A2
#define MHO_MEM_UNDER_NUM         0x39D7A5DA

// Required globals
global u32              mho_mem_malloc_cnt;
global u32              mho_mem_free_cnt;
global usize            mho_mem_total_alloc;
global mho_mem_rec_t    *mho_mem_alloc_head = NULL;

void *
mho_mem_malloc(size_t size,
               const char *file,
               int line)
{
    void		*ptr = NULL;
    dword   	*buff_un,
            	*buff_ov;

    buff_un = (dword *)malloc(size + 2 * sizeof(dword));

    // If allocation succeeded
    if (buff_un)
    {
		mho_mem_rec_t	temp;

        ptr = buff_un + 1;
        buff_ov = (dword *)((byte *)ptr + size);
        *buff_ov = MHO_MEM_OVER_NUM;
        *buff_un = MHO_MEM_UNDER_NUM;

        mho_mem_malloc_cnt++;
        mho_mem_total_alloc += size;

		// Store record
		temp.ptr = ptr;
		temp.file = file;
		temp.line = line;
		temp.size = size;
        mho_arr_push(mho_mem_alloc_head, temp);
    }

    // Return ptr regardless
    return ptr;
}

void
mho_mem_free(void *buffer,
             const char *file,
             int line)
{
    dword		value;
    void        *p;
	u32			i;

    for (i = 0; i < mho_arr_size(mho_mem_alloc_head); i++)
    {
        if (mho_mem_alloc_head[i].ptr == buffer)
        {
			// If it hasn't been freed, set the 'freed' bit
            if (!(mho_mem_alloc_head[i].flags & MHO_MEM_FREE_BIT))
            {
                mho_mem_alloc_head[i].flags |= MHO_MEM_FREE_BIT;
            }
			// If it has already been freed, set the 'double free' bit and fill
			// out the file/line # information
            else if (mho_mem_alloc_head[i].flags & MHO_MEM_FREE_BIT)
            {
                mho_mem_alloc_head[i].flags |= MHO_MEM_DOUBLE_FREE_BIT;
                mho_mem_alloc_head[i].df_file = (char *)file;
                mho_mem_alloc_head[i].df_line = line;
            }

            break;
        }
    }

    // After (potentially) setting the 'double freed' flag, proceed to
    // check for any buffer runs and then free the ptr.
    if (!(mho_mem_alloc_head[i].flags & MHO_MEM_DOUBLE_FREE_BIT))
    {
        // Check for overrun
        p = (byte *)buffer + mho_mem_alloc_head[i].size;
        value = *(dword *)p;
        if (value != MHO_MEM_OVER_NUM)
        {
            mho_mem_alloc_head[i].flags |= MHO_MEM_OVER_BIT;
        }

        // Check for underrun
        p = (byte *)buffer - 4;
        value = *(dword *)p;
        if (value != MHO_MEM_UNDER_NUM)
        {
            mho_mem_alloc_head[i].flags |= MHO_MEM_UNDER_BIT;
        }

        mho_mem_free_cnt++;

        // free the buffer
        free((dword *)buffer - 1);
    }
}

void
mho_mem_debug_memory()
{
    dword		value;
    void        *p;
	u32			i;

    for (i = 0; i < mho_arr_size(mho_mem_alloc_head); i++)
    {
        if (!(mho_mem_alloc_head[i].flags && MHO_MEM_FREE_BIT))
        {
            // Find overruns
            p = (byte *)mho_mem_alloc_head[i].ptr + mho_mem_alloc_head[i].size;
            value = *(dword *)p;
			// Set the flag if there was
            if (value != MHO_MEM_OVER_NUM)
            {
                mho_mem_alloc_head[i].flags |= MHO_MEM_OVER_BIT;
            }

            // Find underruns
            p = (byte *)mho_mem_alloc_head[i].ptr - 4;
            value = *(dword *)p;
			// Set the flag if there was
            if (value != MHO_MEM_UNDER_NUM)
            {
                mho_mem_alloc_head[i].flags |= MHO_MEM_UNDER_BIT;
            }
        }
    }
}

void
mho_mem_print(FILE *stream)
{
    void		*p;
	u32			i;

    mho_mem_debug_memory();

    fprintf(stream, "\n=========================================================\n");
    fprintf(stream, "                    DEBUG REPORT\n");
    fprintf(stream, "=========================================================\n");
    fprintf(stream, "Total Mallocs: %d\n", mho_mem_malloc_cnt);
    fprintf(stream, "Total Frees:   %d\n", mho_mem_free_cnt);
    fprintf(stream, "Total Size:    %zu bytes\n\n", mho_mem_total_alloc);

    // Print out all debug-related info
    for (i = 0; i < mho_arr_size(mho_mem_alloc_head); i++)
    {
        // Memory Leaks
        if (!(mho_mem_alloc_head[i].flags & MHO_MEM_FREE_BIT))
        {
            fprintf(stream, "UNFREED MEMORY:   0x%p (%s (%d))\n", mho_mem_alloc_head[i].ptr,
                                                                  mho_mem_alloc_head[i].file,
                                                                  mho_mem_alloc_head[i].line);
        }

        // Double Frees
        if (mho_mem_alloc_head[i].flags & MHO_MEM_DOUBLE_FREE_BIT)
        {
            fprintf(stream, "DOUBLE FREE:      0x%p (%s (%d))\n", mho_mem_alloc_head[i].ptr,
                                                                  mho_mem_alloc_head[i].df_file,
                                                                  mho_mem_alloc_head[i].df_line);
        }

        // Buffer Underruns
        if (mho_mem_alloc_head[i].flags & MHO_MEM_UNDER_BIT)
        {
            p = (byte *)mho_mem_alloc_head[i].ptr - 4;
            fprintf(stream, "BUFFER UNDERRUN:  0x%p (%s (%d))\n", p,
                                                                  mho_mem_alloc_head[i].file,
                                                                  mho_mem_alloc_head[i].line);
        }

        // Buffer Overruns
        if (mho_mem_alloc_head[i].flags & MHO_MEM_OVER_BIT)
        {
            p = (byte *)mho_mem_alloc_head[i].ptr + mho_mem_alloc_head[i].size;
            fprintf(stream, "BUFFER OVERRUN:   0x%p (%s (%d))\n", p,
                                                                  mho_mem_alloc_head[i].file,
                                                                  mho_mem_alloc_head[i].line);
        }
    }

    fprintf(stream, "=========================================================\n");
    fprintf(stream, "                    END OF REPORT\n");
    fprintf(stream, "=========================================================\n");
}
