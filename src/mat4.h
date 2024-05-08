#pragma once

#include "vec3.h"

typedef struct Mat4 {
  float values[16];
} Mat4;

Mat4 mat4_create_identity_matrix();

Mat4 mat4_create_projection_matrix(float fov, float aspect_ratio, float near,
                                   float far);

Mat4 mat4_translate(const Mat4 *matrix, Vec3 vector);

Mat4 mat4_multiply(const Mat4 *matrix_a, const Mat4 *matrix_b);

void mat4_print(const Mat4 *matrix);
