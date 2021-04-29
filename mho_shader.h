#ifndef MHO_SHADER_H
#define MHO_SHADER_H

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <types.h>

typedef enum
{
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    TESSELLATION,
    COMPUTE,
    PROGRAM
} m_shader_enum;

// Loads a vertex shader + fragment shader from a file source and
// creates a shader program
u32 m_load_shader(const char *vs_path, const char *fs_path);

// Loads a compute shader from a file source and creates a shader program
u32 m_load_compute_shader(const char *cs_path);

// Loads shader source from a file and returns a char * containing its data
char *m_load_shader_source(const char *filename);

// Checks for any shader compilation errors given shader data, shader type, and
// the originating filename
void m_check_compile_errors(u32 data, m_shader_enum type, const char *filename);

///////////////////////
////// IMPL ///////////

 #ifdef MHO_SHADER_IMPL

#pragma warning(disable: 4996)

char *
m_load_shader_source(const char *filename)
{
    FILE    *fptr;
    s32     file_len,
            ret;
    char    *source = NULL;

    fptr = fopen(filename, "rb");
    if (fptr)
    {
        fseek(fptr, 0, SEEK_END);
        file_len = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
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

u32
m_load_shader(const char *vs_path,
              const char *fs_path)
{
    u32     vertex,
            fragment,
            program;
    char    *shader_source;

    // Vertex Shader
    shader_source = m_load_shader_source(vs_path);
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &shader_source, NULL);
    glCompileShader(vertex);
    m_check_compile_errors(vertex, VERTEX, vs_path);
    free(shader_source);

    // Fragment Shader
    shader_source = m_load_shader_source(fs_path);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &shader_source, NULL);
    glCompileShader(fragment);
    m_check_compile_errors(fragment, FRAGMENT, fs_path);
    free(shader_source);

    // Shader Program
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    m_check_compile_errors(program, PROGRAM, NULL);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

u32
m_load_compute_shader(const char *cs_path)
{
    u32     compute,
            program;
    char    *shader_source;

    // Compute Shader
    shader_source = m_load_shader_source(cs_path);
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &shader_source, NULL);
    glCompileShader(compute);
    m_check_compile_errors(compute, COMPUTE, cs_path);
    free(shader_source);

    // Shader Program
    program = glCreateProgram();
    glAttachShader(program, compute);
    glLinkProgram(program);
    m_check_compile_errors(program, PROGRAM, NULL);

    glDeleteShader(compute);

    return program;
}

void
m_check_compile_errors(u32 data,
                     m_shader_enum type,
                     const char *filename)
{
    b32     success;
    char    info_log[1024];

    if (type == PROGRAM)
    {
        glGetProgramiv(data, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(data, 1024, NULL, info_log);
            printf("PROGRAM LINKING ERROR OF TYPE:\n %s\n\n", info_log);
        }
    }
    else if (type == VERTEX || type == FRAGMENT|| type == COMPUTE)
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

 #endif // MHO_SHADER_IMPL

#pragma warning(default: 4996)

#endif // MHO_SHADER_H
