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

b32 m_load_obj(const char *file, m_obj_data_t *data, b32 interleaved);

//////////////////////////////////
// --------- IMPL ---------- //

 #ifdef MHO_OBJ_IMPL
#pragma warning(disable: 4996)

b32
m_load_obj(const char *file,
           m_obj_data_t *data,
           b32 interleaved)
{
    
    FILE *fptr = fopen(file, "r");

    if (!fptr)
    {
        printf("Could not load file!: %s\n", file);

        return FALSE;
    }

    else
    {
        u32 nv = 0,
            nvt = 0,
            nvn = 0,
            nf = 0,
            i;
        u32 *vertex_indices,
            *uv_indices,
            *normal_indices;
        u32 v_idx, vt_idx, vn_idx, f_idx;
        m_vec3_t *temp_verts,
                 *temp_norms;
        m_vec2_t *temp_uvs;
            
        // Parse the file

        // Count number of vertices, normals, uvs, faces
        while (1)
        {
            // Read start of file
            char token[16];
            s32 res = fscanf(fptr, "%s\n", token);
            if (res == EOF)
                break;

            // Count    
            if (strcmp(token, "#") == 0)
                continue;
            else if (strcmp(token, "v") == 0)
                nv++;
            else if (strcmp(token, "vt") == 0)
                nvt++;
            else if (strcmp(token, "vn") == 0)
                nvn++;
            else if (strcmp(token, "f") == 0)
                nf++;
        }

        // Allocate our buffers
        temp_verts = (m_vec3_t *)malloc(nv * sizeof(m_vec3_t));
        temp_norms = (m_vec3_t *)malloc(nvn * sizeof(m_vec3_t));
        temp_uvs = (m_vec2_t *)malloc(nvt * sizeof(m_vec2_t));
        vertex_indices = (u32 *)malloc(nf * sizeof(u32));
        uv_indices = (u32 *)malloc(nf * sizeof(u32));
        normal_indices = (u32 *)malloc(nf * sizeof(u32));

        // Set counters for indexing
        v_idx = 0;
        vt_idx = 0;
        vn_idx = 0;
        f_idx = 0;

        // Read data
        fseek(fptr, 0, SEEK_SET);
        while (1)
        {
            char token[16];
            s32 res = fscanf(fptr, "%s\n", token);
            if (res == EOF)
                break;

            if (strcmp(token, "#") == 0)
                continue;
            else if (strcmp(token, "v") == 0)
            {
                fscanf(fptr, "%f %f %f\n", &temp_verts[v_idx++],
                                            &temp_verts[v_idx++],
                                            &temp_verts[v_idx++]);
            }
            else if (strcmp(token, "vt") == 0)
            {
                fscanf(fptr, "%f %f\n", &temp_uvs[vt_idx++],
                                        &temp_uvs[vt_idx++]);
            }
            else if (strcmp(token, "vn") == 0)
            {
                fscanf(fptr, "%f %f %f\n", &temp_norms[vn_idx++],
                                            &temp_norms[vn_idx++],
                                            &temp_norms[vn_idx++]);
            }
            else if (strcmp(token, "f") == 0)
            {
                s32 matches;
                matches = fscanf(fptr, "%d/%d/%d", &vertex_indices[f_idx],
                                                    &uv_indices[f_idx],
                                                    &normal_indices[f_idx++]);
                matches += fscanf(fptr, "%d/%d/%d", &vertex_indices[f_idx],
                                                    &uv_indices[f_idx],
                                                    &normal_indices[f_idx++]);
                matches += fscanf(fptr, "%d/%d/%d\n", &vertex_indices[f_idx],
                                                        &uv_indices[f_idx],
                                                        &normal_indices[f_idx++]);
                if (matches != 9)
                {
                    printf("Failed to read file! (faces): %s\n", file);
                    fclose(fptr);
                    return FALSE;
                }
            }
        }

        for (i = 0; i < nf; i++)
        {
            v_idx = vertex_indices[i];
            vt_idx = uv_indices[i];
            vn_idx = normal_indices[i];

            data->vertices[i] = temp_verts[v_idx];
            data->uvs[i] = temp_uvs[vt_idx];
            data->normals[i] = temp_norms[vn_idx];
        }
        data->interleaved = TRUE;

        return TRUE;
    }
}

 #endif // MHO_OBJ_IMPL

#pragma warning(default: 4996)

#endif // MHO_OBJ_H
