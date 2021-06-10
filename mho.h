// mho is a general utility library, consisting of C functions that can be
// useful for any type of development. It's a single header library, so all
// that's required is to include it in your project directory and #define
// the implementation symbol.
//
// mho should ideally be the last file that is #included in your project, as
// it contains some definitions (ie, TRUE/FALSE) that other libraries should
// have precedence for, as well as requiring other headers/files to be
// included for certain functionality to work(ie, <glad.h> for OpenGL).

#ifndef MHO_H
#define MHO_H

// TODO: Fix and implement mho_obj.
// TODO: Implement these C stdlib fn's.
// TODO: See if we can prefix the typedef names with 'mho_' in case a file is
// already included whose names collide.
// TODO: Add mho_arr_shrink/compact().
// TODO: Add MHO_ARR_INC_SIZE macro to resize array by a given size.
// TODO: Add vec4 functions.
///////////////////////////////////////////////////////////////////////////////
//
//		Includes
//

#pragma warning(disable: 4996) // fopen
#pragma warning(disable: 4055) // fn ptr void * (enabled on +MSVC 14)
#pragma warning(disable: 4201) // anon struct/union
#pragma warning(disable: 4054) // typecast from fn ptr

#include <stdio.h>	// FILE *
#include <stdint.h> // int_t's
#include <stdlib.h> // malloc/realloc/free
#include <string.h> // strchr
#include <math.h>	// sin, cos, tan, sqrt



///////////////////////////////////////////////////////////////////////////////
//
//		Types
//

 #ifndef TYPES_H

// Defs for static
#define local		static
#define global		static
#define internal	static

// Unsigned
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;
typedef size_t		usize;

typedef u8			byte;
typedef u16			word;
typedef u32			dword;
typedef u64			qword;

// Signed
typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;

// Floating point
typedef float		f32;
typedef double		f64;

// Boolean
typedef s8			b8;
typedef s32			b32;

// Max values
#define U8_MAX		UINT8_MAX
#define U16_MAX		UINT16_MAX
#define U32_MAX		UINT32_MAX
#define U64_MAX		UINT64_MAX

#define S8_MAX		INT8_MAX
#define S16_MAX		INT16_MAX
#define S32_MAX		INT32_MAX
#define S64_MAX		INT64_MAX

 #endif // TYPES_H



///////////////////////////////////////////////////////////////////////////////
//
//		General
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
	#define MHO_EXTERN	extern "C"
#else
	#define MHO_EXTERN	extern
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
	#define __FILENAME__	(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
	#define __FILENAME__	(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif



///////////////////////////////////////////////////////////////////////////////
//
//		Misc.
//

#define mho_lerp(__a, __b, __t) \
	((__a) + (f32)((__b) - (__a)) * (__t))

#define mho_clamp(__x, __min, __max) \
	((__x) < (__min) ? (__min) : (__x) > (__max) ? (__max) : (__x))

#define mho_swap(__a, __b, __type)		\
	do									\
	{									\
		__type __MHO_SWAP_TEMP = __a;	\
		__a = __b;						\
		__b = __MHO_SWAP_TEMP;			\
	} while (0)

#define mho_min(__a, __b) \
	((__a) < (__b) ? (__a) : (__b))

#define mho_max(__a, __b) \
	((__a) > (__b) ? (__a) : (__b))



///////////////////////////////////////////////////////////////////////////////
//
//		Dynamic Array
//

