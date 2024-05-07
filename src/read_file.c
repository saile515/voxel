#include "read_file.h"

#include <stdio.h>
#include <stdlib.h>

const char *read_file(const char *path) {
  FILE *file = fopen(path, "r");

  if (file == NULL) {
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  unsigned int size = ftell(file);
  rewind(file);

  char *content = malloc(size);
  fread(content, 1, size, file);

  return content;
}
