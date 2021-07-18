// mho is a general utility library, consisting of C functions that can be
// useful for any type of development. It contains various functionality,
// such as vector/matrix math, a dynamic array (like std::vector), memory
// and FILE * debugging and some general utility functions for files and
// strings.

// mho should be the last file that is #included in your project, as
// it contains some definitions (ie, TRUE/FALSE) that other libraries should
// have precedence for, as well as requiring other headers/files to be
// included for certain functionality to work(ie, <glad.h> for OpenGL).

#ifndef MHO_H
#define MHO_H

// TODO: Implement quaternion and VQS structures + functions.
// TODO: Add #pragma's to .c files.
// TODO: Add MHO_ARR_INCSZ macro to resize array by a given size.
// TODO: Implement these C stdlib fn's.
// TODO: See if we can prefix the typedef names with 'mho_' in case a file is
//       already included whose names collide.
// TODO: Implement debugging for fopen/fclose.
// TODO: Maybe take OpenGL stuff out?

///////////////////////////////////////////////////////////////////////////////
//
//      Includes
//

#include <stdio.h> // FILE *
#include <stdint.h> // int_t's
#include <string.h>
#include <stdlib.h>
#include <math.h>



///////////////////////////////////////////////////////////////////////////////
//
//      Types
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
//      General
//

// Defines NULL in C/C++ if not already defined (should be since stdlib.h is
// already included above)
#ifndef NULL
    #ifdef __cplusplus
        #define NULL    0
    #else
        #define NULL    ((void *)0)
    #endif // __cplusplus
#endif // NULL

// For C vs. C++ compilation
#ifdef __cplusplus
    #define MHO_EXTERN  extern "C"
#else
    #define MHO_EXTERN  extern
#endif

// TRUE/FALSE
#ifndef TRUE
    #define TRUE    1
#endif // TRUE
#ifndef FALSE
    #define FALSE   0
#endif // FALSE

// Macro to strip just the filename out of the full filepath.
#if defined(_WIN32) || defined(_WIN64)
    #define __FILENAME__    (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #define __FILENAME__    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif



///////////////////////////////////////////////////////////////////////////////
//
//      Misc.
//

#define mho_lerp(__a, __b, __t) \
    ((__a) + (f32)((__b) - (__a)) * (__t))

#define mho_clamp(__x, __min, __max) \
    ((__x) < (__min) ? (__min) : (__x) > (__max) ? (__max) : (__x))

#define mho_swap(__a, __b, __type)      \
    do                                  \
    {                                   \
        __type __MHO_SWAP_TEMP = __a;   \
        __a = __b;                      \
        __b = __MHO_SWAP_TEMP;          \
    } while (0)

#define mho_min(__a, __b) \
    ((__a) < (__b) ? (__a) : (__b))

#define mho_max(__a, __b) \
    ((__a) > (__b) ? (__a) : (__b))



///////////////////////////////////////////////////////////////////////////////
//
//      Dynamic Array
//
// Inspired GREATLY by John Jackson's 'gs_dyn_array' implementation from
// gunslinger: https://github.com/MrFrenik/gunslinger/blob/master/gs.h

typedef struct _TAG_mho_arr_header
{
    u32     capacity;   // Total number of elements
    u32     size;       // Number of elements in array containing data
} mho_arr_header_t;

// Wrapper macro
#define mho_arr(__type) \
    __type *

// Returns the header of the array
#define mho_arr_head(__array) \
    ((mho_arr_header_t *)(__array) - 1)

// Returns the size of the array
#define mho_arr_size(__array) \
    (__array == NULL ? 0 : mho_arr_head(__array)->size)

// Returns the capacity of the array
#define mho_arr_capacity(__array) \
    (__array == NULL ? 0 : mho_arr_head(__array)->capacity)

// Returns the length of the array (size * sizeof(type))
#define mho_arr_len(__array) \
    (mho_arr_size(__array) * (sizeof(*(__array))))