typedef struct _TAG_mho_arr_header
{
	u32		size;		// Number of elements in array containing data
	u32		capacity;	// Total number of elements
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

// Grows the array (doubles in size)
#define mho_arr_grow(__array) \
	mho_arr_resize((__array), sizeof(*(__array)), mho_arr_capacity(__array) ? mho_arr_capacity(__array) * 2 : 1)

// Grows the array by a given size
#define mho_arr_grow_size(__array, __sz) \
	mho_arr_resize((__array), sizeof(*(__array)), mho_arr_capacity(__array) ? mho_arr_capacity(__array) + (__sz) : 1)

// Pushes data to the back of the array
#define mho_arr_push(__array, __data)										\
	do																		\
	{																		\
		mho_arr_init((void **)&(__array), sizeof(*(__array)));				\
		if (!(__array) || ((__array) && mho_arr_need_grow(__array, 1)))		\
		{																	\
			*((void **)&(__array)) = mho_arr_grow(__array);					\
		}																	\
		(__array)[mho_arr_size(__array)] = (__data);						\
		mho_arr_head(__array)->size++;										\
	} while (0)

// Pops data off the array (decrements the size) and returns it
#define mho_arr_pop(__array) \
	((mho_arr_size(__array) > 0) ? (__array)[(--mho_arr_head(__array)->size)] : (__array)[(mho_arr_head(__array)->size)])

// Clears all the elements in the array, (simply sets size to 0)
#define mho_arr_clear(__array)					\
	do											\
	{											\
		if (__array)							\
			mho_arr_head(__array)->size = 0;	\
	} while (0)

// Reserves internal space in the array for __amount, non-initialized elements
#define mho_arr_reserve(__array, __amount)													\
	do																						\
	{																						\
		if ((!__array))																		\
			mho_arr_init((void **)&(__array), sizeof(*(__array)));							\
		if ((!__array) || (usize)__amount > mho_arr_capacity(__array))						\
		{																					\
			*((void **)&(__array)) = mho_arr_resize(__array, sizeof(*(__array)), __amount); \
		}																					\
	} while (0)

// Initializes the array
MHO_EXTERN void		**mho_arr_init(void *arr, usize val_size);

// Resizes the array
MHO_EXTERN void		*mho_arr_resize(void *arr, usize sz, usize amt);



///////////////////////////////////////////////////////////////////////////////
//
//		Util
//

// Reads a file and writes it into a char * buffer
MHO_EXTERN char		*mho_read_file_buffer(const char *filename);

// Returns the length of a file using the FILE * handle
MHO_EXTERN s64		mho_filelen(FILE *fp);

MHO_EXTERN void		mho_memcpy(void *dest, void *src, usize n);
MHO_EXTERN void		*mho_memset(void *dest, s32 c, usize n);
MHO_EXTERN void		mho_strcpy(s8 *dest, s8 *src);
MHO_EXTERN void		mho_strncpy(s8 *dest, s8 *src, usize n);
MHO_EXTERN usize	mho_strlen(s8 *str);
MHO_EXTERN s8		*mho_strcat(s8 *str1, s8 *str2);

MHO_EXTERN s32		mho_strcmp(s8 *dest, s8 *src);
MHO_EXTERN s32		mho_strncmp(s8 *dest, s8 *src, usize n);


///////////////////////////////////////////////////////////////////////////////
//
//		Math
//

// Useful #define macros + constants
#define mho_sqrt(__n)		sqrtf(__n)
#define mho_sin(__n)		sinf(__n)
#define mho_cos(__n)		cosf(__n)
#define mho_tan(__n)		tanf(__n)
#define mho_cast(__n)		(f32 *)&__n
#define mho_rads(__n)		(__n * 0.017453f)
#define mho_degs(__n)		(__n * 57.29578f)
#ifndef M_PI
	#define M_PI			3.141592653589f
#endif // M_PI

/* ============================ *
 * =====	 Vector2D	  ===== *
 * ============================ */

// Definition of a 2D vector
typedef struct _TAG_mho_vec2
{
	union
	{
		struct
		{
			f32		x,
					y;
		};
		f32			elements[2];
	};
} mho_vec2_t;

// Constructs a 2D vector given x, y components
MHO_EXTERN mho_vec2_t		mho_vec2_ctor(f32 x, f32 y);

// Computes the sum of two 2D vectors
// (v1 + v2)
MHO_EXTERN mho_vec2_t		mho_vec2_add(mho_vec2_t v1, mho_vec2_t v2);

// Computes the difference of two 2D vectors
// (v1 - v2)
MHO_EXTERN mho_vec2_t		mho_vec2_sub(mho_vec2_t v1, mho_vec2_t v2);

// Computes a 2D vector multiplied by a scalar
// (vec * scalar)
MHO_EXTERN mho_vec2_t		mho_vec2_scal(mho_vec2_t vec, f32 scalar);

// Computes the dot product of two 2D vectors
// (v1 . v2)
MHO_EXTERN f32				mho_vec2_dot(mho_vec2_t v1, mho_vec2_t v2);

// Computes the magnitude (norm) of a 2D vector
// (||vec||)
MHO_EXTERN f32				mho_vec2_mag(mho_vec2_t vec);

// Normalizes a given 2D vector
// (vec * (1 / ||vec||))
MHO_EXTERN mho_vec2_t		mho_vec2_normalize(mho_vec2_t vec);

// Computes a 2D vector rotated by an angle in degrees
MHO_EXTERN mho_vec2_t		mho_vec2_rotate(mho_vec2_t vec, f32 angle);

/* ============================ *
 * =====	 Vector3D	  ===== *
 * ============================ */

// Definition of a 3D vector
typedef struct _TAG_mho_vec3
{
	union
	{
		struct
		{
			f32		x,
					y,
					z;
		};
		f32			elements[3];
	};
} mho_vec3_t;

// Constructs a 3D vector given x, y, z components
MHO_EXTERN mho_vec3_t		mho_vec3_ctor(f32 x, f32 y, f32 z);

// Computes the sum of two 3D vectors
// (v1 + v2)
MHO_EXTERN mho_vec3_t		mho_vec3_add(mho_vec3_t v1, mho_vec3_t v2);

// Computes the difference of two 3D vectors
// (v1 - v2)
MHO_EXTERN mho_vec3_t		mho_vec3_sub(mho_vec3_t v1, mho_vec3_t v2);

// Computes a 3D vector multipled by a scalar
// (vec * scalar)
MHO_EXTERN mho_vec3_t		mho_vec3_scal(mho_vec3_t vec, f32 scalar);

// Computes the dot product of two 3D vectors
// (v1 . v2)
MHO_EXTERN f32				mho_vec3_dot(mho_vec3_t v1, mho_vec3_t v2);

// Computes the cross product of two 3D vectors
// (v1 x v2)
MHO_EXTERN mho_vec3_t		mho_vec3_cross(mho_vec3_t v1, mho_vec3_t v2);

// Computes the magnitude (norm) of a 3D vector
// (||vec||)
MHO_EXTERN f32				mho_vec3_mag(mho_vec3_t vec);

// Normalizes a given 3D vector
// (vec * (1 / ||vec||))
MHO_EXTERN mho_vec3_t		mho_vec3_normalize(mho_vec3_t vec);

/* ============================ *
 * =====	 Vector4D	  ===== *
 * ============================ */

// Definition of a 4D vector
typedef struct _TAG_mho_vec4
{
	union
	{
		struct
		{
			f32		x,
					y,
					z,
					w;
		};
		f32			elements[4];
	};
} mho_vec4_t;

// Constructs a 3D vector given x, y, z, w components
MHO_EXTERN mho_vec4_t		mho_vec4_ctor(f32 x, f32 y, f32 z, f32 w);

// Computes the sum of two 4D vectors
// (v1 + v2)
MHO_EXTERN mho_vec4_t		mho_vec4_add(mho_vec4_t v1, mho_vec4_t v2);

// Computes the difference of two 4D vectors
// (v1 - v2)
MHO_EXTERN mho_vec4_t		mho_vec4_sub(mho_vec4_t v1, mho_vec4_t v2);

// Computes a 4D vector multipled by a scalar
// (vec * scalar)
MHO_EXTERN mho_vec4_t		mho_vec4_scal(mho_vec4_t vec, f32 scalar);

// Computes the dot product of two 4D vectors
// (v1 . v2)
MHO_EXTERN f32				mho_vec4_dot(mho_vec4_t v1, mho_vec4_t v2);

// Computes the magnitude (norm) of a 4D vector
// (||vec||)
MHO_EXTERN f32				mho_vec4_mag(mho_vec4_t vec);

// Normalizes a given 4D vector
// (vec * (1 / ||vec||))
MHO_EXTERN mho_vec4_t		mho_vec4_normalize(mho_vec4_t vec);

/* ============================ *
 * =====	 Matrix4	  ===== *
 * ============================ */

// Definition of a 4x4 matrix
typedef struct _TAG_mho_mat4
{
	union
	{
		struct
		{
			mho_vec4_t		col1,
							col2,
							col3,
							col4;
		};
		f32					elements[16];
	};
} mho_mat4_t;

// Returns a 4x4 identity matrix
MHO_EXTERN mho_mat4_t		mho_mat4_identity(void);

// Returns a 4x4 translation matrix given x, y, z components
MHO_EXTERN mho_mat4_t		mho_mat4_translate(f32 x, f32 y, f32 z);

// Returns a 4x4 translation matrix given a 3D vector
MHO_EXTERN mho_mat4_t		mho_mat4_translate_v(mho_vec3_t vec);

// Removes the "translation" component from a 4x4 matrix
// (col4.xyz ---> 0)
MHO_EXTERN mho_mat4_t		mho_mat4_translate_remove(mho_mat4_t matrix);

// Prints the given 4x4 matrix to stdout
MHO_EXTERN void				mho_mat4_print(mho_mat4_t matrix);

// Returns a 4x4 rotation matrix given an angle in degrees and x, y, z components
MHO_EXTERN mho_mat4_t		mho_mat4_rotate(f32 angle, f32 x, f32 y, f32 z);

// Returns a 4x4 rotation matrix given an angle in degrees and 3D vector
MHO_EXTERN mho_mat4_t		mho_mat4_rotate_v(f32 angle, mho_vec3_t vec);

// Returns a 4x4 perspective matrix given an FOV, aspect ratio, and
// near/far plane distance values
MHO_EXTERN mho_mat4_t		mho_mat4_perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far);

