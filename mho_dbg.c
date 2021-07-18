#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strchr
#include "mho.h"

// Important #define constants we use
#define MHO_MEM_FREE_BIT          0x01
#define MHO_MEM_OVER_BIT          0x02
#define MHO_MEM_UNDER_BIT         0x04
#define MHO_MEM_DOUBLE_FREE_BIT   0x08
#define MHO_MEM_OVER_NUM          0x192BA3A2
#define MHO_MEM_UNDER_NUM         0x39D7A5DA

// Required globals
global u32              	mho_mem_malloc_cnt;
global u32              	mho_mem_free_cnt;
global usize            	mho_mem_total_alloc;
global mho_dbg_mem_rec_t    *mho_mem_alloc_head = NULL;
global mho_dbg_file_rec_t	*mho_file_head = NULL;

void *
mho_dbg_malloc(size_t size,
               const char *file,
               int line)
{
    void    *ptr = NULL;
    dword   *buff_un,
            *buff_ov;

    buff_un = (dword *)malloc(size + 2 * sizeof(dword));

    // If allocation succeeded
    if (buff_un)
    {
        ptr = buff_un + 1;
        buff_ov = (dword *)((byte *)ptr + size);
        *buff_ov = MHO_MEM_OVER_NUM;
        *buff_un = MHO_MEM_UNDER_NUM;

        mho_mem_malloc_cnt++;
        mho_mem_total_alloc += size;
        mho_dbg_mem_rec_append(ptr, file, line, size);
    }

    // Return ptr regardless
    return ptr;
}

void
mho_dbg_free(void *buffer,
             const char *file,
             int line)
{
    mho_dbg_mem_rec_t   *temp;
    dword           	value;
    void            	*p;

    // Set the 'freed' (or 'double freed') flag(s) if necessary
    temp = mho_mem_alloc_head;
    while (temp != NULL)
    {
        if (temp->ptr == buffer)
        {
            if (!(temp->flags & MHO_MEM_FREE_BIT))
            {
                temp->flags |= MHO_MEM_FREE_BIT;
            }
            else if (temp->flags & MHO_MEM_FREE_BIT)
            {
                temp->flags |= MHO_MEM_DOUBLE_FREE_BIT;
                temp->df_file = (char *)file;
                temp->df_line = line;
            }

            break;
        }

        temp = temp->next;
    }

    // After (potentially) setting the 'double freed' flag, proceed to
    // check for any buffer runs and then free the ptr.
    if (!(temp->flags & MHO_MEM_DOUBLE_FREE_BIT))
    {
        // Check for overrun
        p = (byte *)buffer + temp->size;
        value = *(dword *)p;
        if (value != MHO_MEM_OVER_NUM)
        {
            temp->flags |= MHO_MEM_OVER_BIT;
        }

        // Check for underrun
        p = (byte *)buffer - 4;
        value = *(dword *)p;
        if (value != MHO_MEM_UNDER_NUM)
        {
            temp->flags |= MHO_MEM_UNDER_BIT;
        }

        mho_mem_free_cnt++;

        // free the buffer
        free((dword *)buffer - 1);
    }
}

void
mho_dbg_mem_rec_append(void *ptr,
                   const char *file,
                   int line,
                   int size)
{
    mho_dbg_mem_rec_t   *new_node,
                    	*temp;

    // Allocate and fill new node
    new_node = (mho_dbg_mem_rec_t *)malloc(sizeof(mho_dbg_mem_rec_t));
    new_node->ptr = ptr;
    new_node->file = (char *)file;
    new_node->line = line;
    new_node->df_file = NULL;
    new_node->df_line = 0;
    new_node->size = size;
    new_node->flags = 0x0;
    new_node->next = NULL;

    // If list is empty
    if (mho_mem_alloc_head == NULL)
    {
        mho_mem_alloc_head = new_node;
    }

    else
    {
        // Find last node
        temp = mho_mem_alloc_head;
        while (temp->next != NULL)
            temp = temp->next;

        // Append
        temp->next = new_node;
    }
}