// Returns whether the array is full
#define mho_arr_full(__array) \
    ((mho_arr_size(__array)) == (mho_arr_capacity(__array)))

// Returns whether the array is empty
#define mho_arr_empty(__array) \
    (mho_arr_init((void **)&(__array), sizeof(*(__array)), (mho_arr_size(__array) == 0)))

// Frees the array
#define mho_arr_free(__array) \
    (free(mho_arr_head(__array)))

// Returns whether the array needs to grow by a given size
#define mho_arr_need_grow(__array, __n) \
    ((__array) == 0 || mho_arr_size(__array) + (__n) > mho_arr_capacity(__array))

// Grows the array (doubles in size or increases by MHO_ARR_INCSZ increments)
#define mho_arr_grow(__array) \
    mho_arr_resize((__array), sizeof(*(__array)), mho_arr_capacity(__array) ? mho_arr_capacity(__array) * 2 : 1)

// Grows the array by a given size
#define mho_arr_grow_size(__array, __sz) \
    mho_arr_resize((__array), sizeof(*(__array)), mho_arr_capacity(__array) ? mho_arr_capacity(__array) + (__sz) : 1)

// Pushes data to the back of the array
#define mho_arr_push(__array, __data)                                       \
    do                                                                      \
    {                                                                       \
        mho_arr_init((void **)&(__array), sizeof(*(__array)));              \
        if (!(__array) || ((__array) && mho_arr_need_grow(__array, 1)))     \
        {                                                                   \
            *((void **)&(__array)) = mho_arr_grow(__array);                 \
        }                                                                   \
        (__array)[mho_arr_size(__array)] = (__data);                        \
        mho_arr_head(__array)->size++;                                      \
    } while (0)

// Pops data off the array (decrements the size) and returns it
#define mho_arr_pop(__array) \
    ((mho_arr_size(__array) > 0) ? (__array)[(--mho_arr_head(__array)->size)] : (__array)[(mho_arr_head(__array)->size)])

// Clears all the elements in the array, (simply sets size to 0)
#define mho_arr_clear(__array)                  \
    do                                          \
    {                                           \
        if (__array)                            \
            mho_arr_head(__array)->size = 0;    \
    } while (0)

// Reserves internal space in the array for __amount, non-initialized elements
#define mho_arr_reserve(__array, __amount)                                                  \
    do                                                                                      \
    {                                                                                       \
        if ((!__array))                                                                     \
            mho_arr_init((void **)&(__array), sizeof(*(__array)));                          \
        if ((!__array) || (usize)__amount > mho_arr_capacity(__array))                      \
        {                                                                                   \
            *((void **)&(__array)) = mho_arr_resize(__array, sizeof(*(__array)), __amount); \
        }                                                                                   \
    } while (0)

// Shrinks array such that capacity = size (no excess memory usage for unused elements)
#define mho_arr_shrink(__array) \
    *((void **)&(__array)) = mho_arr_resize(__array, sizeof(*(__array)), mho_arr_size(__array));

// Inserts data into the array at the specified position (0 indexing)
// NOTE: DOES NOT WORK AT INDEX 0 (ie, cannot insert at start of array)
#define mho_arr_insert(__array, __val, __pos)                                               \
    do                                                                                      \
    {                                                                                       \
        u32 _mho_iter_;                                                                     \
        if ((mho_arr_need_grow(__array, 1)))                                                \
            *((void **)&(__array)) = mho_arr_grow_size(__array, 1);                         \
        for (_mho_iter_ = mho_arr_size(__array) - 1; _mho_iter_ >= (__pos); _mho_iter_--)   \
        {                                                                                   \
            (__array)[_mho_iter_ + 1] = (__array)[_mho_iter_];                              \
        }                                                                                   \
        (__array)[(__pos)] = (__val);                                                       \
        mho_arr_head(__array)->size++;                                                      \
    } while (0);

