// mho is a general utility library, consisting of C functions that can be
// useful for any type of development. It's a single header library, so all
// that's required is to include it in your project directory and #define
// and implementation symbol.
//
// mho should ideally be the last file that is #included in your project, as
// it contains some definitions (ie, TRUE/FALSE) that other libraries should
// have precedence for, as well as requiring other headers/files to be
// included for certain functionality (ie, <glad.h>).

#ifndef MHO_H
#define MHO_H

// TODO: we're going to start moving over all of the library functionality we
// currently have (mho_array/shader/etc, mmath, m_mem, etc) into one big
// library here - mho.h
//
// TODO: What will we prefix names with?
// TODO: HOW will we name/rename things?
// TODO: move decls to top, defs to bottom*****

// NOTE: For now, we will just directly inline our <types.h>
// NOTE: For now, we'll include implementation directly within the file. Later,
// we might put implementations into separate .c files, as suggested above.
// NOTE: For now, we won't overwrite the func/type names for the math section.
// This can be done later and it's not a concern currently.
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
//
//		MATH
//

// Useful #define macros + constants
#define m_sqrt(__n)   	sqrtf(__n)
#define m_sin(__n)    	sinf(__n)
#define m_cos(__n)    	cosf(__n)
#define m_tan(__n)    	tanf(__n)
#define m_cast(__n)   	(f32 *)&__n
#define m_rads(__n)   	(__n * 0.017453f)
#define m_degs(__n)   	(__n * 57.29578f)
#ifndef M_PI
	#define M_PI		3.141592653589f
#endif // M_PI

/* ============================ *
 * =====     Vector2D     ===== *
 * ============================ */

// Definition of a 2D vector
typedef struct _TAG_vec2
{
    union
    {
        struct
        {
            f32 x,
                y;
        };
        f32 elements[2];
    };
} vec2_t;

// Constructs a 2D vector given x, y components
MHO_EXTERN vec2_t      vec2_ctor(f32 x, f32 y);

// Computes the sum of two 2D vectors
// (v1 + v2)
MHO_EXTERN vec2_t      vec2_add(vec2_t v1, vec2_t v2);

// Computes the difference of two 2D vectors
// (v1 - v2)
MHO_EXTERN vec2_t      vec2_sub(vec2_t v1, vec2_t v2);

// Computes a 2D vector multiplied by a scalar
// (vec * scalar)
MHO_EXTERN vec2_t      vec2_scal(vec2_t vec, f32 scalar);

// Computes the dot product of two 2D vectors
// (v1 . v2)
MHO_EXTERN f32         vec2_dot(vec2_t v1, vec2_t v2);

// Computes the magnitude (norm) of a 2D vector
// (||vec||)
MHO_EXTERN f32         vec2_mag(vec2_t vec);

// Normalizes a given 2D vector
// (vec * (1 / ||vec||))
MHO_EXTERN vec2_t      vec2_normalize(vec2_t vec);

// Computes a 2D vector rotated by an angle in degrees
MHO_EXTERN vec2_t      vec2_rotate(vec2_t vec, f32 angle);

/* ============================ *
 * =====     Vector3D     ===== *
 * ============================ */

// Definition of a 3D vector
typedef struct _TAG_vec3
{
    union
    {
        struct
        {
            f32 x,
                y,
                z;
        };
        f32 elements[3];
    };
} vec3_t;

// Constructs a 3D vector given x, y, z components
MHO_EXTERN vec3_t      vec3_ctor(f32 x, f32 y, f32 z);

// Computes the sum of two 3D vectors
// (v1 + v2)
MHO_EXTERN vec3_t      vec3_add(vec3_t v1, vec3_t v2);

// Computes the difference of two 3D vectors
// (v1 - v2)
MHO_EXTERN vec3_t      vec3_sub(vec3_t v1, vec3_t v2);

// Computes a 3D vector multipled by a scalar
// (vec * scalar)
MHO_EXTERN vec3_t      vec3_scal(vec3_t vec, f32 scalar);

// Computes the dot product of two 3D vectors
// (v1 . v2)
MHO_EXTERN f32         vec3_dot(vec3_t v1, vec3_t v2);

// Computes the cross product of two 3D vectors
// (v1 x v2)
MHO_EXTERN vec3_t      vec3_cross(vec3_t v1, vec3_t v2);

