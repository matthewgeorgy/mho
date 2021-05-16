// Mho is a general utility library, consisting of C functions that can be
// useful for any type of development. It's a single header library, so all
// that's required is to include it in your project directory and #define
// and implementation symbol.
//
// Mho should ideally be the last file that is #included in your project, as
// it contains some definitions (ie, TRUE/FALSE) that other libraries should
// have precedence for.

#ifndef MHO_H
#define MHO_H

// TODO: we're going to start moving over all of the library functionality we
// currently have (mho_array/shader/etc, mmath, mmdbg, etc) into one big
// library here - mho.h
//
// TODO: Where will <types.h> go?
// TODO: What will we prefix names with?
// TODO: HOW will we name/rename things?
// TODO: align the \ in #define's

// NOTE: For now, we will just directly inline our <ypes.h>
// NOTE: For now, we'll include implementation directly within the file. Later,
// we might put implementations into separate .c files, as suggested above.
///////////////////////////////////////////////////////////////////////////////
//
//		INCLUDES
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



///////////////////////////////////////////////////////////////////////////////
//
//		GENERAL
//

// Defines NULL in C/C++ if not already defined (should be since stdlib.h is
// already included above)
#ifndef NULL
	#ifdef __cplusplus
		#define NULL	0
	#else
		#define NULL	((void *)0)
	#endif // __cplusplus
#endif // NULL

// For C vs. C++ compilation
#ifdef __cplusplus
    #define MHO_EXTERN		extern "C"
#else
    #define MHO_EXTERN    	extern
#endif

// TRUE/FALSE
#ifndef TRUE
	#define TRUE	1
#endif // TRUE
#ifndef FALSE
	#define FALSE	0
#endif // TRUE

// Useful macro to strip just the filename out of the full filepath.
#if defined(_WIN32) || defined(_WIN64)
    #define __FILENAME__    (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #define __FILENAME__    (strrchr(__FILE__, '/ ') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif



///////////////////////////////////////////////////////////////////////////////
//
//		MISC
//

#define m_lerp(__a, __b, __t) \
	((__a) + (f32)((__b) - (__a)) * (__t))

#define m_clamp(__x, __min, __max) \
	((__x) < (__min) ? (__min) : (__x) > (__max) ? (__max) : (__x))

#define m_swap(__a, __b, __type) \
	do \
	{ \
		__type __M_SWAP_TEMP = __a; \
		__a = __b; \
		__b = __M_SWAP_TEMP; \
	} while (0)


///////////////////////////////////////////////////////////////////////////////
//
//		DYNAMIC ARRAY
//

typedef struct _TAG_m_array_header
{
	u32 size;		// number of elements in array containing data
	u32 capacity;	// total number of elements
} m_array_header_t;

// Initializes the array
MHO_EXTERN void **m_array_init(void *arr, usize val_size);

// Resizes the array
MHO_EXTERN void *m_array_resize(void *arr, usize sz, usize amt);

// Wrapper macro
#define m_array(__type) \
	__type *

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
	(free(m_array_head(__array)))

// Returns whether the array needs to grow by a given size
#define m_array_need_grow(__array, __n) \
	((__array) == 0 || m_array_size(__array) + (__n) > m_array_capacity(__array))

// Grows the array (doubles in size)
#define m_array_grow(__array) \
	m_array_resize((__array), sizeof(*(__array)), m_array_capacity(__array) ? m_array_capacity(__array) * 2 : 1)

// Grows the array by a given size
#define m_array_grow_size(__array, __sz) \
	m_array_resize((__array), __sz, m_array_capacity(__array) ? m_array_capacity(__array) * 2 : 1)

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
		if (__array && !m_array_empty(__array) && m_array_size(__array) != 0) \
			m_array_head(__array)->size -= 1; \
	} while (0)

// Clears all the elements in the array, (simply sets size to 0)
#define m_array_clear(__array) \
	do \
	{ \
		if (__array) \
			m_array_head(__array)->size = 0; \
	} while (0)

#ifdef MHO_IMPL

void **
m_array_init(void **arr,
			 usize val_size)
{
	m_array_header_t	*data;

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
	usize				capacity;
	m_array_header_t	*data;

	if (arr)
		capacity = amt;
	else
		capacity = 0;

	// Create new array with header + desired size (or 0 if arr is empty)
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

#endif // MHO_IMPL

#endif // MHO_H
