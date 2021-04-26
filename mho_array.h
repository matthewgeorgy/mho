#ifndef MHO_ARRAY_H
#define MHO_ARRAY_H

#include <stdlib.h>
#include <types.h>

typedef struct _TAG_m_array_header
{
    u32 size;       // number of elements in array containing data
    u32 capacity;   // total number of elements
} m_array_header_t;

void **m_array_init(void *arr, usize val_size);
void *m_array_resize(void *arr, usize sz, usize amt);

#define m_array_head(__array) \ 
	((m_array_header_t *)(__array) - 1)

#define m_array_size(__array) \ 
	(__array == NULL ? 0 : m_array_head(__array)->size)

#define m_array_capacity(__array) \ 
	(__array == NULL ? 0 : m_array_head(__array)->capacity)

#define m_array_full(__array) \ 
	((m_array_size(__array)) == (m_array_capacity(__array)))

#define m_array_empty(__array) \
	(m_array_init((void **)&(__array), sizeof(*(__array)), (m_array_size(__array) == 0)))

#define m_array_free(__array) \ 
	(free(m_array_head(__array))) // tighten with NULL checks

#define m_array_need_grow(__array, __n) \
	((__array) == 0 || m_array_size(__array) + (__n) >= m_array_capacity(__array))

#define m_array_grow(__array) \
	m_array_resize((__array), sizeof(*(__array)), m_array_capacity(__array) ? m_array_capacity(__array) * 2 : 1)

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

#define m_array_pop(__array) \
	do \
	{ \
		if (__array && !m_array_empty(__array)) \
		m_array_head(__array)->size -= 1; \
	} while(0)

#define m_array(__type)				__type *

/////////////////////////////////
// ====== IMPL =========== //

 #ifdef MHO_ARRAY_IMPL

void**
m_array_init(void **arr,
             usize val_size)
{
    if (*arr == NULL)
    {
        m_array_header_t *data;
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

void*
m_array_resize(void *arr,
			   usize sz,
			   usize amt)
{
	usize capacity;
	m_array_header_t *data;

	if (arr)
		capacity = amt;
	else
		capacity = 0;

	// Create new array with just the header
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