// Returns a 4x4 lookat matrix given an eye, center, and up 3D vector
MHO_EXTERN mho_mat4_t		mho_mat4_lookat(mho_vec3_t eye, mho_vec3_t center, mho_vec3_t up);

// Returns a 4x4 scale matrix given a scale factor/value
MHO_EXTERN mho_mat4_t		mho_mat4_scale(f32 scale_value);

// Computes the product of two 4x4 matrices
MHO_EXTERN mho_mat4_t		mho_mat4_mult(mho_mat4_t m1, mho_mat4_t m2);

/* ============================ *
 * =====	   Misc		  ===== *
 * ============================ */

// Pseudo RNG for unsigned 32bit ints
MHO_EXTERN u32		mho_randui(u32 index);

// Pseudo RNG for 32bit floats (ranged 0 to 1)
MHO_EXTERN f32		mho_randf(u32 index);

// Pseudo RNG for 64bit floats (ranged 0 to 1)
MHO_EXTERN f64		mho_randd(u32 index);

// Pseudo RNG for 32bit floats (ranged -1 to 1)
MHO_EXTERN f32		mho_randnf(u32 index);

// Pseudo RNG for 64bit floats (ranged -1 to 1)
MHO_EXTERN f64		mho_randnd(u32 index);

// Quake III square root
MHO_EXTERN f32		mho_fsqrt(f32 number);

// Quake III inverse square root
MHO_EXTERN f32		mho_fsqrtinv(f32 number);



///////////////////////////////////////////////////////////////////////////////
//
//		OpenGL
//

// Builds a GLSL shader program using a vertex + fragment source
MHO_EXTERN u32		mho_load_shader_vf(const char *vs_path, const char *fs_path);

// Builds a GLSL shader program using a compute source
MHO_EXTERN u32		mho_load_shader_comp(const char *cs_path);

// Checks for any compile errors in a given GLSL shader program or source
MHO_EXTERN void		mho_check_compile_errors(u32 data, u8 type, const char *filename);

 #ifdef _glfw3_h_
// Default framebuffer resize callback function for OpenGL + GLFW
MHO_EXTERN void		mho_framebuffer_size_callback(GLFWwindow *window, s32 width, s32 height);

// Default mouse callback function for OpenGL + GLFW
MHO_EXTERN void		mho_mouse_callback(GLFWwindow *window, f64 x_pos, f64 y_pos);

 #endif