// Computes the magnitude (norm) of a 3D vector
// (||vec||)
MHO_EXTERN f32         vec3_mag(vec3_t vec);

// Normalizes a given 3D vector
// (vec * (1 / ||vec||))
MHO_EXTERN vec3_t      vec3_normalize(vec3_t vec);

/* ============================ *
 * =====     Vector4D     ===== *
 * ============================ */

// Definition of a 4D vector
typedef struct _TAG_vec4
{
    union
    {
        struct
        {
            f32 x,
                y,
                z,
                w;
        };
        f32 elements[4];
    };
} vec4_t;

/* ============================ *
 * =====     Matrix4      ===== *
 * ============================ */

// Definition of a 4x4 matrix
typedef struct _TAG_mat4
{
    union
    {
        struct
        {
            vec4_t  col1,
                    col2,
                    col3,
                    col4;
        };
        f32 elements[16];
    };
} mat4_t;

// Returns a 4x4 identity matrix
MHO_EXTERN mat4_t      mat4_identity(void);

// Returns a 4x4 translation matrix given x, y, z components
MHO_EXTERN mat4_t      mat4_translate(f32 x, f32 y, f32 z);

// Returns a 4x4 translation matrix given a 3D vector
MHO_EXTERN mat4_t      mat4_translate_v(vec3_t vec);

// Removes the "translation" component from a 4x4 matrix
// (col4.xyz ---> 0)
MHO_EXTERN mat4_t      mat4_translate_remove(mat4_t matrix);

// Prints the given 4x4 matrix to stdout
MHO_EXTERN void        mat4_print(mat4_t matrix);

// Returns a 4x4 rotation matrix given an angle in degrees and x, y, z components
MHO_EXTERN mat4_t      mat4_rotate(f32 angle, f32 x, f32 y, f32 z);

// Returns a 4x4 rotation matrix given an angle in degrees and 3D vector
MHO_EXTERN mat4_t      mat4_rotate_v(f32 angle, vec3_t vec);

// Returns a 4x4 perspective matrix given an FOV, aspect ratio, and
// near/far plane distance values
MHO_EXTERN mat4_t      mat4_perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far);

// Returns a 4x4 lookat matrix given an eye, center, and up 3D vector
MHO_EXTERN mat4_t      mat4_lookat(vec3_t eye, vec3_t center, vec3_t up);

// Returns a 4x4 scale matrix given a scale factor/value
MHO_EXTERN mat4_t      mat4_scale(f32 scale_value);

// Computes the product of two 4x4 matrices
MHO_EXTERN mat4_t      mat4_mult(mat4_t m1, mat4_t m2);

/* ============================ *
 * =====       Misc       ===== *
 * ============================ */

// Pseudo RNG for unsigned 32bit ints
MHO_EXTERN u32         m_randui(u32 index);

// Pseudo RNG for 32bit floats (ranged 0 to 1)
MHO_EXTERN f32         m_randf(u32 index);

// Pseudo RNG for 64bit floats (ranged 0 to 1)
MHO_EXTERN f64         m_randd(u32 index);

// Pseudo RNG for 32bit floats (ranged -1 to 1)
MHO_EXTERN f32         m_randnf(u32 index);

// Pseudo RNG for 64bit floats (ranged -1 to 1)
MHO_EXTERN f64         m_randnd(u32 index);

// Quake III square root
MHO_EXTERN f32         m_fsqrt(f32 number);

// Quake III inverse square root
MHO_EXTERN f32         m_fsqrtinv(f32 number);



///////////////////////////////////////////////////////////////////////////////
//
//		MMDBG
//

// Record structure for storing memory information
typedef struct _TAG_m_mem_rec
{
    void    *ptr;
    char    *file,
            *df_file;
    int     line,
            df_line,
            size;
    byte    flags;
    struct  _TAG_m_mem_rec *next;
} m_mem_rec_t;

MHO_EXTERN void   *m_mem_malloc(size_t size, const char *file, int line);
MHO_EXTERN void   m_mem_free(void *buffer, const char *file, int line);
MHO_EXTERN void   m_mem_print(FILE *stream);
MHO_EXTERN void   m_mem_rec_append(void *ptr, const char *file, int line, int size);
MHO_EXTERN void   m_mem_debug_memory();



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



//////////////////////////////////////////////////////////////////
// Math

