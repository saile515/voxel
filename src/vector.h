#pragma once

#include <stdint.h>

#define MakeVectorDeclaration(T)                                               \
  typedef struct Vector_##T {                                                  \
    T *data;                                                                   \
    unsigned int size;                                                         \
    unsigned int allocated_size;                                               \
  } Vector_##T;                                                                \
                                                                               \
  void vector_init_##T(Vector_##T *vector, unsigned int size);                 \
                                                                               \
  void vector_insert_##T(Vector_##T *vector, T value);                         \
                                                                               \
  T vector_get_##T(const Vector_##T *vector, unsigned int index);              \
                                                                               \
  void vector_free_##T(Vector_##T *vector);

MakeVectorDeclaration(float);
