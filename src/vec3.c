#include "vec3.h"

#include <math.h>

void vec3_add(Vec3 out, const Vec3 a, const Vec3 b) {
  out[0] = a[0] + b[0];
  out[1] = a[1] + b[1];
  out[2] = a[2] + b[2];
}

void vec3_multiply_double(Vec3 out, const Vec3 vector, double scalar) {
  out[0] = vector[0] * scalar;
  out[1] = vector[1] * scalar;
  out[2] = vector[2] * scalar;
}

void vec3_normalize(Vec3 out, const Vec3 vector) {
  double length = vec3_length(vector);

  if (length == 0) {
    return;
  }

  vec3_multiply_double(out, vector, 1 / length);
}

double vec3_length(const Vec3 vector) {
  return sqrt(pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2));
}