// Obj Data Structure
typedef struct _TAG_mho_obj_data
{
	mho_arr(mho_vec3_t)		vertices;
	mho_arr(mho_vec3_t)		normals;
	mho_arr(mho_vec2_t)		uvs;
	mho_arr(u32)			indices;
	b32						interleaved;
} mho_obj_data_t;

MHO_EXTERN b32		mho_load_obj(const char *file, mho_obj_data_t *data, b32 interleaved);

MHO_EXTERN b32		mho_load_obj_inter(const char *file, mho_obj_data_t *data);

MHO_EXTERN b32		mho_load_obj_index(const char *file, mho_obj_data_t *data);



///////////////////////////////////////////////////////////////////////////////
//
//		Debugging
//

// Record structure for storing memory information
typedef struct _TAG_mho_mem_rec
{
	void		*ptr;
	char		*file,
				*df_file;
	int			line,
				df_line,
				size;
	byte		flags;
	struct		_TAG_mho_mem_rec *next;
} mho_mem_rec_t;

// Custom malloc impl for debugging
MHO_EXTERN void		*mho_mem_malloc(size_t size, const char *file, int line);

// Custom free impl for debugging
MHO_EXTERN void		mho_mem_free(void *buffer, const char *file, int line);

// Prints a memory debugging report to a specified stream
MHO_EXTERN void		mho_mem_print(FILE *stream);

// Adds a new memory record to the debug list
MHO_EXTERN void		mho_mem_rec_append(void *ptr, const char *file, int line, int size);

// Verifies memory integrity of list (searches for over/underruns)
MHO_EXTERN void		mho_mem_debug_memory();



///////////////////////////////////////////////////////////////////////////////
//																			 //
// ---------------------- MHO IMPLEMENTATION ------------------------------- //
//																			 //
///////////////////////////////////////////////////////////////////////////////

// #define this symbol in exactly 1 .c/.cpp file
// before including this file!
// For example:
//			#include <...>
//			#include <...>
//			#include <...>
//
//			#define MHO_IMPL
//			#include <mho.h>

 #ifdef MHO_IMPL

//////////////////////////////////////////////////////////////////
// Dynamic Array