/////////////////////////////
// VECTOR2D IMPLEMENTATION

vec2_t
vec2_ctor(f32 x,
          f32 y)
{
    vec2_t vec;

    vec.x = x;
    vec.y = y;

    return vec;
}

vec2_t
vec2_add(vec2_t v1,
         vec2_t v2)
{
    v1.x += v2.x;
    v1.y += v2.y;

    return v1;
}

vec2_t
vec2_sub(vec2_t v1,
         vec2_t v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;

    return v1;
}

vec2_t
vec2_scal(vec2_t vec,
          f32 scalar)
{
    vec.x *= scalar;
    vec.y *= scalar;

    return vec;
}

f32
vec2_dot(vec2_t v1,
         vec2_t v2)
{
    return ((v1.x * v2.x) + (v1.y * v2.y));
}

f32
vec2_mag(vec2_t vec)
{
    return m_sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

vec2_t
vec2_normalize(vec2_t vec)
{
    f32 val = m_fsqrtinv((vec.x * vec.x) + (vec.y * vec.y));

    vec.x *= val;
    vec.y *= val;

    return vec;
}

vec2_t
vec2_rotate(vec2_t vec,
            f32 angle)
{
    f32 r_angle = m_rads(angle);
    f32 c = m_cos(r_angle);
    f32 s = m_sin(r_angle);

    vec.x = ((vec.x * c) - (vec.y * s));
    vec.y = ((vec.x * s) + (vec.y * c));

    return vec;
}

//////////////////////////////
// VECTOR3D IMPLEMENTATION

vec3_t
vec3_ctor(f32 x,
          f32 y,
          f32 z)
{
    vec3_t vec;

    vec.x = x;
    vec.y = y;
    vec.z = z;

    return vec;
}

vec3_t
vec3_add(vec3_t v1,
         vec3_t v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;

    return v1;
}

vec3_t
vec3_sub(vec3_t v1,
         vec3_t v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;

    return v1;
}

vec3_t
vec3_scal(vec3_t vec,
          f32 scalar)
{
    vec.x *= scalar;
    vec.y *= scalar;
    vec.z *= scalar;

    return vec;
}

f32
vec3_dot(vec3_t v1,
         vec3_t v2)
{
    return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

vec3_t
vec3_cross(vec3_t v1,
           vec3_t v2)
{
    vec3_t cross_prod;
    
    cross_prod.x = (v1.y * v2.z) - (v1.z * v2.y);
    cross_prod.y = (v1.z * v2.x) - (v1.x * v2.z);
    cross_prod.z = (v1.x * v2.y) - (v1.y * v2.x);

    return cross_prod;
}

f32
vec3_mag(vec3_t vec)
{
    return m_sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

vec3_t 
vec3_normalize(vec3_t vec)
{
    f32 val = m_fsqrtinv((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));

    vec.x *= val;
    vec.y *= val;
    vec.z *= val;

    return vec;
}

//////////////////////////////
// MATRIX4 IMPLEMENTATION

mat4_t
mat4_identity(void)
{
    mat4_t matrix = {0};
    
    matrix.col1.x = 1.0f;
    matrix.col2.y = 1.0f;
    matrix.col3.z = 1.0f;
    matrix.col4.w = 1.0f;

    return matrix;
}

mat4_t
mat4_translate(f32 x,
               f32 y,
               f32 z)
{
    mat4_t matrix = mat4_identity();

    matrix.col4.x = x;
    matrix.col4.y = y;
    matrix.col4.z = z;

    return matrix;
}

mat4_t
mat4_translate_v(vec3_t vec)
{
    mat4_t matrix = mat4_identity();

    matrix.col4.x = vec.x;
    matrix.col4.y = vec.y;
    matrix.col4.z = vec.z;

    return matrix;
}

mat4_t
mat4_translate_remove(mat4_t matrix)
{
    matrix.col4.x = 0.0f;
    matrix.col4.y = 0.0f;
    matrix.col4.z = 0.0f;

    return matrix;
}

void
mat4_print(mat4_t matrix)
{
    for (u8 i = 0; i < 4; i++)
    {
        for (u8 j = 0; j < 4; j++)
        {
            printf("%f ", matrix.elements[i + j * 4]);
        }
        printf("\n");
    }
} 

mat4_t
mat4_rotate(f32 angle, 
            f32 x, 
            f32 y, 
            f32 z)
{
    vec3_t vec = {x, y, z};
    vec = vec3_normalize(vec);
    f32 c = m_cos(m_rads(angle));
    f32 s = m_sin(m_rads(angle));
    f32 c1 = 1.0f - c;

    mat4_t matrix = {0};

    matrix.col1.x = (c1 * vec.x * vec.x) + c;
    matrix.col1.y = (c1 * vec.x * vec.y) + s * vec.z;
    matrix.col1.z = (c1 * vec.x * vec.z) - s * vec.y;

    matrix.col2.x = (c1 * vec.x * vec.y) - s * vec.z;
    matrix.col2.y = (c1 * vec.y * vec.y) + c;
    matrix.col2.z = (c1 * vec.y * vec.z) + s * vec.x;

    matrix.col3.x = (c1 * vec.x * vec.z) + s * vec.y;
    matrix.col3.y = (c1 * vec.y * vec.z) - s * vec.x;
    matrix.col3.z = (c1 * vec.z * vec.z) + c;

    matrix.col4.w = 1.0f;

    return matrix;
}

mat4_t
mat4_rotate_v(f32 angle,
              vec3_t vec)
{
    vec = vec3_normalize(vec);
    f32 c = m_cos(m_rads(angle));
    f32 s = m_sin(m_rads(angle));
    f32 c1 = 1.0f - c;

    mat4_t matrix = {0};

    matrix.col1.x = (c1 * vec.x * vec.x) + c;
    matrix.col1.y = (c1 * vec.x * vec.y) + s * vec.z;
    matrix.col1.z = (c1 * vec.x * vec.z) - s * vec.y;

    matrix.col2.x = (c1 * vec.x * vec.y) - s * vec.z;
    matrix.col2.y = (c1 * vec.y * vec.y) + c;
    matrix.col2.z = (c1 * vec.y * vec.z) + s * vec.x;

    matrix.col3.x = (c1 * vec.x * vec.z) + s * vec.y;
    matrix.col3.y = (c1 * vec.y * vec.z) - s * vec.x;
    matrix.col3.z = (c1 * vec.z * vec.z) + c;

    matrix.col4.w = 1.0f;

    return matrix;
}

mat4_t
mat4_perspective(f32 fov,
                 f32 aspect_ratio,
                 f32 near,
                 f32 far)
{
    f32 t = m_tan(m_rads(fov) / 2.0f);
    f32 fdelta = far - near;
    
    mat4_t matrix = {0};

    matrix.col1.x = 1 / (aspect_ratio * t);

    matrix.col2.y = 1 / t;

    matrix.col3.z = -1 * ((far + near) / fdelta);
    matrix.col3.w = -1;

    matrix.col4.z = ((-2.0f * far * near) / fdelta);

    return matrix;
}

mat4_t
mat4_lookat(vec3_t eye, 
            vec3_t center, 
            vec3_t up)
{
    const vec3_t f = vec3_normalize(vec3_sub(center, eye));
    const vec3_t s = vec3_normalize(vec3_cross(f, up));
    const vec3_t u = vec3_cross(s, f);

    mat4_t matrix;

    matrix.col1.x = s.x;
    matrix.col1.y = u.x;
    matrix.col1.z = -f.x;
    matrix.col1.w = 0.0f;

    matrix.col2.x = s.y;
    matrix.col2.y = u.y;
    matrix.col2.z = -f.y;
    matrix.col2.w = 0.0f;

    matrix.col3.x = s.z;
    matrix.col3.y = u.z;
    matrix.col3.z = -f.z;
    matrix.col3.w = 0.0f;

    matrix.col4.x = -vec3_dot(s, eye);
    matrix.col4.y = -vec3_dot(u, eye);
    matrix.col4.z = vec3_dot(f, eye);
    matrix.col4.w = 1.0f;

    return matrix;
}

mat4_t
mat4_scale(f32 scale_value)
{
    mat4_t matrix = {0};

    matrix.col1.x = scale_value;
    matrix.col2.y = scale_value;
    matrix.col3.z = scale_value;
    matrix.col4.w = 1.0f;

    return matrix;
}

mat4_t
mat4_mult(mat4_t m1,
          mat4_t m2)
{
    mat4_t res;

    for (u8 y = 0; y < 4; y++)
    {
        for (u8 x = 0; x < 4; x++)
        {
            f32 sum = 0.0f;
            for (u8 e = 0; e < 4; e++)
            {
                sum += m1.elements[x + e * 4] * m2.elements[e + y * 4];
            }
            res.elements[x + y * 4] = sum;
        }
    }

    return res;
}

//////////////////////////////
// MISC IMPLEMENTATION

u32
m_randui(u32 index)
{
    index = (index << 13) ^ index;
    return ((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF);
}

f32
m_randf(u32 index)
{
    index = (index << 13) ^ index;
    return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0f) * 0.5f;
}

f64
m_randd(u32 index)
{
    index = (index << 13) ^ index;
    return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0) * 0.5;
}

f32
m_randnf(u32 index)
{
    index = (index << 13) ^ index;
    return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0f) - 1.0f;
}

f64
m_randnd(u32 index)
{
    index = (index << 13) ^ index;
    return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0) - 1.0;
}