// Removes data from the array at the specified position and realigns elements
#define mho_arr_remove(__array, __pos)                                                  \
    do                                                                                  \
    {                                                                                   \
        u32 _mho_iter_;                                                                 \
        for (_mho_iter_ = (__pos); _mho_iter_ < mho_arr_size(__array); _mho_iter_++)    \
        {                                                                               \
            (__array)[_mho_iter_] = (__array)[_mho_iter_ + 1];                          \
        }                                                                               \
        mho_arr_head(__array)->size--;                                                  \
    } while (0);

// Initializes the array
local void **
mho_arr_init(void **arr,
             usize val_size)
{
    mho_arr_header_t    *data;

    if (*arr == NULL)
    {
        data = (mho_arr_header_t *)malloc(val_size + sizeof(mho_arr_header_t));

        if (data)
        {
            data->size = 0;
            data->capacity = 1;
            *arr = ((mho_arr_header_t *)data + 1);

            return arr;
        }
    }

    return NULL;
}

// Resizes the array
local void *
mho_arr_resize(void *arr,
               usize sz,
               usize amt)
{
    usize               capacity;
    mho_arr_header_t    *data;

    if (arr)
        capacity = amt;
    else
        capacity = 0;

    // Create new array with header + desired size (or 0 if arr is empty)
    data = (mho_arr_header_t *)realloc(arr ? mho_arr_head(arr) : 0, capacity * sz + sizeof(mho_arr_header_t));

    if (data)
    {
        if (!arr)
            data->size = 0;

        data->capacity = (u32)capacity;
        data = (mho_arr_header_t *)data + 1;
    }

    return data;
}



///////////////////////////////////////////////////////////////////////////////
//
//      Util
//

// Reads a file and writes it into a char * buffer
MHO_EXTERN char     *mho_read_file_buffer(const char *filename);

// Returns the length of a file given a FILE * handle
MHO_EXTERN s64      mho_filelen(FILE *fp);

// C stdlib custom implementations
MHO_EXTERN void     mho_memcpy(void *dest, void *src, usize n);
MHO_EXTERN void     *mho_memset(void *dest, s32 c, usize n);
MHO_EXTERN void     mho_strcpy(s8 *dest, s8 *src);
MHO_EXTERN void     mho_strncpy(s8 *dest, s8 *src, usize n);
MHO_EXTERN usize    mho_strlen(s8 *str);
MHO_EXTERN s8       *mho_strcat(s8 *str1, s8 *str2);
MHO_EXTERN s32      mho_strcmp(s8 *dest, s8 *src);
MHO_EXTERN s32      mho_strncmp(s8 *dest, s8 *src, usize n);


///////////////////////////////////////////////////////////////////////////////
//
//      Math
//

// Useful #define macros + constants
#define mho_sqrt(__n)       sqrtf(__n)
#define mho_sin(__n)        sinf(__n)
#define mho_cos(__n)        cosf(__n)
#define mho_tan(__n)        tanf(__n)
#define mho_cast(__n)       (f32 *)&__n
#define mho_rads(__n)       (__n * 0.017453f)
#define mho_degs(__n)       (__n * 57.29578f)
#ifndef M_PI
    #define M_PI            3.141592653589f
#endif // M_PI

/* ============================ *
 * =====     Vector2D     ===== *
 * ============================ */

// Definition of a 2D vector
typedef struct _TAG_mho_vec2
{
    union
    {
        struct
        {
            f32     x,
                    y;
        };
        f32         elements[2];
    };
} mho_vec2_t;

// Constructs a 2D vector given x, y components
MHO_EXTERN mho_vec2_t       mho_vec2_ctor(f32 x, f32 y);

// Computes the sum of two 2D vectors
// (v1 + v2)
MHO_EXTERN mho_vec2_t       mho_vec2_add(mho_vec2_t v1, mho_vec2_t v2);