void **
mho_arr_init(void **arr,
			 usize val_size)
{
	mho_arr_header_t	*data;

	if (*arr == NULL)
	{
		data = (mho_arr_header_t *)malloc(val_size + sizeof(mho_arr_header_t));

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
mho_arr_resize(void *arr,
			   usize sz,
			   usize amt)
{
	usize				capacity;
	mho_arr_header_t	*data;

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


//////////////////////////////////////////////////////////////////
// Utils

char *
mho_read_file_buffer(const char *filename)
{
	FILE	*fptr;
	s32		file_len,
			ret;
	char	*source = NULL;

	fptr = fopen(filename, "rb");
	if (fptr)
	{
		file_len = mho_filelen(fptr);
		if (file_len != -1)
		{
			source = (char *)malloc(file_len + 1);
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

s64
mho_filelen(FILE *fp)
{
	s64		len,
			pos;

	pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return len;
}

void
mho_memcpy(void *dest,
		   void *src,
		   usize n)
{
	usize	i;

	for (i = 0; i < n; i++)
	{
		((s8 *)dest)[i] = ((s8 *)src)[i];
	}
}

void *
mho_memset(void *dest,
		   s32 c,
		   usize n)
{
	u8		uc = (u8)c;
	usize	i;

	for (i = 0; i < n; i++)
	{
		((u8 *)dest)[i] = uc;
	}

	return dest;
}

void
mho_strcpy(s8 *dest,
		   s8 *src)
{
	while (*dest != 0)
	{
		*dest++ = *src++;
	}
}

void
mho_strncpy(s8 *dest,
			s8 *src,
			usize n)
{
	usize	i;

	for (i = 0; i < n; i++)
	{
		dest[i] = src[i];
	}
}

usize
mho_strlen(s8 *str)
{
	usize	len = 0;

	while (*str++ != '\0')
	{
		len++;
	}

	return len;
}

s8 *
mho_strcat(s8 *str1,
		   s8 *str2)
{
	u32		size1,
			size2,
			index,
			i;
	s8		*new_str;

	for (size1 = 0; str1[size1] != '\0'; size1++);
	for (size2 = 0; str2[size2] != '\0'; size2++);

	new_str = (s8 *)malloc(size1 + size2);

	for (index = 0; index < size1; index++)
		new_str[index] = str1[index];
	for (i = 0; index < (size1 + size2); index++, i++)
		new_str[index] = str2[i];

	return new_str;
}

//////////////////////////////////////////////////////////////////
// OpenGL

// glad.h is the OpenGL function loader that I primarily use; this can
// be easily swapped by just replacing this symbol with your respective
// loader's symbol in the IMPLEMENTATION section.
  #ifdef __glad_h_

u32
mho_load_shader_vf(const char *vs_path,
				   const char *fs_path)
{
	u32		vertex,
			fragment,
			program;
	char	*source;

	// Vertex Shader
	source = mho_read_file_buffer(vs_path);
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &source, NULL);
	glCompileShader(vertex);
	mho_check_compile_errors(vertex, 0, vs_path);
	free(source);

	// Fragment Shader
	source = mho_read_file_buffer(fs_path);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &source, NULL);
	glCompileShader(fragment);
	mho_check_compile_errors(fragment, 0, fs_path);
	free(source);

	// Shader Program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	mho_check_compile_errors(program, 1, NULL);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}

u32
mho_load_shader_comp(const char *cs_path)
{
	u32		compute,
			program;
	char	*source;

	// Compute Shader
	source = mho_read_file_buffer(cs_path);
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &source, NULL);
	glCompileShader(compute);
	mho_check_compile_errors(compute, 0, cs_path);
	free(source);

	// Shader Program
	program = glCreateProgram();
	glAttachShader(program, compute);
	glLinkProgram(program);
	mho_check_compile_errors(program, 1, NULL);

	glDeleteShader(compute);

	return program;
}

void
mho_check_compile_errors(u32 data,
						 u8 type,
						 const char *filename)
{
	b32		success;
	char	info_log[1024];

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


   #ifdef _glfw3_h_
/* void */
/* mho_framebuffer_size_callback(GLFWwindow *window, */
/*							  s32 width, */
/*							  s32 height) */
/* { */
/*	glViewport(0, 0, width, height); */
/* } */

/*
void
mho_mouse_callback(GLFWwindow *window,
				   f64 x_pos,
				   f64 y_pos)
{
	local b32 first_mouse = TRUE;
	local f32 yaw = -90.0f;
	local f32 pitch = 0.0f;
	local f64 last_x = 400.0f;
	local f64 last_y = 300.0f;
	mho_vec3_t direction;
	f32 x_offset = x_pos - last_x;
	f32 y_offset = last_y - y_pos;
	const f32 sens = 0.1f;

	if (first_mouse)
	{
		last_x = x_pos;
		last_y = y_pos;
		first_mouse = FALSE;
	}

	last_x = x_pos;
	last_y = y_pos;

	x_offset *= sens;
	y_offset *= sens;

	yaw += x_offset;
	pitch += y_offset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	direction.x = mho_cos(mho_rads(yaw)) * mho_cos(mho_rads(pitch));
	direction.y = mho_sin(mho_rads(pitch));
	direction.z = mho_sin(mho_rads(yaw)) * mho_cos(mho_rads(pitch));

	// assuming a 'camera' is defined globally
	// camera.front = vec3_normalize(direction);
}
*/
   #endif // _glfw3_h_

  #endif // __glad_h_


b32
mho_load_obj(const char *file,
			 mho_obj_data_t *data,
			 b32 interleaved)
{
	/* if (interleaved) */
	/* { */
	/*	return mho_obj_load_inter(file, data); */
	/* } */
	/* else */
	/* { */
	/*	return m_obj_load_indexed(file, data); */
	/* } */

	 return FALSE;
}

b32
mho_load_obj_inter(const char *file,
				   mho_obj_data_t *data)
{
	FILE				*fptr;
	u32					vert_idx[3],
						uv_idx[3],
						norm_idx[3],
						i;
	mho_vec2_t			temp2d;
	mho_vec3_t			temp3d;
	mho_arr(mho_vec3_t) verts = NULL;
	mho_arr(mho_vec3_t) norms = NULL;
	mho_arr(mho_vec2_t) uvs = NULL;
	mho_arr(u32)		vert_inds = NULL;
	mho_arr(u32)		uv_inds = NULL;
	mho_arr(u32)		norm_inds = NULL;

	fptr = fopen(file, "r");
	if (!fptr)
	{
		printf("Failed to load file: %s\n", file);

		return FALSE;
	}

	for (;;)
	{
		s8 token[128];
		s32 res;

		res = fscanf(fptr, "%s", token);
		if (res == EOF)
			break;

		if (token[0] == '#')
			continue;
		else if (token[0] == 'v')
		{
			if (token[1] == 't')
			{
				fscanf(fptr, "%f %f\n", &temp2d.x, &temp2d.y);
				mho_arr_push(uvs, temp2d);
			}
			else if (token[1] == 'n')
			{
				fscanf(fptr, "%f %f %f\n", &temp3d.x, &temp3d.y, &temp3d.z);
				mho_arr_push(norms, temp3d);
			}
			else if (token[1] == 'f')
			{
				res = fscanf(fptr, "%d/%d/%d", &vert_idx[0], &uv_idx[0], &norm_idx[0]);
				res += fscanf(fptr, "%d/%d/%d", &vert_idx[1], &uv_idx[1], &norm_idx[1]);
				res += fscanf(fptr, "%d/%d/%d", &vert_idx[2], &uv_idx[2], &norm_idx[2]);

				if (res != 9)
				{
					printf("File is incompatible with mho_obj!: %s\n", file);
					fclose(fptr);

					return FALSE;
				}

				mho_arr_push(vert_inds, vert_idx[0]);
				mho_arr_push(vert_inds, vert_idx[1]);
				mho_arr_push(vert_inds, vert_idx[2]);
				mho_arr_push(uv_inds, uv_idx[0]);
				mho_arr_push(uv_inds, uv_idx[1]);
				mho_arr_push(uv_inds, uv_idx[2]);
				mho_arr_push(norm_inds, norm_idx[0]);
				mho_arr_push(norm_inds, norm_idx[1]);
				mho_arr_push(norm_inds, norm_idx[2]);
			}
			else
			{
				fscanf(fptr, "%f %f %f\n", &temp3d.x, &temp3d.y, &temp3d.z);
				mho_arr_push(verts, temp3d);
			}
		}
	}

	data->vertices = NULL;
	data->normals = NULL;
	data->uvs = NULL;

	for (i = 0; i < mho_arr_size(vert_inds); i++)
	{
		u32 v = vert_inds[i];
		u32 n = norm_inds[i];
		u32 u = uv_inds[i];

		mho_vec3_t vertex = verts[v - 1];
		mho_vec3_t normal = norms[n - 1];
		mho_vec2_t uv = uvs[u - 1];

		mho_arr_push(data->vertices, vertex);
		mho_arr_push(data->normals, normal);
		mho_arr_push(data->uvs, uv);
	}
	data->interleaved = TRUE;

	mho_arr_free(verts);
	mho_arr_free(norms);
	mho_arr_free(uvs);
	mho_arr_free(vert_inds);
	mho_arr_free(uv_inds);
	mho_arr_free(norm_inds);
	fclose(fptr);

	return TRUE;
}

/* MHO_EXTERN b32 mho_load_obj_index(const char *file, mho_obj_data_t *data); */

//////////////////////////////////////////////////////////////////
// Math

/////////////////////////////
// VECTOR2D IMPLEMENTATION
mho_vec2_t
mho_vec2_ctor(f32 x,
			  f32 y)
{
	mho_vec2_t	vec;

	vec.x = x;
	vec.y = y;

	return vec;
}

mho_vec2_t
mho_vec2_add(mho_vec2_t v1,
			 mho_vec2_t v2)
{
	v1.x += v2.x;
	v1.y += v2.y;

	return v1;
}

mho_vec2_t
mho_vec2_sub(mho_vec2_t v1,
			 mho_vec2_t v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;

	return v1;
}

mho_vec2_t
mho_vec2_scal(mho_vec2_t vec,
			  f32 scalar)
{
	vec.x *= scalar;
	vec.y *= scalar;

	return vec;
}

f32
mho_vec2_dot(mho_vec2_t v1,
			 mho_vec2_t v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y));
}

f32
mho_vec2_mag(mho_vec2_t vec)
{
	return mho_sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

mho_vec2_t
mho_vec2_normalize(mho_vec2_t vec)
{
	f32		val;

	val = mho_fsqrtinv((vec.x * vec.x) + (vec.y * vec.y));
	vec.x *= val;
	vec.y *= val;

	return vec;
}

mho_vec2_t
mho_vec2_rotate(mho_vec2_t vec,
				f32 angle)
{
	f32		c,
			s;

	c = mho_cos(mho_rads(angle));
	s = mho_sin(mho_rads(angle));
	vec.x = ((vec.x * c) - (vec.y * s));
	vec.y = ((vec.x * s) + (vec.y * c));

	return vec;
}

//////////////////////////////
// VECTOR3D IMPLEMENTATION

mho_vec3_t
mho_vec3_ctor(f32 x,
			  f32 y,
			  f32 z)
{
	mho_vec3_t	vec;

	vec.x = x;
	vec.y = y;
	vec.z = z;

	return vec;
}

mho_vec3_t
mho_vec3_add(mho_vec3_t v1,
			 mho_vec3_t v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;

	return v1;
}

mho_vec3_t
mho_vec3_sub(mho_vec3_t v1,
			 mho_vec3_t v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;

	return v1;
}

mho_vec3_t
mho_vec3_scal(mho_vec3_t vec,
			  f32 scalar)
{
	vec.x *= scalar;
	vec.y *= scalar;
	vec.z *= scalar;

	return vec;
}

f32
mho_vec3_dot(mho_vec3_t v1,
			 mho_vec3_t v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

mho_vec3_t
mho_vec3_cross(mho_vec3_t v1,
			   mho_vec3_t v2)
{
	mho_vec3_t	cross_prod;

	cross_prod.x = (v1.y * v2.z) - (v1.z * v2.y);
	cross_prod.y = (v1.z * v2.x) - (v1.x * v2.z);
	cross_prod.z = (v1.x * v2.y) - (v1.y * v2.x);

	return cross_prod;
}

f32
mho_vec3_mag(mho_vec3_t vec)
{
	return mho_sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

mho_vec3_t
mho_vec3_normalize(mho_vec3_t vec)
{
	f32		val;

	val = mho_fsqrtinv((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
	vec.x *= val;
	vec.y *= val;
	vec.z *= val;

	return vec;
}

//////////////////////////////
// VECTOR4D IMPLEMENTATION


mho_vec4_t
mho_vec4_ctor(f32 x,
			  f32 y,
			  f32 z,
			  f32 w)
{
	mho_vec4_t	vec;

	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.w = w;

	return vec;
}

mho_vec4_t
mho_vec4_add(mho_vec4_t v1,
			 mho_vec4_t v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;

	return v1;
}

mho_vec4_t
mho_vec4_sub(mho_vec4_t v1,
			 mho_vec4_t v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;

	return v1;
}

mho_vec4_t
mho_vec4_scal(mho_vec4_t vec,
			  f32 scalar)
{
	vec.x *= scalar;
	vec.y *= scalar;
	vec.z *= scalar;
	vec.z *= scalar;

	return vec;
}

f32
mho_vec4_dot(mho_vec4_t v1,
			 mho_vec4_t v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w));
}

f32
mho_vec4_mag(mho_vec4_t vec)
{
	return mho_sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w));
}

mho_vec4_t
mho_vec4_normalize(mho_vec4_t vec)
{
	f32		val;

	val = mho_fsqrtinv((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w));
	vec.x *= val;
	vec.y *= val;
	vec.z *= val;
	vec.w *= val;

	return vec;
}

//////////////////////////////
// MATRIX4 IMPLEMENTATION

mho_mat4_t
mho_mat4_identity(void)
{
	mho_mat4_t	matrix = {0};

	matrix.col1.x = 1.0f;
	matrix.col2.y = 1.0f;
	matrix.col3.z = 1.0f;
	matrix.col4.w = 1.0f;

	return matrix;
}

mho_mat4_t
mho_mat4_translate(f32 x,
				   f32 y,
				   f32 z)
{
	mho_mat4_t	matrix;

	matrix = mho_mat4_identity();
	matrix.col4.x = x;
	matrix.col4.y = y;
	matrix.col4.z = z;

	return matrix;
}

mho_mat4_t
mho_mat4_translate_v(mho_vec3_t vec)
{
	mho_mat4_t	matrix;

	matrix = mho_mat4_identity();
	matrix.col4.x = vec.x;
	matrix.col4.y = vec.y;
	matrix.col4.z = vec.z;

	return matrix;
}

mho_mat4_t
mho_mat4_translate_remove(mho_mat4_t matrix)
{
	matrix.col4.x = 0.0f;
	matrix.col4.y = 0.0f;
	matrix.col4.z = 0.0f;

	return matrix;
}

void
mho_mat4_print(mho_mat4_t matrix)
{
	u8	i,
		j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			printf("%f ", matrix.elements[i + j * 4]);
		}
		printf("\n");
	}
}

mho_mat4_t
mho_mat4_rotate(f32 angle,
				f32 x,
				f32 y,
				f32 z)
{
	f32			c,
				s,
				c1;
	mho_vec3_t	vec = {x, y, z};
	mho_mat4_t	matrix = {0};

	c = mho_cos(mho_rads(angle));
	s = mho_sin(mho_rads(angle));
	c1 = 1.0f - c;
	vec = mho_vec3_normalize(vec);

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

mho_mat4_t
mho_mat4_rotate_v(f32 angle,
				  mho_vec3_t vec)
{

	f32			c,
				s,
				c1;
	mho_mat4_t	matrix = {0};

	c = mho_cos(mho_rads(angle));
	s = mho_sin(mho_rads(angle));
	c1 = 1.0f - c;
	vec = mho_vec3_normalize(vec);

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

mho_mat4_t
mho_mat4_perspective(f32 fov,
					 f32 aspect_ratio,
					 f32 near,
					 f32 far)
{
	f32			t,
				fdelta;
	mho_mat4_t	matrix = {0};

	t = mho_tan(mho_rads(fov) / 2.0f);
	fdelta = far - near;

	matrix.col1.x = 1 / (aspect_ratio * t);

	matrix.col2.y = 1 / t;

	matrix.col3.z = -1 * ((far + near) / fdelta);
	matrix.col3.w = -1;

	matrix.col4.z = ((-2.0f * far * near) / fdelta);

	return matrix;
}

mho_mat4_t
mho_mat4_lookat(mho_vec3_t eye,
				mho_vec3_t center,
				mho_vec3_t up)
{
	mho_mat4_t	matrix;
	mho_vec3_t	f,
				s,
				u;

	f = mho_vec3_normalize(mho_vec3_sub(center, eye));
	s = mho_vec3_normalize(mho_vec3_cross(f, up));
	u = mho_vec3_cross(s, f);

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

	matrix.col4.x = -mho_vec3_dot(s, eye);
	matrix.col4.y = -mho_vec3_dot(u, eye);
	matrix.col4.z = mho_vec3_dot(f, eye);
	matrix.col4.w = 1.0f;

	return matrix;
}

mho_mat4_t
mho_mat4_scale(f32 scale_value)
{
	mho_mat4_t	matrix = {0};

	matrix.col1.x = scale_value;
	matrix.col2.y = scale_value;
	matrix.col3.z = scale_value;
	matrix.col4.w = 1.0f;

	return matrix;
}

mho_mat4_t
mho_mat4_mult(mho_mat4_t m1,
			  mho_mat4_t m2)
{
	mho_mat4_t	res;
	u8			y,
				x,
				e;
	f32			sum;

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			sum = 0.0f;
			for (e = 0; e < 4; e++)
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
mho_randui(u32 index)
{
	index = (index << 13) ^ index;
	return ((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF);
}

f32
mho_randf(u32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0f) * 0.5f;
}

f64
mho_randd(u32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0) * 0.5;
}

f32
mho_randnf(u32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0f) - 1.0f;
}

f64
mho_randnd(u32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0) - 1.0;
}

f32
mho_fsqrt(f32 number)
{
	f32		x,
			y;
	s32		i;

	x = number * 0.5f;
	y = number;
	i = *(s32 *)&y;					// evil floating point bit hack
	i = 0x5F3759DF - (i >> 1);		// what the fuck?
	y = *(f32 *)&i;
	y = y * (1.5f - (x * y * y));	// 1st iteration
	y = y * (1.5f - (x * y * y));	// 2nd iteration

	return number * y;				// mulitply by original num to reverse and get sqrt
}

f32
mho_fsqrtinv(f32 number)
{
	f32		x,
			y;
	s32		i;

	x = number * 0.5f;
	y = number;
	i = *(s32 *)&y;					// evil floating point bit hack
	i = 0x5F3759DF - (i >> 1);		// what the fuck?
	y = *(f32 *)&i;
	y = y * (1.5f - (x * y * y));	// 1st iteration
	y = y * (1.5f - (x * y * y));	// 2nd iteration

	return y;
}



//////////////////////////////////////////////////////////////////
// Debugging

// Important #define constants we use
#define MHO_MEM_TRUE			  1
#define MHO_MEM_FALSE			  0
#define MHO_MEM_FREE_BIT		  0x01
#define MHO_MEM_OVER_BIT		  0x02
#define MHO_MEM_UNDER_BIT		  0x04
#define MHO_MEM_DOUBLE_FREE_BIT   0x08
#define MHO_MEM_OVER_NUM		  0x192BA3A2
#define MHO_MEM_UNDER_NUM		  0x39D7A5DA

global s32				mho_mem_malloc_cnt;
global s32				mho_mem_free_cnt;
global usize			mho_mem_total_alloc;
global mho_mem_rec_t	*mho_mem_alloc_head = NULL;

void *
mho_mem_malloc(size_t size,
			   const char *file,
			   int line)
{
	void	*ptr = NULL;
	dword	*buff_un,
			*buff_ov;

	buff_un = (dword *)malloc(size + 2 * sizeof(dword));

	// if allocation succeeded
	if (buff_un)
	{
		ptr = buff_un + 1;
		buff_ov = (dword *)((byte *)ptr + size);
		*buff_ov = MHO_MEM_OVER_NUM;
		*buff_un = MHO_MEM_UNDER_NUM;

		mho_mem_malloc_cnt++;
		mho_mem_total_alloc += size;
		mho_mem_rec_append(ptr, file, line, size);
	}

	// return ptr regardless
	return ptr;
}

void
mho_mem_free(void *buffer,
			 const char *file,
			 int line)
{
	mho_mem_rec_t	*temp;
	dword			value;
	void			*p;

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
mho_mem_rec_append(void *ptr,
				   const char *file,
				   int line,
				   int size)
{
	mho_mem_rec_t	*new_node,
					*temp;

	// Allocate and fill new node
	new_node = (mho_mem_rec_t *)malloc(sizeof(mho_mem_rec_t));
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
	mho_mem_rec_t	*temp;
	dword			value;
	void			*p;

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

void
mho_mem_print(FILE *stream)
{
	mho_mem_rec_t	*temp;
	void			*p;

	mho_mem_debug_memory();

	fprintf(stream, "\n=========================================================\n");
	fprintf(stream, "					 DEBUG REPORT\n");
	fprintf(stream, "=========================================================\n");
	fprintf(stream, "Total Mallocs: %d\n", mho_mem_malloc_cnt);
	fprintf(stream, "Total Frees:	%d\n", mho_mem_free_cnt);
	fprintf(stream, "Total Size:	%d bytes\n\n", mho_mem_total_alloc);

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
			fprintf(stream, "DOUBLE FREE:	   0x%p (%s (%d))\n", temp->ptr,
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
	fprintf(stream, "					 END OF REPORT\n");
	fprintf(stream, "=========================================================\n");
}

 #endif // MHO_IMPL

// Wraps malloc() and free()
 #ifdef MHO_MEM_DEBUG
	#define malloc(__n)		mho_mem_malloc(__n, __FILENAME__, __LINE__)
	#define free(__n)		mho_mem_free(__n, __FILENAME__, __LINE__)
 #endif // M_MEM_DEBUG

// Simplifies Math related names for types and functions
 #ifndef MHO_MATH_COMPLEX_NAMES
	typedef mho_vec2_t					vec2_t;
	typedef mho_vec3_t					vec3_t;
	typedef mho_vec4_t					vec4_t;
	typedef mho_mat4_t					mat4_t;

	#define vec2_ctor					mho_vec2_ctor
	#define vec2_add					mho_vec2_add
	#define vec2_sub					mho_vec2_sub
	#define vec2_scal					mho_vec2_scal
	#define vec2_dot					mho_vec2_dot
	#define vec2_mag					mho_vec2_mag
	#define vec2_normalize				mho_vec2_normalize
	#define vec2_rotate					mho_vec2_rotate

	#define vec3_ctor					mho_vec3_ctor
	#define vec3_add					mho_vec3_add
	#define vec3_sub					mho_vec3_sub
	#define vec3_scal					mho_vec3_scal
	#define vec3_dot					mho_vec3_dot
	#define vec3_cross					mho_vec3_cross
	#define vec3_mag					mho_vec3_mag
	#define vec3_normalize				mho_vec3_normalize

	#define mat4_identity				mho_mat4_identity
	#define mat4_translate				mho_mat4_translate
	#define mat4_translate_v			mho_mat4_translate_v
	#define mat4_translate_remove		mho_mat4_translate_remove
	#define mat4_print					mho_mat4_print
	#define mat4_rotate					mho_mat4_rotate
	#define mat4_rotate_v				mho_mat4_rotate_v
	#define mat4_perspective			mho_mat4_perspective
	#define mat4_lookat					mho_mat4_lookat
	#define mat4_scale					mho_mat4_scale
	#define mat4_mult					mho_mat4_mult

 #endif // MHO_MATH_COMPLEX_NAMES

#pragma warning(default: 4996) // fopen
#pragma warning(default: 4055) // fn ptr void * (enabled on +MSVC 14)
#pragma warning(default: 4021) // anon struct/union
#pragma warning(default: 4054) // typecast from fn ptr

#endif // MHO_H