f32
m_fsqrt(f32 number)
{
    f32 x = number * 0.5f;
    f32 y = number;
    s32 i = *(s32 *)&y;             // evil floating point bit hack
    i = 0x5F3759DF - (i >> 1);      // what the fuck?
    y = *(f32 *)&i;
    y = y * (1.5f - (x * y * y));   // 1st iteration
    y = y * (1.5f - (x * y * y));   // 2nd iteration

    return number * y;              // mulitply by original num to reverse and get sqrt
}

f32
m_fsqrtinv(f32 number)
{
    f32 x = number * 0.5f;
    f32 y = number;
    s32 i = *(s32 *)&y;             // evil floating point bit hack
    i = 0x5F3759DF - (i >> 1);      // what the fuck?
    y = *(f32 *)&i;
    y = y * (1.5f - (x * y * y));   // 1st iteration
    y = y * (1.5f - (x * y * y));   // 2nd iteration

    return y;
}



//////////////////////////////////////////////////////////////////
// MMDbg

// Important #define constants we use
#define M_MEM_TRUE              1
#define M_MEM_FALSE             0
#define M_MEM_FREE_BIT          0x01
#define M_MEM_OVER_BIT          0x02
#define M_MEM_UNDER_BIT         0x04
#define M_MEM_DOUBLE_FREE_BIT   0x08 
#define M_MEM_OVER_NUM          0x192BA3A2
#define M_MEM_UNDER_NUM         0x39D7A5DA