// Computes the difference of two 2D vectors
// (v1 - v2)
MHO_EXTERN mho_vec2_t       mho_vec2_sub(mho_vec2_t v1, mho_vec2_t v2);

// Computes a 2D vector multiplied by a scalar
// (vec * scalar)
MHO_EXTERN mho_vec2_t       mho_vec2_scal(mho_vec2_t vec, f32 scalar);

// Computes the dot product of two 2D vectors
// (v1 . v2)
MHO_EXTERN f32              mho_vec2_dot(mho_vec2_t v1, mho_vec2_t v2);

// Computes the magnitude (norm) of a 2D vector
// (||vec||)
MHO_EXTERN f32              mho_vec2_mag(mho_vec2_t vec);

// Normalizes a given 2D vector
// (vec * (1 / ||vec||))
MHO_EXTERN mho_vec2_t       mho_vec2_normalize(mho_vec2_t vec);

// Computes a 2D vector rotated by an angle in degrees
MHO_EXTERN mho_vec2_t       mho_vec2_rotate(mho_vec2_t vec, f32 angle);

// Performs a pairwise multiplication between two 2D vectors
MHO_EXTERN mho_vec2_t       mho_vec2_pmul(mho_vec2_t v1, mho_vec2_t v2);

/* ============================ *
 * =====     Vector3D     ===== *
 * ============================ */

// Definition of a 3D vector
typedef struct _TAG_mho_vec3
{
    union
    {
        struct
        {
            f32     x,
                    y,
                    z;
        };
        f32         elements[3];
    };
} mho_vec3_t;

// Constructs a 3D vector given x, y, z components
MHO_EXTERN mho_vec3_t       mho_vec3_ctor(f32 x, f32 y, f32 z);

// Computes the sum of two 3D vectors
// (v1 + v2)
MHO_EXTERN mho_vec3_t       mho_vec3_add(mho_vec3_t v1, mho_vec3_t v2);

// Computes the difference of two 3D vectors
// (v1 - v2)
MHO_EXTERN mho_vec3_t       mho_vec3_sub(mho_vec3_t v1, mho_vec3_t v2);

// Computes a 3D vector multipled by a scalar
// (vec * scalar)
MHO_EXTERN mho_vec3_t       mho_vec3_scal(mho_vec3_t vec, f32 scalar);

// Computes the dot product of two 3D vectors
// (v1 . v2)
MHO_EXTERN f32              mho_vec3_dot(mho_vec3_t v1, mho_vec3_t v2);

// Computes the cross product of two 3D vectors
// (v1 x v2)
MHO_EXTERN mho_vec3_t       mho_vec3_cross(mho_vec3_t v1, mho_vec3_t v2);

// Computes the magnitude (norm) of a 3D vector
// (||vec||)
MHO_EXTERN f32              mho_vec3_mag(mho_vec3_t vec);

// Normalizes a given 3D vector
// (vec * (1 / ||vec||))
MHO_EXTERN mho_vec3_t       mho_vec3_normalize(mho_vec3_t vec);

// Performs a pairwise multiplication between two 3D vectors
MHO_EXTERN mho_vec3_t       mho_vec3_pmul(mho_vec3_t v1, mho_vec3_t v2);

/* ============================ *
 * =====     Vector4D     ===== *
 * ============================ */

// Definition of a 4D vector
typedef struct _TAG_mho_vec4
{
    union
    {
        struct
        {
            f32     x,
                    y,
                    z,
                    w;
        };
        f32         elements[4];
    };
} mho_vec4_t;

// Constructs a 3D vector given x, y, z, w components
MHO_EXTERN mho_vec4_t       mho_vec4_ctor(f32 x, f32 y, f32 z, f32 w);

// Computes the sum of two 4D vectors
// (v1 + v2)
MHO_EXTERN mho_vec4_t       mho_vec4_add(mho_vec4_t v1, mho_vec4_t v2);

