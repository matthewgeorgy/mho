#ifndef MHO_OBJ_H
#define MHO_OBJ_H

#include <types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MHO_ARRAY_IMPL
#include "mho_array.h"

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

b32 m_obj_load(const char *file, m_obj_data_t *data, b32 interleaved);
b32 m_obj_load_interleaved(const char *file, m_obj_data_t *data);
b32 m_obj_load_inter(const char *file, m_obj_data_t *data);
b32 m_obj_load_indexed(const char *file, m_obj_data_t *data);

//////////////////////////////////
// --------- IMPL ---------- //

 #ifdef MHO_OBJ_IMPL
#pragma warning(disable: 4996)

b32
m_obj_load(const char *file,
		   m_obj_data_t *data,
		   b32 interleaved)
{
	if (interleaved)
	{
		return m_obj_load_interleaved(file, data);
	}
	else
	{
		//return m_obj_load_indexed(file, data);
	}
}

b32
m_obj_load_interleaved(const char *file,
					   m_obj_data_t *data)
{
	FILE		*fptr;
	u32			nv,
				nvt,
				nvn,
				nf,
				i;
	u32			*vertex_indices,
				*uv_indices,
				*normal_indices;
	u32 		v_idx,
				vt_idx,
				vn_idx,
				f_idx;
	m_vec3_t 	*temp_verts,
				 *temp_norms;
	m_vec2_t	 *temp_uvs;
	char 		token[128];
	s32 		res;
		
	fptr = fopen(file, "r");
	nv = 0, nvt = 0, nvn = 0, nf = 0;
	// Parse the file
	// Count number of vertices, normals, uvs, faces
	fseek(fptr, 0, SEEK_SET);
	while (1)
	{
		// Read start of file
		res = fscanf(fptr, "%s", token);
		if (res == EOF)
			break;

		// Count	
		if (token[0] == '#')
			continue;
		else if (token[0] == 'v')
		{
			if (token[1] == 't')
				nvt++;
			else if (token[1] == 'n')
				nvn++;
			else if (token[1] == 'f')
				nf++;
			else;
				nv++;
		}
	}
	printf("%d %d %d %d\n", nv, nvt, nvn, nf);

	// Allocate our buffers
	temp_verts = (m_vec3_t *)malloc(nv * sizeof(m_vec3_t));
	temp_norms = (m_vec3_t *)malloc(nvn * sizeof(m_vec3_t));
	temp_uvs = (m_vec2_t *)malloc(nvt * sizeof(m_vec2_t));
	vertex_indices = (u32 *)malloc(nf * sizeof(u32));
	uv_indices = (u32 *)malloc(nf * sizeof(u32));
	normal_indices = (u32 *)malloc(nf * sizeof(u32));

	// Set counters for indexing
	v_idx = 0, vt_idx = 0, vn_idx = 0, f_idx = 0;

	// Read data
	/* fseek(fptr, 0, SEEK_SET); */
	/* while (1) */
	/* { */
	/* 	char token[16]; */
	/* 	s32 res = fscanf(fptr, "%s\n", token); */
	/* 	if (res == EOF) */
	/* 		break; */

	/* 	if (strcmp(token, "#") == 0) */
	/* 		continue; */
	/* 	else if (strcmp(token, "v") == 0) */
	/* 	{ */
	/* 		fscanf(fptr, "%f %f %f\n", &temp_verts[v_idx++], */
	/* 								   &temp_verts[v_idx++], */
	/* 								   &temp_verts[v_idx++]); */
	/* 	} */
	/* 	else if (strcmp(token, "vt") == 0) */
	/* 	{ */
	/* 		fscanf(fptr, "%f %f\n", &temp_uvs[vt_idx++], */
	/* 								&temp_uvs[vt_idx++]); */
	/* 	} */
	/* 	else if (strcmp(token, "vn") == 0) */
	/* 	{ */
	/* 		fscanf(fptr, "%f %f %f\n", &temp_norms[vn_idx++], */
	/* 								   &temp_norms[vn_idx++], */
	/* 								   &temp_norms[vn_idx++]); */
	/* 	} */
	/* 	else if (strcmp(token, "f") == 0) */
	/* 	{ */
	/* 		s32 num_vals; */
	/* 		num_vals = fscanf(fptr, "%d/%d/%d", &vertex_indices[f_idx], */
	/* 											&uv_indices[f_idx], */
	/* 											&normal_indices[f_idx++]); */
	/* 		num_vals += fscanf(fptr, "%d/%d/%d", &vertex_indices[f_idx], */
	/* 											 &uv_indices[f_idx], */
	/* 											 &normal_indices[f_idx++]); */
	/* 		num_vals += fscanf(fptr, "%d/%d/%d\n", &vertex_indices[f_idx], */
	/* 											   &uv_indices[f_idx], */
	/* 											   &normal_indices[f_idx++]); */
	/* 		if (num_vals != 9) */
	/* 		{ */
	/* 			printf("Failed to read file! (faces): %s\n", file); */
	/* 			fclose(fptr); */
	/* 			return FALSE; */
	/* 		} */
	/* 	} */
	/* } */

	/* for (i = 0; i < nf; i++) */
	/* { */
	/* 	v_idx = vertex_indices[i]; */
	/* 	vt_idx = uv_indices[i]; */
	/* 	vn_idx = normal_indices[i]; */

	/* 	data->vertices[i] = temp_verts[v_idx]; */
	/* 	data->uvs[i] = temp_uvs[vt_idx]; */
	/* 	data->normals[i] = temp_norms[vn_idx]; */
	/* } */
	/* data->interleaved = TRUE; */

	return TRUE;
}

