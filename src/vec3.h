#pragma once

#include <stdbool.h>

typedef double Vec3[3];

typedef int Vec3i[3];

void vec3_add(Vec3 out, const Vec3 a, const Vec3 b);

void vec3_multiply_double(Vec3 out, const Vec3 vector, double scalar);

void vec3_normalize(Vec3 out, const Vec3 vector);

double vec3_length(const Vec3 vector);

void vec3_copy(Vec3 out, const Vec3 vector);

bool vec3_compare(const Vec3 a, const Vec3 b);