// Computes the difference of two 4D vectors
// (v1 - v2)
MHO_EXTERN mho_vec4_t       mho_vec4_sub(mho_vec4_t v1, mho_vec4_t v2);

// Computes a 4D vector multipled by a scalar
// (vec * scalar)
MHO_EXTERN mho_vec4_t       mho_vec4_scal(mho_vec4_t vec, f32 scalar);

// Computes the dot product of two 4D vectors
// (v1 . v2)
MHO_EXTERN f32              mho_vec4_dot(mho_vec4_t v1, mho_vec4_t v2);

// Computes the magnitude (norm) of a 4D vector
// (||vec||)
MHO_EXTERN f32              mho_vec4_mag(mho_vec4_t vec);

// Normalizes a given 4D vector
// (vec * (1 / ||vec||))
MHO_EXTERN mho_vec4_t       mho_vec4_normalize(mho_vec4_t vec);

// Performs a pairwise multiplication between two 4D vectors
MHO_EXTERN mho_vec4_t       mho_vec4_pmul(mho_vec4_t v1, mho_vec4_t v2);

/* ============================ *
 * =====     Matrix4      ===== *
 * ============================ */

// Definition of a 4x4 matrix
typedef struct _TAG_mho_mat4
{
    union
    {
        struct
        {
            mho_vec4_t      col1,
                            col2,
                            col3,
                            col4;
        };
        f32                 elements[16];
    };
} mho_mat4_t;

// Returns a 4x4 identity matrix
MHO_EXTERN mho_mat4_t       mho_mat4_identity(void);

// Returns a 4x4 translation matrix given x, y, z components
MHO_EXTERN mho_mat4_t       mho_mat4_translate(f32 x, f32 y, f32 z);

// Returns a 4x4 translation matrix given a 3D vector
MHO_EXTERN mho_mat4_t       mho_mat4_translate_v(mho_vec3_t vec);

// Removes the "translation" component from a 4x4 matrix
// (col4.xyz ---> 0)
MHO_EXTERN mho_mat4_t       mho_mat4_translate_remove(mho_mat4_t matrix);

// Prints the given 4x4 matrix to stdout
MHO_EXTERN void             mho_mat4_print(mho_mat4_t matrix);

// Returns a 4x4 rotation matrix given an angle in degrees and x, y, z components
MHO_EXTERN mho_mat4_t       mho_mat4_rotate(f32 angle, f32 x, f32 y, f32 z);

// Returns a 4x4 rotation matrix given an angle in degrees and 3D vector
MHO_EXTERN mho_mat4_t       mho_mat4_rotate_v(f32 angle, mho_vec3_t vec);

// Returns a 4x4 perspective matrix given an FOV, aspect ratio, and
// near/far plane distance values
MHO_EXTERN mho_mat4_t       mho_mat4_perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far);

// Returns a 4x4 lookat matrix given an eye, center, and up 3D vector
MHO_EXTERN mho_mat4_t       mho_mat4_lookat(mho_vec3_t eye, mho_vec3_t center, mho_vec3_t up);

// Returns a 4x4 scale matrix given a scale factor/value
MHO_EXTERN mho_mat4_t       mho_mat4_scale(f32 scale_value);

// Returns a 4x4 scale matrix given a scale vector
MHO_EXTERN mho_mat4_t       mho_mat4_scale_v(mho_vec3_t scale);

// Computes the product of two 4x4 matrices
MHO_EXTERN mho_mat4_t       mho_mat4_mul(mho_mat4_t m1, mho_mat4_t m2);

/* ============================ *
 * =====    Quaternion    ===== *
 * ============================ */

// Definition of a quaternion
typedef struct _TAG_mho_quat
{
    union
    {
        struct
        {
            f32     w,
                    i,
                    j,
                    k;
        };
        f32         elements[4];
    };
} mho_quat_t;

// Constructs a quaternion given w, i, j, k components
MHO_EXTERN mho_quat_t       mho_quat_ctor(f32 w, f32 i, f32 j, f32 k);