b32
m_obj_load_inter(const char *file,
				 m_obj_data_t *data)
{
	FILE *fptr;
	m_vec3_t *verts, *norms;
	m_vec2_t *uvs;
	char token[128];
	s32 res, num_vals;
	u32 vert_idx[3], uv_idx[3], norm_idx[3];
	m_vec3_t temp;
	m_vec2_t temp2;

	fptr = fopen(file, "r");
	if (!fptr)
	{
		printf("Failed to load file: %s\n", file);

		return FALSE;
	}

	verts = NULL;
	norms = NULL;
	uvs = NULL;
	for (;;)
	{
		res = fscanf(fptr, "%s", token);
		if (res == EOF)
			break;

		if (token[0] == '#')
			continue;
		else if (token[0] == 'v')
		{
			if (token[1] == 't')
			{
				fscanf(fptr, "%f %f\n", &temp2.x, &temp2.y);
				m_array_push(uvs, temp2);
			}
			else if (token[1] == 'n')
			{
				fscanf(fptr, "%f %f %f\n", &temp.x, &temp.y, &temp.z);
				m_array_push(norms, temp);
			}
			else if (token[1] == 'f')
			{
				num_vals = 0;
				num_vals = fscanf(fptr, "%d/%d/%d", &vert_idx[0],
													&uv_idx[0],
													&norm_idx[0]);
				num_vals += fscanf(fptr, "%d/%d/%d", &vert_idx[1],
													 &uv_idx[1],
													 &norm_idx[1]);
				num_vals += fscanf(fptr, "%d/%d/%d", &vert_idx[2],
													 &uv_idx[2],
													 &norm_idx[2]);
				if (num_vals != 9)
				{
					printf("File is incompatible with mho_obj!: %s\n", file);
					fclose(fptr);

					return FALSE;
				}
				
				m_array_push(verts, vert_idx[0]);
				m_array_push(verts, vert_idx[1]);
				m_array_push(verts, vert_idx[2]);
				m_array_push(verts, uv_idx[0]);
				m_array_push(verts, uv_idx[1]);
				m_array_push(verts, uv_idx[2]);
				m_array_push(verts, norm_idx[0]);
				m_array_push(verts, norm_idx[1]);
				m_array_push(verts, norm_idx[2]);
			}
			else
			{
				fscanf(fptr, "%f %f %f\n", &temp.x, &temp.y, &temp.z);
				m_array_push(verts, temp);
			}
		}
	}

	return TRUE;
}
	
#pragma warning(default: 4996)

 #endif // MHO_OBJ_IMPL

#endif // MHO_OBJ_H
