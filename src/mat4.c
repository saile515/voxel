#include "mat4.h"

#include "math_util.h"
#include "vec3.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void mat4_create_identity_matrix(Mat4 out) {
  out[0] = 1;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  out[4] = 0;
  out[5] = 1;
  out[6] = 0;
  out[7] = 0;
  out[8] = 0;
  out[9] = 0;
  out[10] = 1;
  out[11] = 0;
  out[12] = 0;
  out[13] = 0;
  out[14] = 0;
  out[15] = 1;
}

void mat4_create_projection_matrix(Mat4 out, double fov, double aspect_ratio,
                                   double near, double far) {
  mat4_create_identity_matrix(out);

  double tangent = tan(fov / 2 * DEG_TO_RAD);

  out[0] = 1 / (tangent * aspect_ratio);
  out[5] = 1 / tangent;
  out[10] = -(far + near) / (far - near);
  out[11] = -1;
  out[14] = -(2 * far * near) / (far - near);
  out[15] = 0;
}

void mat4_from_transform(Mat4 out, const Transform *transform) {
  mat4_create_identity_matrix(out);

  mat4_scale(out, out, transform->scale);
  mat4_rotate(out, out, transform->rotation);
  mat4_translate(out, out, transform->position);
};

void mat4_scale(Mat4 out, const Mat4 matrix, const Vec3 scale) {
  Mat4 scaling_matrix = {0};

  scaling_matrix[0] = scale[0];
  scaling_matrix[5] = scale[1];
  scaling_matrix[10] = scale[2];
  scaling_matrix[15] = 1;

  mat4_multiply(out, scaling_matrix, matrix);
};

void mat4_rotate(Mat4 out, const Mat4 matrix, const Vec3 rotation) {
  Vec3 rotation_rad;
  vec3_multiply_double(rotation_rad, rotation, DEG_TO_RAD);

  double sinx = sin(rotation_rad[0]);
  double cosx = cos(rotation_rad[0]);
  double siny = sin(rotation_rad[1]);
  double cosy = cos(rotation_rad[1]);
  double sinz = sin(rotation_rad[2]);
  double cosz = cos(rotation_rad[2]);

  Mat4 rotation_matrix;
  mat4_create_identity_matrix(rotation_matrix);

  rotation_matrix[0] = cosy * cosz;
  rotation_matrix[1] = cosy * sinz;
  rotation_matrix[2] = -siny;
  rotation_matrix[4] = cosz * sinx * siny - cosx * sinz;
  rotation_matrix[5] = cosx * cosz + sinx * siny * sinz;
  rotation_matrix[6] = cosy * sinx;
  rotation_matrix[8] = cosx * cosz * siny + sinx * sinz;
  rotation_matrix[9] = cosx * siny * sinz - cosz * sinx;
  rotation_matrix[10] = cosx * cosy;

  mat4_multiply(out, rotation_matrix, matrix);
};

void mat4_translate(Mat4 out, const Mat4 matrix, const Vec3 position) {
  Mat4 translation_matrix;
  mat4_create_identity_matrix(translation_matrix);

  translation_matrix[12] = position[0];
  translation_matrix[13] = position[1];
  translation_matrix[14] = position[2];

  mat4_multiply(out, translation_matrix, matrix);
};

void mat4_multiply(Mat4 out, const Mat4 a, const Mat4 b) {
  Mat4 result;

  result[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
  result[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
  result[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
  result[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];
  result[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
  result[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
  result[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
  result[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];
  result[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
  result[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
  result[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
  result[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];
  result[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
  result[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
  result[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
  result[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];

  memcpy(out, result, 16 * sizeof(float));
}

void mat4_inverse(Mat4 out, const Mat4 m) {
  Mat4 inv;
  float determinant;

  inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] +
           m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
  inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] -
           m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
  inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] +
           m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
  inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -
           m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
  inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] -
           m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
  inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] +
           m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
  inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] -
           m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
  inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] +
           m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
  inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] +
           m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
  inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] -
           m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
  inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] +
            m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
  inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] -
            m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
  inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] -
            m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
  inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] +
            m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
  inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] -
            m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
  inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] +
            m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  determinant = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  if (determinant == 0) {
    return;
  }

  determinant = 1.0 / determinant;

  for (unsigned int i = 0; i < 16; i++) {
    out[i] = inv[i] * determinant;
  }
}

void mat4_print(const Mat4 matrix) {
  for (unsigned int i = 0; i < 4; i++) {
    printf("| %f %f %f %f |\n", matrix[i], matrix[i + 4], matrix[i + 8],
           matrix[i + 12]);
  }
  printf("\n");
}