// Computes the sum of two quaternions
// (q1 + q2)
MHO_EXTERN mho_quat_t       mho_quat_add(mho_quat_t q1, mho_quat_t q2);

// Computes the difference of two quaternions
// (q1 - q2)
MHO_EXTERN mho_quat_t       mho_quat_sub(mho_quat_t q1, mho_quat_t q2);

// Computes the product of two quaternions
// (q1 * q2)
MHO_EXTERN mho_quat_t       mho_quat_mul(mho_quat_t q1, mho_quat_t q2);

// Computes the conjugate of a quaternion
// (w - i - j - k)
MHO_EXTERN mho_quat_t       mho_quat_conj(mho_quat_t q);

/* ============================ *
 * =====        VQS       ===== *
 * ============================ */

// Definition of a VQS structure
typedef struct _TAG_mho_vqs
{
    mho_vec3_t      pos;
    mho_quat_t      rotate;
    mho_vec3_t      scale;
} mho_vqs_t;

/* ============================ *
 * =====       Misc       ===== *
 * ============================ */

// Pseudo RNG for unsigned 32bit ints
MHO_EXTERN u32      mho_randui(u32 index);

// Pseudo RNG for 32bit floats (ranged 0 to 1)
MHO_EXTERN f32      mho_randf(u32 index);

// Pseudo RNG for 64bit floats (ranged 0 to 1)
MHO_EXTERN f64      mho_randd(u32 index);

// Pseudo RNG for 32bit floats (ranged -1 to 1)
MHO_EXTERN f32      mho_randnf(u32 index);

// Pseudo RNG for 64bit floats (ranged -1 to 1)
MHO_EXTERN f64      mho_randnd(u32 index);

// Quake III square root
MHO_EXTERN f32      mho_fsqrt(f32 number);

// Quake III inverse square root
MHO_EXTERN f32      mho_fsqrtinv(f32 number);

// Single octave 2D 32bit float noise.
MHO_EXTERN f32      mho_noise2f(f32 x, f32 y);

///////////////////////////////////////////////////////////////////////////////
//
//      OpenGL
//

#if 0
// Builds a GLSL shader program using a vertex + fragment source
MHO_EXTERN u32      mho_load_shader_vf(const char *vs_path, const char *fs_path);

// Builds a GLSL shader program using a compute source
MHO_EXTERN u32      mho_load_shader_comp(const char *cs_path);

// Checks for any compile errors in a given GLSL shader program or source
MHO_EXTERN void     mho_check_compile_errors(u32 data, u8 type, const char *filename);

 #ifdef _glfw3_h_
// Default framebuffer resize callback function for OpenGL + GLFW
MHO_EXTERN void     mho_framebuffer_size_callback(GLFWwindow *window, s32 width, s32 height);

// Default mouse callback function for OpenGL + GLFW
MHO_EXTERN void     mho_mouse_callback(GLFWwindow *window, f64 x_pos, f64 y_pos);
#endif
 #endif



///////////////////////////////////////////////////////////////////////////////
//
//      Debugging
//

// Record structure for storing memory information
typedef struct _TAG_mho_dbg_mem_rec
{
    void						*ptr;
    char						*file,
								*df_file;
    int							line,
								df_line,
								size;
    byte						flags;
	struct _TAG_mho_dbg_mem_rec 	*next;
} mho_dbg_mem_rec_t;

// Record structure for storing file handle information
typedef struct _TAG_mho_dbg_file_rec
{
	char							*file,
									*filename,
									*mode;
	int								line;
	byte							flags;
	struct _TAG_mho_dbg_file_rec	*next;
} mho_dbg_file_rec_t;

// Custom malloc impl for debugging
MHO_EXTERN void     *mho_dbg_malloc(size_t size, char *file, int line);

// Custom free impl for debugging
MHO_EXTERN void     mho_dbg_free(void *buffer, char *file, int line);

