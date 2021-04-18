#ifndef MHO_OBJ_H
#define MHO_OBJ_H

#include <types.h>
#include <stdlib.h>

// --------- Essential structures for this library ------------- //

// 2D Vector
typedef struct _TAG_m_vec2
{
	f32 x, y;
} m_vec2_t;

// 3D Vector
typedef struct _TAG_m_vec3
{
	f32 x, y, z;
} m_vec3_t;

// Obj Data
typedef struct _TAG_m_obj_data
{
	m_vec3_t *vertices, *normals;
	m_vec2_t *uvs;
	u32 *indices;
	b32 interleaved;
} m_obj_data_t;

b32 load_obj(const char *file, m_obj_data_t *data, b32 interleaved);

#endif // MHO_OBJ_H
