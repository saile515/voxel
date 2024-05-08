#pragma once

#include "transform.h"
#include "vec3.h"

typedef struct Mat4 {
  float values[16];
} Mat4;

Mat4 mat4_create_identity_matrix();

Mat4 mat4_create_projection_matrix(float fov, float aspect_ratio, float near,
                                   float far);

Mat4 mat4_from_transform(Transform *transform);

Mat4 mat4_scale(const Mat4 *matrix, Vec3 vector);

Mat4 mat4_rotate(const Mat4 *matrix, Vec3 vector);

Mat4 mat4_translate(const Mat4 *matrix, Vec3 vector);

Mat4 mat4_multiply(const Mat4 *matrix_a, const Mat4 *matrix_b);

Mat4 mat4_inverse(const Mat4 *matrix);

void mat4_print(const Mat4 *matrix);