static s32              m_mem_malloc_cnt;
static s32              m_mem_free_cnt;
static usize           	m_mem_total_alloc;
static m_mem_rec_t     	*m_mem_alloc_head = NULL;

void *
m_mem_malloc(size_t size,
             const char *file,
             int line)
{
    void    *ptr;
    dword   *buff_un,
            *buff_ov;
    
    buff_un = (dword *)malloc(size + 2 * sizeof(dword));

    // if allocation succeeded
    if (buff_un)
    {
        ptr = buff_un + 1;
        buff_ov = (dword *)((byte *)ptr + size);
        *buff_ov = MMDBG_OVER_NUM;
        *buff_un = MMDBG_UNDER_NUM;

        m_mem_malloc_cnt++;
        m_mem_total_alloc += size;
        m_mem_rec_append(&m_mem_alloc_head, ptr, file, line, size);
    }
    
    // return ptr regardless
    return ptr;
}

void
m_mem_free(void *buffer,
           const char *file,
           int line)
{
    m_mem_rec_t     *temp;
    dword           value;
    void            *p;

    // Set the 'freed' (or 'double freed') flag(s) if necessary
    temp = m_mem_alloc_head;
    while (temp != NULL)
    {
        if (temp->ptr == buffer)
        {
            if (!(temp->flags & MMDBG_FREE_BIT))
            {
                temp->flags |= MMDBG_FREE_BIT;
            }
            else if (temp->flags & MMDBG_FREE_BIT)
            {
                temp->flags |= MMDBG_DOUBLE_FREE_BIT;
                temp->df_file = (char *)file;
                temp->df_line = line;
            }

            break;
        }

        temp = temp->next;
    }

    // After (potentially) setting the 'double freed' flag, proceed to
    // check for any buffer runs and then free the ptr.
    if (!(temp->flags & MMDBG_DOUBLE_FREE_BIT))
    {
        // Check for overrun
        p = (byte *)buffer + temp->size;
        value = *(dword *)p;
        if (value != MMDBG_OVER_NUM)
        {
            temp->flags |= MMDBG_OVER_BIT;
        }

        // Check for underrun
        p = (byte *)buffer - 4;
        value = *(dword *)p;
        if (value != MMDBG_UNDER_NUM)
        {
            temp->flags |= MMDBG_UNDER_BIT;
        }

        m_mem_free_cnt++;

        // free the buffer
        free((dword *)buffer - 1);
    }
}

