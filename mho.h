// Mho is a general utility library, consisting of C functions that can be
// useful for any type of development. It's a single header library, so all
// that's required is to include it in your project directory and #define
// and implementation symbol.
//
// Mho should ideally be the last file that is #included in your project, as
// it contains some definitions (ie, TRUE/FALSE) that other libraries should
// have precedence for, as well as requiring other headers/files to be
// included for certain functionality (ie, <glad.h>).

#ifndef MHO_H
#define MHO_H

// TODO: we're going to start moving over all of the library functionality we
// currently have (mho_array/shader/etc, mmath, mmdbg, etc) into one big
// library here - mho.h
//
// TODO: What will we prefix names with?
// TODO: add mmath.h
// TODO: add mmdbg.h
// TODO: HOW will we name/rename things?
// TODO: move decls to top, defs to bottom*****

// NOTE: For now, we will just directly inline our <types.h>
// NOTE: For now, we'll include implementation directly within the file. Later,
// we might put implementations into separate .c files, as suggested above.
///////////////////////////////////////////////////////////////////////////////
//
//		INCLUDES
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



///////////////////////////////////////////////////////////////////////////////
//
//		TYPES
//

 #ifndef TYPES_H

// Defs for static
#define local       static
#define global      static
#define internal    static

// Unsigned
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef size_t      usize;

typedef u8          byte;
typedef u16         word;
typedef u32         dword;
typedef u64         qword;

// Signed
typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

// Floating point
typedef float       f32;
typedef double      f64;

// Boolean
typedef s8          b8;
typedef s32         b32;

// Max values
#define U8_MAX      UINT8_MAX
#define U16_MAX     UINT16_MAX
#define U32_MAX     UINT32_MAX
#define U64_MAX     UINT64_MAX

#define S8_MAX      INT8_MAX
#define S16_MAX     INT16_MAX
#define S32_MAX     INT32_MAX
#define S64_MAX     INT64_MAX

 #endif // TYPES_H



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
#endif // FALSE

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

#define m_swap(__a, __b, __type) 		\
	do 									\
	{ 									\
		__type __M_SWAP_TEMP = __a; 	\
		__a = __b; 						\
		__b = __M_SWAP_TEMP; 			\
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
#define m_array_push(__array, __data)										\
	do 																		\
	{ 																		\
		m_array_init((void **)&(__array), sizeof(*(__array))); 				\
		if (!(__array) || ((__array) && m_array_need_grow(__array, 1))) 	\
		{ 																	\
			*((void **)&(__array)) = m_array_grow(__array); 				\
		} 																	\
		(__array)[m_array_size(__array)] = (__data); 						\
		m_array_head(__array)->size++; 										\
	} while (0)

// "Pops" data off of the array (decrements the size)
#define m_array_pop(__array) 													\
	do 																			\
	{ 																			\
		if (__array && !m_array_empty(__array) && m_array_size(__array) != 0) 	\
			m_array_head(__array)->size -= 1; 									\
	} while (0)

// Clears all the elements in the array, (simply sets size to 0)
#define m_array_clear(__array) 					\
	do 											\
	{ 											\
		if (__array) 							\
			m_array_head(__array)->size = 0; 	\
	} while (0)

// Initializes the array
MHO_EXTERN void **m_array_init(void *arr, usize val_size);

// Resizes the array
MHO_EXTERN void *m_array_resize(void *arr, usize sz, usize amt);



///////////////////////////////////////////////////////////////////////////////
//
//		UTIL
//

MHO_EXTERN s8 *m_read_file_buffer(const char *filename);



///////////////////////////////////////////////////////////////////////////////
//
//		OPENGL SHADERS
//

MHO_EXTERN u32 m_load_shader_vf(const char *vs_path, const char *fs_path);
MHO_EXTERN u32 m_load_shader_comp(const char *cs_path);
// 0 = source, 1 = program
MHO_EXTERN void m_check_compile_errors(u32 data, u8 type, const char *filename);



///////////////////////////////////////////////////////////////////////////////
//																			 //
// ---------------------- MHO IMPLEMENTATION ------------------------------- //
//																			 //
///////////////////////////////////////////////////////////////////////////////

// #define this symbol in exactly 1 .c/.cpp file
// before including this file!
// For example:
//          #include <...>
//          #include <...>
//          #include <...>
//
//          #define MHO_IMPL
//          #include <mho.h>

 #ifdef MHO_IMPL

//////////////////////////////////////////////////////////////////
// Dynamic Array

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



//////////////////////////////////////////////////////////////////
// Utils

s8 *
m_read_file_buffer(const char *filename)
{
    FILE    *fptr;
    s32     file_len,
            ret;
    s8    	*source = NULL;

    fptr = fopen(filename, "rb");
    if (fptr)
    {
        fseek(fptr, 0, SEEK_END);
        file_len = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
        if (file_len != -1)
        {
            source = (s8 *)malloc(file_len + 1);
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



//////////////////////////////////////////////////////////////////
// OpenGL Shaders

// glad.h is the OpenGL function loader that I primarily use; this can
// be easily swapped by just replacing this symbol with your respective
// loader's symbol in the IMPLEMENTATION section.
  #ifdef __glad_h_

u32
m_load_shader_vf(const char *vs_path,
                 const char *fs_path)
{
    u32     vertex,
            fragment,
            program;
    s8    	*source;

    // Vertex Shader
    source = m_read_file_buffer(vs_path);
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &source, NULL);
    glCompileShader(vertex);
    m_check_compile_errors(vertex, 0, vs_path);
    free(source);

    // Fragment Shader
    source = m_read_file_buffer(fs_path);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &source, NULL);
    glCompileShader(fragment);
    m_check_compile_errors(fragment, 0, fs_path);
    free(source);

    // Shader Program
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    m_check_compile_errors(program, 1, NULL);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

u32
m_load_shader_comp(const char *cs_path)
{
    u32     compute,
            program;
    s8    	*source;

    // Compute Shader
    source = m_read_file_buffer(cs_path);
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &source, NULL);
    glCompileShader(compute);
    m_check_compile_errors(compute, 0, cs_path);
    free(source);

    // Shader Program
    program = glCreateProgram();
    glAttachShader(program, compute);
    glLinkProgram(program);
    m_check_compile_errors(program, 1, NULL);

    glDeleteShader(compute);

    return program;
}

void
m_check_compile_errors(u32 data,
                       u8 type,
                       const char *filename)
{
    b8		success;
    s8		info_log[1024];

    if (type == 1) // program
    {
        glGetProgramiv(data, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(data, 1024, NULL, info_log);
            printf("PROGRAM LINKING ERROR OF TYPE:\n %s\n\n", info_log);
        }
    }
    else if (type == 0) // source
    {
        glGetShaderiv(data, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(data, 1024, NULL, info_log);
            printf("SHADER COMPILATION ERROR OF TYPE: %s\n %s\n\n", filename, info_log);
        }
    }
    else
    {
        printf("INVALID SHADER TYPE\n");
    }
}

  #endif // __glad_h_

 #endif // MHO_IMPL

#endif // MHO_H
