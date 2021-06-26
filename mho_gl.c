#include <stdlib.h>
#include "mho.h"

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
    u32     vertex,
            fragment,
            program;
    char    *source;

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
    u32     compute,
            program;
    char    *source;

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
    b32     success;
    char    info_log[1024];

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
/*                            s32 width, */
/*                            s32 height) */
/* { */
/*  glViewport(0, 0, width, height); */
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