void
m_mem_rec_append(void *ptr,
                 const char *file,
                 int line,
                 int size)
{
    m_mem_rec_t     *new_node,
                    *temp;

    // Allocate and fill new node
    new_node = (m_mem_rec_t *)malloc(sizeof(m_mem_rec_t));
    new_node->ptr = ptr;
    new_node->file = (char *)file;
    new_node->line = line;
    new_node->df_file = NULL;
    new_node->df_line = 0;
    new_node->size = size;
    new_node->flags = 0x0;
    new_node->next = NULL;

    // If list is empty
    if (*head == NULL)
    {
        *head = new_node;
    }

    else
    {
        // Find last node
        temp = *head;
        while (temp->next != NULL)
            temp = temp->next;

        // Append
        temp->next = new_node;
    }
}

void
m_mem_debug_memory()
{
    m_mem_rec_t     *temp;
    dword           value;
    void            *p;

    temp = m_mem_alloc_head;
    while (temp != NULL)
    {
        if (!(temp->flags && MMDBG_FREE_BIT))
        {
            // Find overruns
            p = (byte *)temp->ptr + temp->size;
            value = *(dword *)p;
            if (value != MMDBG_OVER_NUM)
            {
                temp->flags |= MMDBG_OVER_BIT;
            }

            // Find underruns
            p = (byte *)temp->ptr - 4;
            value = *(dword *)p;
            if (value != MMDBG_UNDER_NUM)
            {
                temp->flags |= MMDBG_UNDER_BIT;
            }
        }

        temp = temp->next;
    }
}

void
m_mem_print(FILE *stream)
{
    m_mem_rec_t     *temp;
    void            *p;

    m_mem_debug_memory();

    fprintf(stream, "\n=========================================================\n");
    fprintf(stream, "                    MMDBG OUTPUT\n");
    fprintf(stream, "=========================================================\n");
    fprintf(stream, "Total Mallocs: %d\n", m_mem_malloc_cnt);
    fprintf(stream, "Total Frees:   %d\n", m_mem_free_cnt);
#ifdef __cplusplus
    fprintf(stream, "Total News:    %d\n", m_mem_new_cnt);
    fprintf(stream, "Total Deletes: %d\n", m_mem_delete_cnt);
#endif
    fprintf(stream, "Total Size:    %d bytes\n\n", m_mem_total_alloc);

    // Print out all debug-related info
    temp = m_mem_alloc_head;
    while (temp != NULL)
    {
        // Memory Leaks
        if (!(temp->flags & MMDBG_FREE_BIT))
        {
            fprintf(stream, "UNFREED MEMORY:   0x%p (%s (%d))\n", temp->ptr,
                                                                  temp->file,
                                                                  temp->line);
        }

        // Double Frees
        if (temp->flags & MMDBG_DOUBLE_FREE_BIT)
        {
            fprintf(stream, "DOUBLE FREE:      0x%p (%s (%d))\n", temp->ptr,
                                                                  temp->df_file,
                                                                  temp->df_line);
        }

        // Buffer Underruns
        if (temp->flags & MMDBG_UNDER_BIT)
        {
            p = (byte *)temp->ptr - 4;
            fprintf(stream, "BUFFER UNDERRUN:  0x%p (%s (%d))\n", p,
                                                                  temp->file,
                                                                  temp->line);
        }

        // Buffer Overruns
        if (temp->flags & MMDBG_OVER_BIT)
        {
            p = (byte *)temp->ptr + temp->size;
            fprintf(stream, "BUFFER OVERRUN:   0x%p (%s (%d))\n", p,
                                                                  temp->file,
                                                                  temp->line);
        }

        temp = temp->next;
    }

    fprintf(stream, "=========================================================\n");
    fprintf(stream, "                    END OF OUTPUT\n");
    fprintf(stream, "=========================================================\n");
}

 #endif // MHO_IMPL

#endif // MHO_H
