#include "mat4.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

float deg_to_rad = M_PI / 180;

Mat4 mat4_create_identity_matrix() {
  Mat4 matrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  return matrix;
}

Mat4 mat4_create_projection_matrix(float fov, float aspect_ratio, float near,
                                   float far) {
  Mat4 matrix = {0};

  float tangent = tan(fov * deg_to_rad / 2);
  float right = near * tangent;
  float top = right / aspect_ratio;

  matrix.values[0] = near / right;
  matrix.values[5] = near / top;
  matrix.values[10] = -(far + near) / (far - near);
  matrix.values[11] = -1;
  matrix.values[14] = -(2 * far * near) / (far - near);

  return matrix;
}

Mat4 mat4_from_transform(Transform *transform) {
  Mat4 matrix = mat4_create_identity_matrix();

  matrix = mat4_scale(&matrix, transform->scale);
  matrix = mat4_translate(&matrix, transform->position);
  matrix = mat4_rotate(&matrix, transform->rotation);

  return matrix;
};

Mat4 mat4_scale(const Mat4 *matrix, Vec3 vector) {
  Mat4 scaling_matrix = {0};

  scaling_matrix.values[0] = vector.values[0];
  scaling_matrix.values[5] = vector.values[1];
  scaling_matrix.values[10] = vector.values[2];
  scaling_matrix.values[15] = 1;

  return mat4_multiply(&scaling_matrix, matrix);
};

Mat4 mat4_rotate(const Mat4 *matrix, Vec3 vector) {
  Mat4 rotation_matrix_x = mat4_create_identity_matrix();

  vector.values[0] *= deg_to_rad;
  vector.values[1] *= deg_to_rad;
  vector.values[2] *= deg_to_rad;

  rotation_matrix_x.values[5] = cos(vector.values[0]);
  rotation_matrix_x.values[6] = -sin(vector.values[0]);
  rotation_matrix_x.values[9] = sin(vector.values[0]);
  rotation_matrix_x.values[10] = cos(vector.values[0]);

  Mat4 rotation_matrix_y = mat4_create_identity_matrix();

  rotation_matrix_y.values[0] = cos(vector.values[1]);
  rotation_matrix_y.values[2] = sin(vector.values[1]);
  rotation_matrix_y.values[8] = -sin(vector.values[1]);
  rotation_matrix_y.values[10] = cos(vector.values[1]);

  Mat4 rotation_matrix_z = mat4_create_identity_matrix();

  rotation_matrix_z.values[0] = cos(vector.values[2]);
  rotation_matrix_z.values[1] = -sin(vector.values[2]);
  rotation_matrix_z.values[4] = sin(vector.values[2]);
  rotation_matrix_z.values[5] = cos(vector.values[2]);

  Mat4 x_rotation = mat4_multiply(&rotation_matrix_x, matrix);
  Mat4 y_rotation = mat4_multiply(&rotation_matrix_y, &x_rotation);

  return mat4_multiply(&rotation_matrix_z, &y_rotation);
};

Mat4 mat4_translate(const Mat4 *matrix, Vec3 vector) {
  Mat4 translation_matrix = mat4_create_identity_matrix();

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

Mat4 mat4_inverse(const Mat4 *matrix) {
  float inv[16];
  float determinant;

  float m[16];
  memcpy(&m, &matrix->values, 16 * sizeof(float));

  inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] +
           m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

  inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] -
           m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

  inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] +
           m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

  inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] -
            m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

  inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] -
           m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

  inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] +
           m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

  inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] -
           m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

  inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] +
            m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

  inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] +
           m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

  inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] -
           m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

  inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] +
            m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

  inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] -
            m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

  inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -
           m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

  inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] +
           m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

  inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] -
            m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

  inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] +
            m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  determinant = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  if (determinant == 0)
    return mat4_create_identity_matrix();

  determinant = 1.0 / determinant;

  Mat4 inversed_matrix;

  for (unsigned int i = 0; i < 16; i++)
    inversed_matrix.values[i] = inv[i] * determinant;

  return inversed_matrix;
}

void mat4_print(const Mat4 *matrix) {
  for (unsigned int i = 0; i < 16; i += 4) {
    printf("| %f %f %f %f |\n", matrix->values[i], matrix->values[i + 1],
           matrix->values[i + 2], matrix->values[i + 3]);
  }
  printf("\n");
}
