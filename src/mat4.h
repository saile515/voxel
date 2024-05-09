#pragma once

#include "transform.h"
#include "vec3.h"

typedef float Mat4[16];

void mat4_create_identity_matrix(Mat4 out);

void mat4_create_projection_matrix(Mat4 out, double fov, double aspect_ratio,
                                   double near, double far);

void mat4_from_transform(Mat4 out, const Transform *transform);

void mat4_scale(Mat4 out, const Mat4 matrix, const Vec3 scale);

void mat4_rotate(Mat4 out, const Mat4 matrix, const Vec3 rotation);

void mat4_translate(Mat4 out, const Mat4 matrix, const Vec3 position);

void mat4_multiply(Mat4 out, const Mat4 a, const Mat4 b);

void mat4_inverse(Mat4 out, const Mat4 m);

void mat4_print(const Mat4 matrix);
