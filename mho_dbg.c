#include "mho.h"

#pragma warning(disable: 4996) // fopen unsafe
#pragma warning(disable: 4477) // %u vs %zu in fprintf (MSVC2015 ONLY)

//---------------------- Memory ---------------------//

#define MHO_MEM_FREE_BIT          0x01
#define MHO_MEM_OVER_BIT          0x02
#define MHO_MEM_UNDER_BIT         0x04
#define MHO_MEM_DOUBLE_FREE_BIT   0x08
#define MHO_MEM_OVER_NUM          0x192BA3A2
#define MHO_MEM_UNDER_NUM         0x39D7A5DA

// Record structure for storing memory information
typedef struct _TAG_mho_dbg_mrec
{
    void                        *ptr;
    char                        *file,
                                *df_file;
    int                         line,
                                df_line;
    usize                       size;
    byte                        flags;
    struct _TAG_mho_dbg_mrec     *next;
} mho_dbg_mrec_t;

global u32                  mho_mem_malloc_cnt;
global u32                  mho_mem_free_cnt;
global usize                mho_mem_total_alloc;
global u32                  mho_fopen_cnt;
global u32                  mho_fclose_cnt;
global mho_dbg_mrec_t   *mho_dbg_mhead = NULL;

void *
mho_dbg_malloc(size_t size,
               char *file,
               int line)
{
    void        *ptr = NULL;
    dword       *buff_un,
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
             char *file,
             int line)
{
    mho_dbg_mrec_t      *temp;
    dword               value;
    void                *p;


    // Set the 'freed' (or 'double freed') flag(s) if necessary
    temp = mho_dbg_mhead;
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
                   char *file,
                   int line,
                   usize size)
{
    mho_dbg_mrec_t      *new_node,
                        *temp;


    // Allocate and fill new node
    new_node = (mho_dbg_mrec_t *)malloc(sizeof(mho_dbg_mrec_t));
    new_node->ptr = ptr;
    new_node->file = (char *)file;
    new_node->line = line;
    new_node->df_file = NULL;
    new_node->df_line = 0;
    new_node->size = size;
    new_node->flags = 0x0;
    new_node->next = NULL;

    // If list is empty
    if (mho_dbg_mhead == NULL)
    {
        mho_dbg_mhead = new_node;
    }

    else
    {
        // Find last node
        temp = mho_dbg_mhead;
        while (temp->next != NULL)
            temp = temp->next;

        // Append
        temp->next = new_node;
    }
}

void
mho_dbg_memory()
{
    mho_dbg_mrec_t      *temp;
    dword               value;
    void                *p;


    temp = mho_dbg_mhead;
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

//---------------------- Files ---------------------//

#define MHO_FILE_CLOSED_BIT       0x01

// Record structure for storing file handle information
typedef struct _TAG_mho_dbg_frec
{
    FILE                            *fptr;
    char                            *file,
                                    *filename,
                                    *mode;
    int                             line;
    byte                            flags;
    struct _TAG_mho_dbg_frec    *next;
} mho_dbg_frec_t;

global mho_dbg_frec_t   *mho_dbg_fhead = NULL;

FILE
*mho_dbg_fopen(char *filename,
               char *mode,
               char *file,
               int line)
{
    FILE        *fptr;


    fptr = fopen(filename, mode);
    if (fptr)
    {
        mho_dbg_file_rec_append(fptr, filename, file, line);
        mho_fopen_cnt++;
    }

    return fptr;
}

void
mho_dbg_fclose(FILE *fptr,
               char *file,
               int line)
{
    mho_dbg_frec_t      *temp;


    temp = mho_dbg_fhead;
    while (temp != NULL)
    {
        if (temp->fptr == fptr)
        {
            temp->flags |= MHO_FILE_CLOSED_BIT;
            break;
        }

        temp = temp->next;
    }

    fclose(fptr);
    mho_fclose_cnt++;
}

void
mho_dbg_file_rec_append(FILE *fptr,
                        char *filename,
                        char *file,
                        int line)
{
    mho_dbg_frec_t      *new_node,
                        *temp;


    new_node = (mho_dbg_frec_t *)malloc(sizeof(mho_dbg_frec_t));
    new_node->fptr = fptr;
    new_node->filename = filename;
    new_node->file = file;
    new_node->line = line;
    new_node->flags = 0x0;
    new_node->next = NULL;
    temp = NULL;

    // If list is empty
    if (mho_dbg_fhead == NULL)
    {
        mho_dbg_fhead = new_node;
    }

    else
    {
        // Find last node
        temp = mho_dbg_fhead;
        while (temp->next != NULL)
            temp = temp->next;

        // Append
        temp->next = new_node;
    }
}

void
mho_dbg_print(FILE *stream)
{
    mho_dbg_mrec_t      *mtemp;
    mho_dbg_frec_t      *ftemp;
    void                *p;


    fprintf(stream, "\n=========================================================\n");
    fprintf(stream, "                  DEBUG REPORT\n");
    fprintf(stream, "=========================================================\n");
    fprintf(stream, "Total Mallocs:         %d\n", mho_mem_malloc_cnt);
    fprintf(stream, "Total Frees:           %d\n", mho_mem_free_cnt);
    fprintf(stream, "Total Size:            %u bytes\n", mho_mem_total_alloc);
    fprintf(stream, "-----\n");
    fprintf(stream, "Total files opened:    %u\n", mho_fopen_cnt);
    fprintf(stream, "Total files closed:    %u\n\n\n", mho_fclose_cnt);

    // Memory info
    mtemp = mho_dbg_mhead;
    mho_dbg_memory();
    while (mtemp != NULL)
    {
        // Memory Leaks
        if (!(mtemp->flags & MHO_MEM_FREE_BIT))
        {
            fprintf(stream, "UNFREED MEMORY:   0x%p (%s [%d])\n", mtemp->ptr,
                                                                  mtemp->file,
                                                                  mtemp->line);
        }

        // Double Frees
        if (mtemp->flags & MHO_MEM_DOUBLE_FREE_BIT)
        {
            fprintf(stream, "DOUBLE FREE:      0x%p (%s [%d])\n", mtemp->ptr,
                                                                  mtemp->df_file,
                                                                  mtemp->df_line);
        }

        // Buffer Underruns
        if (mtemp->flags & MHO_MEM_UNDER_BIT)
        {
            p = (byte *)mtemp->ptr - 4;
            fprintf(stream, "BUFFER UNDERRUN:  0x%p (%s [%d])\n", p,
                                                                  mtemp->file,
                                                                  mtemp->line);
        }

        // Buffer Overruns
        if (mtemp->flags & MHO_MEM_OVER_BIT)
        {
            p = (byte *)mtemp->ptr + mtemp->size;
            fprintf(stream, "BUFFER OVERRUN:   0x%p (%s [%d])\n", p,
                                                                  mtemp->file,
                                                                  mtemp->line);
        }

        mtemp = mtemp->next;
    }
    fprintf(stream, "\n");

    // Files info
    ftemp = mho_dbg_fhead;
    while (ftemp != NULL)
    {
        if (!(ftemp->flags & MHO_FILE_CLOSED_BIT))
        {
            fprintf(stream, "UNCLOSED FILE:    0x%p (%s [%d])\n", ftemp->fptr,
                                                             ftemp->file,
                                                             ftemp->line);
        }

        ftemp = ftemp->next;
    }
    fprintf(stream, "\n");

    fprintf(stream, "=========================================================\n");
    fprintf(stream, "                    END OF REPORT\n");
    fprintf(stream, "=========================================================\n");
}

#pragma warning(default: 4996) // fopen unsafe

