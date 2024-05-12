#include "vector.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MakeVectorDefinition(T)                                                \
  void vector_init_##T(Vector_##T *vector, unsigned int size) {                \
    vector->allocated_size = size;                                             \
    vector->size = 0;                                                          \
                                                                               \
    if (vector->data != NULL) {                                                \
      free(vector->data);                                                      \
    }                                                                          \
                                                                               \
    vector->data = calloc(vector->allocated_size, sizeof(T));                  \
  }                                                                            \
                                                                               \
  void vector_insert_##T(Vector_##T *vector, T value) {                        \
    if (vector->size >= vector->allocated_size) {                              \
      vector->allocated_size *= 2;                                             \
      T *current_data = vector->data;                                          \
      vector->data = calloc(vector->allocated_size, sizeof(T));                \
      memcpy(vector->data, current_data,                                       \
             vector->allocated_size / 2 * sizeof(T));                          \
      free(current_data);                                                      \
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
