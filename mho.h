#ifndef MHO_H
#define MHO_H

// TODO: we're going to start moving over all of the library functionality we
// currently have (mho_array/shader/etc, mmath, mmdbg, etc) into one big
// library here - mho.h
//
// TODO: Where will <types.h> go?
// TODO: What will we prefix names with?
// TODO: HOW will we name/rename things?
// TODO: Single header with multiple IMPL symbols? Or separate .c files
// containing each section?
// TODO: #define a custom 'EXTERN' symbol for compilation with C++

// NOTE: For now, we will just include <types.h> directly here. Later, we may
// elect to inline them.
// NOTE: For now, we'll include implementation directly within the file. Later,
// we might put implementations into separate .c files, as suggested above.
///////////////////////////////////////////////////////////////////////////////
//
//		TYPES
//

#include <types.h>



///////////////////////////////////////////////////////////////////////////////
//
//		GENERAL
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
    u32 size;       // number of elements in array containing data
    u32 capacity;   // total number of elements
} m_array_header_t;

// Initializes the array
void **m_array_init(void *arr, usize val_size);

// Resizes the array
void *m_array_resize(void *arr, usize sz, usize amt);

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


#endif // MHO_H
