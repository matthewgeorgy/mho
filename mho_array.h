#ifndef MHO_ARRAY_H
#define MHO_ARRAY_H

/*
    Inspired GREATLY by John Jackson's "gs_dyn_array" implementation. Almost
    identical but has a few differences.
*/

#include <types.h>

typedef struct _TAG_m_array_header
{
    u32 size;       // number of elements in array containing data
    u32 capacity;   // total number of elements
} m_array_header_t;

// Wrapper macro
#define m_array(__type) \
    __type *

// Initializes the array
void **m_array_init(void *arr, usize val_size);

// Resizes the array
void *m_array_resize(void *arr, usize sz, usize amt);

// Returns the header of the array
#define m_array_head(__array) \
    ((m_array_header_t *)(__array) - 1)

// Returns the size of the array
#define m_array_size(__array) \
    (__array == NULL ? 0 : m_array_head(__array)->size)

// Returns the capacity of the array
#define m_array_capacity(__array) \
    (__array == NULL ? 0 : m_array_head(__array)->capacity)

// Returns whether the array is full
#define m_array_full(__array) \
    ((m_array_size(__array)) == (m_array_capacity(__array)))

// Returns whether the array is empty
#define m_array_empty(__array) \
    (m_array_init((void **)&(__array), sizeof(*(__array)), (m_array_size(__array) == 0)))

// Frees the array
#define m_array_free(__array) \
    (free(m_array_head(__array))) // tighten with NULL checks

// Returns whether the array needs to grow by a given size
#define m_array_need_grow(__array, __n) \
    ((__array) == 0 || m_array_size(__array) + (__n) > m_array_capacity(__array))

// Grows the array (doubles in size)
#define m_array_grow(__array) \
    m_array_resize((__array), sizeof(*(__array)), m_array_capacity(__array) ? m_array_capacity(__array) * 2 : 1)

// Pushes data to the back of the array
#define m_array_push(__array, __data) \
    do \
    { \
        m_array_init((void **)&(__array), sizeof(*(__array))); \
        if (!(__array) || ((__array) && m_array_need_grow(__array, 1))) \
        { \
            *((void **)&(__array)) = m_array_grow(__array); \
        } \
        (__array)[m_array_size(__array)] = (__data); \
        m_array_head(__array)->size++; \
    } while (0)

// "Pops" data off of the array (decrements the size)
#define m_array_pop(__array) \
    do \
    { \
        if (__array && !m_array_empty(__array)) \
        m_array_head(__array)->size -= 1; \
    } while(0)

// Clears all the elements in the array, (simply sets size to 0)
#define m_array_clear(__array) \
    do \
    { \
        if (__array) \
            m_array_head(__array)->size = 0; \
    } while (0)

/////////////////////////////////
// ====== IMPL =========== //

 #ifdef MHO_ARRAY_IMPL

#include <stdlib.h>

void **
m_array_init(void **arr,
             usize val_size)
{
    m_array_header_t    *data;

    if (*arr == NULL)
    {
        data = (m_array_header_t *)malloc(val_size + sizeof(m_array_header_t));

        if (data)
        {
            data->size = 0;
            data->capacity = 1;
            *arr = ((u32 *)data + 2);

            return arr;
        }
    }

    return NULL;
}

void *
m_array_resize(void *arr,
               usize sz,
               usize amt)
{
    usize               capacity;
    m_array_header_t    *data;

    if (arr)
        capacity = amt;
    else
        capacity = 0;

    // Create new array with header + desired size
    data = (m_array_header_t *)realloc(arr ? m_array_head(arr) : 0, capacity * sz + sizeof(m_array_header_t));

    if (data)
    {
        if (!arr)
            data->size = 0;

        data->capacity = (u32)capacity;
        data = (m_array_header_t *)data + 1;
    }

    return data;
}

 #endif // MHO_ARRAY_IMPL

#endif // MHO_ARRAY_H
