#include "read_file.h"

#include <stdio.h>
#include <stdlib.h>

const char *read_file(const char *path) {
  FILE *file = fopen(path, "rt");

  if (file == NULL) {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  unsigned int size = ftell(file);
  rewind(file);

  char *content = malloc(size + 1);
  fread(content, 1, size, file);

  content[size] = '\0';

  fclose(file);

  return content;
}