void
mho_mem_debug_memory()
{
    mho_dbg_mem_rec_t   *temp;
    dword           value;
    void            *p;

    temp = mho_mem_alloc_head;
    while (temp != NULL)
    {
        if (!(temp->flags && MHO_MEM_FREE_BIT))
        {
            // Find overruns
            p = (byte *)temp->ptr + temp->size;
            value = *(dword *)p;
            if (value != MHO_MEM_OVER_NUM)
            {
                temp->flags |= MHO_MEM_OVER_BIT;
            }

            // Find underruns
            p = (byte *)temp->ptr - 4;
            value = *(dword *)p;
            if (value != MHO_MEM_UNDER_NUM)
            {
                temp->flags |= MHO_MEM_UNDER_BIT;
            }
        }

        temp = temp->next;
    }
}


FILE
*mho_dbg_fopen(char *filename,
			   char *mode,
			   char *file,
			   int line)
{
	FILE		*fptr;

	fptr = fopen(filename, mode);
	if (fptr)
	{
		mho_dbg_file_rec_append(filename, mode, file, line);
	}

	return fptr;
}

void
mho_dbg_fclose(FILE *fptr,
			   char *file,
			   int line)
{

}

void
mho_dbg_file_rec_append(char *filename,
						char *mode,
						char *file,
						int line)
{
	mho_dbg_file_rec_t	*new_node,
						*temp;

	new_node = (mho_dbg_file_rec_t *)malloc(sizeof(mho_dbg_file_rec_t));
	new_node->filename = filename;
	new_node->file = file;
	new_node->mode = mode;
	new_node->line = line;

    // If list is empty
    if (mho_file_head == NULL)
    {
        mho_file_head = new_node;
    }

    else
    {
        // Find last node
        temp = mho_file_head;
        while (temp->next != NULL)
            temp = temp->next;

        // Append
        temp->next = new_node;
    }
}

void
mho_mem_print(FILE *stream)
{
    mho_dbg_mem_rec_t   *temp;
    void            	*p;

    mho_mem_debug_memory();

    fprintf(stream, "\n=========================================================\n");
    fprintf(stream, "                    DEBUG REPORT\n");
    fprintf(stream, "=========================================================\n");
    fprintf(stream, "Total Mallocs: %d\n", mho_mem_malloc_cnt);
    fprintf(stream, "Total Frees:   %d\n", mho_mem_free_cnt);
    fprintf(stream, "Total Size:    %u bytes\n\n", mho_mem_total_alloc);

    // Print out all debug-related info
    temp = mho_mem_alloc_head;
    while (temp != NULL)
    {
        // Memory Leaks
        if (!(temp->flags & MHO_MEM_FREE_BIT))
        {
            fprintf(stream, "UNFREED MEMORY:   0x%p (%s (%d))\n", temp->ptr,
                                                                  temp->file,
                                                                  temp->line);
        }

        // Double Frees
        if (temp->flags & MHO_MEM_DOUBLE_FREE_BIT)
        {
            fprintf(stream, "DOUBLE FREE:      0x%p (%s (%d))\n", temp->ptr,
                                                                  temp->df_file,
                                                                  temp->df_line);
        }

        // Buffer Underruns
        if (temp->flags & MHO_MEM_UNDER_BIT)
        {
            p = (byte *)temp->ptr - 4;
            fprintf(stream, "BUFFER UNDERRUN:  0x%p (%s (%d))\n", p,
                                                                  temp->file,
                                                                  temp->line);
        }

        // Buffer Overruns
        if (temp->flags & MHO_MEM_OVER_BIT)
        {
            p = (byte *)temp->ptr + temp->size;
            fprintf(stream, "BUFFER OVERRUN:   0x%p (%s (%d))\n", p,
                                                                  temp->file,
                                                                  temp->line);
        }

        temp = temp->next;
    }

    fprintf(stream, "=========================================================\n");
    fprintf(stream, "                    END OF REPORT\n");
    fprintf(stream, "=========================================================\n");
}
