#include "mat4.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

Mat4 mat4_create_identity_matrix() {
  Mat4 matrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  return matrix;
}

Mat4 mat4_create_projection_matrix(float fov, float aspect_ratio, float near,
                                   float far) {
  Mat4 matrix = {0};

  float tangent = tan(fov * M_PI / 180 / 2);
  float right = near * tangent;
  float top = right / aspect_ratio;

  matrix.values[0] = near / right;
  matrix.values[5] = near / top;
  matrix.values[10] = -(far + near) / (far - near);
  matrix.values[11] = -1;
  matrix.values[14] = -(2 * far * near) / (far - near);

  return matrix;
}

Mat4 mat4_translate(const Mat4 *matrix, Vec3 vector) {
  Mat4 translation_matrix = *matrix;

  translation_matrix.values[12] = vector.values[0];
  translation_matrix.values[13] = vector.values[1];
  translation_matrix.values[14] = vector.values[2];

  return mat4_multiply(&translation_matrix, matrix);
};

Mat4 mat4_multiply(const Mat4 *matrix_a, const Mat4 *matrix_b) {
  Mat4 result = {0};

  float a[16];
  memcpy(&a, &matrix_a->values, 16 * sizeof(float));
  float b[16];
  memcpy(&b, &matrix_b->values, 16 * sizeof(float));
  float(*c)[16] = &result.values;

  (*c)[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
  (*c)[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
  (*c)[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
  (*c)[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];
  (*c)[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
  (*c)[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
  (*c)[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
  (*c)[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];
  (*c)[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
  (*c)[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
  (*c)[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
  (*c)[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];
  (*c)[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
  (*c)[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
  (*c)[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
  (*c)[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];

  return result;
}

void mat4_print(const Mat4 *matrix) {
  for (unsigned int i = 0; i < 16; i += 4) {
    printf("| %f %f %f %f |\n", matrix->values[i], matrix->values[i + 1],
           matrix->values[i + 2], matrix->values[i + 3]);
  }
  printf("\n");
}