// Adds a new memory debug rec to the list
MHO_EXTERN void 	mho_dbg_mem_rec_append(void *ptr, const char *file, int line, int size);

// Verifies memory integrity of list (searches for over/underruns)
MHO_EXTERN void     mho_dbg_memory(void);

// Custom fopen impl for debugging
MHO_EXTERN FILE		*mho_dbg_fopen(char *filename, char *mode, char *file, int line);

// Custom fclose impl for debugging
MHO_EXTERN void		mho_dbg_fclose(FILE *fptr, char *filename, char *mode, char *file, int line);

// Adds a new file debug rec to the list
MHO_EXTERN void 	mho_dbg_file_rec_append(char *filename, char *mode, char *file, int line);

// Prints a debugging report to a specified stream
MHO_EXTERN void     mho_dbg_print(FILE *stream);


// Wraps debugging functions
 #ifdef MHO_DEBUG
    #define malloc(__n)     mho_dbg_malloc(__n, __FILENAME__, __LINE__)
    #define free(__n)       mho_dbg_free(__n, __FILENAME__, __LINE__)
 #endif // MHO_DEBUG

// Simplifies Math related names for types and functions
 #ifndef MHO_FULL_NAMES
    typedef mho_vec2_t                  vec2_t;
    typedef mho_vec3_t                  vec3_t;
    typedef mho_vec4_t                  vec4_t;
    typedef mho_mat4_t                  mat4_t;
    typedef mho_quat_t                  quat_t;

    #define vec2_ctor                   mho_vec2_ctor
    #define vec2_add                    mho_vec2_add
    #define vec2_sub                    mho_vec2_sub
    #define vec2_scal                   mho_vec2_scal
    #define vec2_dot                    mho_vec2_dot
    #define vec2_mag                    mho_vec2_mag
    #define vec2_normalize              mho_vec2_normalize
    #define vec2_rotate                 mho_vec2_rotate
    #define vec2_pmul                   mho_vec2_pmul

    #define vec3_ctor                   mho_vec3_ctor
    #define vec3_add                    mho_vec3_add
    #define vec3_sub                    mho_vec3_sub
    #define vec3_scal                   mho_vec3_scal
    #define vec3_dot                    mho_vec3_dot
    #define vec3_cross                  mho_vec3_cross
    #define vec3_mag                    mho_vec3_mag
    #define vec3_normalize              mho_vec3_normalize
    #define vec3_pmul                   mho_vec3_pmul

    #define vec4_ctor                   mho_vec4_ctor
    #define vec4_add                    mho_vec4_add
    #define vec4_sub                    mho_vec4_sub
    #define vec4_scal                   mho_vec4_scal
    #define vec4_dot                    mho_vec4_dot
    #define vec4_mag                    mho_vec4_mag
    #define vec4_normalize              mho_vec4_normalize
    #define vec4_pmul                   mho_vec4_pmul

    #define mat4_identity               mho_mat4_identity
    #define mat4_translate              mho_mat4_translate
    #define mat4_translate_v            mho_mat4_translate_v
    #define mat4_translate_remove       mho_mat4_translate_remove
    #define mat4_print                  mho_mat4_print
    #define mat4_rotate                 mho_mat4_rotate
    #define mat4_rotate_v               mho_mat4_rotate_v
    #define mat4_perspective            mho_mat4_perspective
    #define mat4_lookat                 mho_mat4_lookat
    #define mat4_scale                  mho_mat4_scale
    #define mat4_scale_v                mho_mat4_scale_v
    #define mat4_mul                   	mho_mat4_mul

    #define quat_ctor                   mho_quat_ctor
    #define quat_add                    mho_quat_add
    #define quat_sub                    mho_quat_sub
    #define quat_mul                    mho_quat_mul
    #define quat_conj                   mho_quat_conj

 #endif // MHO_FULL_NAMES

#endif // MHO_H

