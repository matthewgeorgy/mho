#ifndef MHO_ARRAY_H
#define MHO_ARRAY_H

#include <stdlib.h>
#include <types.h>

typedef struct _TAG_m_array_header
{
	u32 size;		// number of elements in array containing data
	u32 capacity;	// total number of elements
} m_array_header_t;

void **m_array_init(void *arr, usize val_size);

#define m_array_head(__array)		((m_array_header_t *)(__array) - 1)
#define m_array_size(__array)		(__array == NULL ? 0 : m_array_head(__array)->size)
#define m_array_capacity(__array)	(__array == NULL ? 0 : m_array_head(__array)->capacity)
#define m_array_full(__array)		((m_array_size(__array)) == (m_array_capacity(__array)))
#define m_array_free(__array)		(free(m_array_head(__array))) // tighten with NULL checks

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

 #endif // MHO_ARRAY_IMPL

#endif // MHO_ARRAY_H
