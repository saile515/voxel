#include "vector.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MakeVectorDefinition(T)                                                \
  void vector_init_##T(Vector_##T *vector, unsigned int size) {                \
    vector->allocated_size = size;                                             \
    vector->size = 0;                                                          \
                                                                               \
    if (vector->data != NULL) {                                                \
      free(vector->data);                                                      \
    }                                                                          \
                                                                               \
    vector->data = malloc(vector->allocated_size * sizeof(T));                 \
  }                                                                            \
                                                                               \
  void vector_insert_##T(Vector_##T *vector, T value) {                        \
    if (vector->size >= vector->allocated_size) {                              \
      vector->allocated_size *= 2;                                             \
      vector->data =                                                           \
          realloc(vector->data, vector->allocated_size * sizeof(T));           \
    }                                                                          \
                                                                               \
    vector->data[vector->size] = value;                                        \
    vector->size++;                                                            \
  }                                                                            \
                                                                               \
  T vector_get_##T(const Vector_##T *vector, unsigned int index) {             \
    return vector->data[index];                                                \
  }                                                                            \
                                                                               \
  void vector_free_##T(Vector_##T *vector) { free(vector->data); }

MakeVectorDefinition(float);
