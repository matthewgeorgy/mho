#include "mho.h"
#include <math.h>

/////////////////////////////
// VECTOR2D IMPLEMENTATION
/////////////////////////////

mho_vec2_t
mho_vec2_ctor(f32 x,
              f32 y)
{
    mho_vec2_t  vec;

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
    f32     val;

    val = mho_fsqrtinv((vec.x * vec.x) + (vec.y * vec.y));
    vec.x *= val;
    vec.y *= val;

    return vec;
}

mho_vec2_t
mho_vec2_rotate(mho_vec2_t vec,
                f32 angle)
{
    f32     c,
            s;

    c = mho_cos(mho_rads(angle));
    s = mho_sin(mho_rads(angle));
    vec.x = ((vec.x * c) - (vec.y * s));
    vec.y = ((vec.x * s) + (vec.y * c));

    return vec;
}

mho_vec2_t
mho_vec2_pmul(mho_vec2_t v1,
              mho_vec2_t v2)
{
    mho_vec2_t      res;

    res.x = v1.x * v2.x;
    res.y = v1.y * v2.y;

    return res;
}

/////////////////////////////
// VECTOR3D IMPLEMENTATION
/////////////////////////////

mho_vec3_t
mho_vec3_ctor(f32 x,
              f32 y,
              f32 z)
{
    mho_vec3_t  vec;

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
    mho_vec3_t  cross_prod;

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
    f32     val;

    val = mho_fsqrtinv((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
    vec.x *= val;
    vec.y *= val;
    vec.z *= val;

    return vec;
}

mho_vec3_t
mho_vec3_pmul(mho_vec3_t v1,
              mho_vec3_t v2)
{
    mho_vec3_t      res;

    res.x = v1.x * v2.x;
    res.y = v1.y * v2.y;
    res.z = v1.z * v2.z;

    return res;
}

/////////////////////////////
// VECTOR4D IMPLEMENTATION
/////////////////////////////

mho_vec4_t
mho_vec4_ctor(f32 x,
              f32 y,
              f32 z,
              f32 w)
{
    mho_vec4_t  vec;

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
    f32     val;

    val = mho_fsqrtinv((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w));
    vec.x *= val;
    vec.y *= val;
    vec.z *= val;
    vec.w *= val;

    return vec;
}

mho_vec4_t
mho_vec4_pmul(mho_vec4_t v1,
              mho_vec4_t v2)
{
    mho_vec4_t      res;

    res.x = v1.x * v2.x;
    res.y = v1.y * v2.y;
    res.z = v1.z * v2.z;
    res.w = v1.w * v2.w;

    return res;
}

//////////////////////////////
// MATRIX4 IMPLEMENTATION
//////////////////////////////

mho_mat4_t
mho_mat4_identity(void)
{
    mho_mat4_t  matrix = {0};

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
    mho_mat4_t  matrix;

    matrix = mho_mat4_identity();
    matrix.col4.x = x;
    matrix.col4.y = y;
    matrix.col4.z = z;

    return matrix;
}

mho_mat4_t
mho_mat4_translate_v(mho_vec3_t vec)
{
    mho_mat4_t  matrix;

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
    u8  i,
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
    f32         c,
                s,
                c1;
    mho_vec3_t  vec = {x, y, z};
    mho_mat4_t  matrix = {0};

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

    f32         c,
                s,
                c1;
    mho_mat4_t  matrix = {0};

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
    f32         t,
                fdelta;
    mho_mat4_t  matrix = {0};

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
    mho_mat4_t  matrix;
    mho_vec3_t  f,
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
    mho_mat4_t  matrix = {0};

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
    mho_mat4_t  res;
    u8          y,
                x,
                e;
    f32         sum;

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
//////////////////////////////

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
    f32     x,
            y;
    s32     i;

    x = number * 0.5f;
    y = number;
    i = *(s32 *)&y;                 // evil floating point bit hack
    i = 0x5F3759DF - (i >> 1);      // what the fuck?
    y = *(f32 *)&i;
    y = y * (1.5f - (x * y * y));   // 1st iteration
    y = y * (1.5f - (x * y * y));   // 2nd iteration

    return number * y;              // mulitply by original num to reverse and get sqrt
}

f32
mho_fsqrtinv(f32 number)
{
    f32     x,
            y;
    s32     i;

    x = number * 0.5f;
    y = number;
    i = *(s32 *)&y;                 // evil floating point bit hack
    i = 0x5F3759DF - (i >> 1);      // what the fuck?
    y = *(f32 *)&i;
    y = y * (1.5f - (x * y * y));   // 1st iteration
    y = y * (1.5f - (x * y * y));   // 2nd iteration

    return y;
}

f32
mho_noise2f(f32 x,
			f32 y)
{
	f32 a, b, c, d;
	s32 xi, yi;

	x += 256.0;
	xi = (s32)x;
	x -= (f32)xi;

	y += 4096.0;
	yi = (s32)y;
	y -= (f32)yi;
	yi *= 11;

	x = (3.0 * x * x - 2.0 * x * x * x);
	y = (3.0 * y * y - 2.0 * y * y * y);

	a = mho_randnf((u32)xi + yi);
	b = mho_randnf((u32)xi + yi + 1);

	c = mho_lerp(a, b, x);
	/* c = a + (b - a) * x; */

	a = mho_randnf((u32)xi + yi + 11);
	b = mho_randnf((u32)xi + yi + 12);

	d = mho_lerp(a, b, x);
	/* d = a + (b - a) * x; */

	return mho_lerp(c, d, y);
	/* return c + (d - c) * y; */
}
