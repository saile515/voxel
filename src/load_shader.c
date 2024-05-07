#include "load_shader.h"

#include "read_file.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int load_shader(const char *vertex_shader_path,
                         const char *fragment_shader_path) {
  unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  const char *vertex_shader_code = read_file(vertex_shader_path);
  const char *fragment_shader_code = read_file(fragment_shader_path);

  if (vertex_shader_code == NULL || fragment_shader_code == NULL) {
    return 0;
  }

  int result = GL_FALSE;
  int error_length;

  glShaderSource(vertex_shader_id, 1, &vertex_shader_code, NULL);
  glCompileShader(vertex_shader_id);

  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &error_length);

  if (error_length > 0) {
    char *error_message = malloc(error_length + 1);
    glGetShaderInfoLog(vertex_shader_id, error_length, NULL, error_message);

    printf("%s", error_message);
    return 0;
  }

  glShaderSource(fragment_shader_id, 1, &fragment_shader_code, NULL);
  glCompileShader(fragment_shader_id);

  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &error_length);

  if (error_length > 0) {
    char *error_message = malloc(error_length + 1);
    glGetShaderInfoLog(fragment_shader_id, error_length, NULL, error_message);

    printf("%s", error_message);
    return 0;
  }

  unsigned int program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glLinkProgram(program_id);

  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &error_length);

  if (error_length > 0) {
    char *error_message = malloc(error_length + 1);
    glGetShaderInfoLog(fragment_shader_id, error_length, NULL, error_message);

    printf("%s", error_message);
    return 0;
  }

  glDetachShader(program_id, vertex_shader_id);
  glDetachShader(program_id, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  return program_id;
};
